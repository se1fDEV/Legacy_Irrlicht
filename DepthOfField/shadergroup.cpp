#include "shadergroup.h"



class ShaderCallBack : public video::IShaderConstantSetCallBack
{
public:
    ShaderGroup *callback;
    ShaderCallBack(ShaderGroup *c)
    {
        callback=c;
    };
    virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
    {
		video::IVideoDriver* driver = services->getVideoDriver();
	 {
			
			if (userData == 1) {
				services->setPixelShaderConstant("sampleDist0", &callback->sampleDist0, 1);
			}
			if (userData == 3) {
				services->setPixelShaderConstant("focus", &callback->focus, 1);
				services->setPixelShaderConstant("range", &callback->range, 1);
			}
			if (userData == 0) {
				services->setVertexShaderConstant("distanceScale", &callback->distanceScale, 1);
				core::matrix4 worldViewProj;
				worldViewProj = driver->getTransform(video::ETS_PROJECTION);			
				worldViewProj *= driver->getTransform(video::ETS_VIEW);
				worldViewProj *= driver->getTransform(video::ETS_WORLD);
				services->setVertexShaderConstant("gl_ModelViewProjectionMatrix", &worldViewProj[0], 16);
				core::matrix4 worldView;
				worldView = driver->getTransform(video::ETS_VIEW);
				worldView *= driver->getTransform(video::ETS_WORLD);
				services->setVertexShaderConstant("gl_ModelViewMatrix", &worldView[0], 16);
			}
		}
    }
};



ShaderGroup::ShaderGroup(IrrlichtDevice* m_device, scene::ISceneManager* m_smgr)
{
    range = 4.5f;  
    focus = .20f;  
    sampleDist0 = 0.0120f;
    sampleDist1 = 0.015f; 
    distanceScale = .0007f;

    device = m_device;
    smgr = m_smgr;
    video::IVideoDriver* driver = device->getVideoDriver();

    postProcessorNode = smgr->addCubeSceneNode(60.0);
    postProcessorNode->setMaterialFlag(video::EMF_LIGHTING, false);

    rt = driver->addRenderTargetTexture(core::dimension2d<u32>(512,512));

    gpu = driver->getGPUProgrammingServices();
    ShaderCallBack* mc = new ShaderCallBack(this);

	c8 *vertPost, *vertRender;
	c8 *fragRender, *fragBlur0, *fragBlur1, *fragCombine;

	video::E_DRIVER_TYPE driverType = driver->getDriverType();
	{
		
		vertPost	= "./shaders/post.vs";
		vertRender	= "./shaders/render.vs";
		fragRender	= "./shaders/render.ps";
		fragBlur0	= "./shaders/blur0.ps";
		fragBlur1	= "./shaders/blur1.ps";
		fragCombine	= "./shaders/combine.ps";
	}

    distance = new ShaderMaterial(device, vertRender, fragRender, 0, mc);
    blur0 = new ShaderMaterial(device, vertPost, fragBlur0, 1, mc);
    blur1 = new ShaderMaterial(device, vertPost, fragBlur1, 2, mc);
    combine = new ShaderMaterial(device, vertPost, fragCombine, 3, mc);
}

ShaderGroup::~ShaderGroup() {}

void ShaderGroup::add(scene::ISceneNode* node)
{
    shaded.push_back(node);
}

int ShaderGroup::render()
{
    scene::ICameraSceneNode* camera = smgr->getActiveCamera();
    video::IVideoDriver* driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

    driver->beginScene(true, true, 0);

    postProcessorNode->setVisible(false);
    driver->setRenderTarget(rt, true, true, video::SColor(0,0,0,0));
    smgr->drawAll();
    driver->setRenderTarget(distance->texture, true, true, video::SColor(0,0,0,0));
    list<scene::ISceneNode*>::const_iterator shadedIter;
    for(shadedIter=shaded.begin();shadedIter!=shaded.end(); ++shadedIter)
    {
        scene::ISceneNode* node = (*shadedIter);
        video::E_MATERIAL_TYPE tempMat = node->getMaterial(0).MaterialType;
        node->setMaterialType((video::E_MATERIAL_TYPE) distance->material);
        node->render();
        node->setMaterialType(tempMat);

    }
    postProcessorNode->setVisible(true);

    driver->setRenderTarget(blur0->texture, true, true, video::SColor(0,0,0,0));
    postProcessorNode->setMaterialTexture(0, rt);
    postProcessorNode->setMaterialType((video::E_MATERIAL_TYPE) blur0->material);
    postProcessorNode->render();

    driver->setRenderTarget(blur1->texture, true, true, video::SColor(0,0,0,0));
    postProcessorNode->setMaterialTexture(0, blur0->texture);
    postProcessorNode->setMaterialType((video::E_MATERIAL_TYPE) blur1->material);
    postProcessorNode->render();

    driver->setRenderTarget(0);
    postProcessorNode->setMaterialTexture(0, rt);
    postProcessorNode->setMaterialTexture(1, distance->texture);
    postProcessorNode->setMaterialTexture(2, blur0->texture);
    postProcessorNode->setMaterialTexture(3, blur1->texture);

    postProcessorNode->setMaterialType((video::E_MATERIAL_TYPE) combine->material);
    postProcessorNode->render();

	env->drawAll();

    driver->endScene();

    return 1;
}
