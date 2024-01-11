
#include <irrlicht.h>
#include <iostream>
#include <IGUIInOutFader.h>

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

bool g_bQuit = false;
bool temp=1;

IrrlichtDevice* device = 0;
// Flagged "true" when MessageBox is closed
bool g_bMessageBoxClosed = false;
// ID of message box
#define MESSAGEBOX_ID 1

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		//GUI events
		if (event.EventType == EET_GUI_EVENT)
		{
			s32 id = event.GUIEvent.Caller->getID();
			// Test for messagebox
			if (id == MESSAGEBOX_ID) {
				if ((event.GUIEvent.EventType == gui::EGET_MESSAGEBOX_OK) || 
					(event.GUIEvent.EventType == gui::EGET_MESSAGEBOX_CANCEL))
				{
					// MesageBox OK button was clicked on "OK" or closed(x)
					g_bMessageBoxClosed = true;
				}
			}
		}

		if (event.EventType == irr::EET_KEY_INPUT_EVENT )
		{
			switch(event.KeyInput.Key)
			{
			case KEY_ESCAPE:
				g_bQuit = true;
				return true;
			}
		}
		return false;
	}
};



class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:

	virtual void OnSetConstants(video::IMaterialRendererServices* services,
			s32 userData)
	{
		video::IVideoDriver* driver = services->getVideoDriver();

		// set inverted world matrix
		
        core::matrix4 worldViewProj;
				worldViewProj = driver->getTransform(video::ETS_PROJECTION);			
				worldViewProj *= driver->getTransform(video::ETS_VIEW);
				worldViewProj *= driver->getTransform(video::ETS_WORLD);
		worldViewProj = worldViewProj.getTransposed();
        services->setVertexShaderConstant("view_proj_matrix", &worldViewProj[0], 16);
		
		core::matrix4 invWorld = driver->getTransform(video::ETS_VIEW);
		invWorld.makeInverse();

		
			services->setVertexShaderConstant("view_matrix", invWorld.pointer(), 16);
	

	
		

		// set lightDir

		video::SColorf Dir(0.0f,0.0f,1.0f,0.0f);

		
			services->setVertexShaderConstant("lightDir",
					reinterpret_cast<f32*>(&Dir), 4);
		
		video::SColorf Dir2(0.0f,0.0f,0.0f,0.0f);

		
		//	services->setVertexShaderConstant("MiscParam",
		//			reinterpret_cast<f32*>(&Dir2), 4);
	


	}
};


int main()
{
	

	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
MyEventReceiver receiver;

	// create device
device = createDevice(driverType,core::dimension2d<u32>(1024, 768), 32, false, true, true, &receiver);

gui::IGUIInOutFader* fader = device->getGUIEnvironment()->addInOutFader();
        fader->setColor(video::SColor(0,0,0,0));
        fader->fadeIn(10000); 


	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* gui = device->getGUIEnvironment();

	

   io::path vsFileName; // filename for the vertex shader
   io::path psFileName; // filename for the pixel shader


			psFileName = "./data/shader/pixel";
	    	vsFileName = "./data/shader/vertex"; 

	
gui::IGUISkin* skin = gui->getSkin();
	gui::IGUIFont* font = gui->getFont("fonts/font.png");
	if (font)
		skin->setFont(font);

	SKeyMap keyMap[8] = {
		// WASD navigation
		{EKA_MOVE_FORWARD,	KEY_KEY_W},
		{EKA_MOVE_BACKWARD,	KEY_KEY_S},
		{EKA_STRAFE_LEFT,	KEY_KEY_A},
		{EKA_STRAFE_RIGHT,	KEY_KEY_D},
		// Cursor-key navigation
		{EKA_MOVE_FORWARD,	KEY_UP},
		{EKA_MOVE_BACKWARD,	KEY_DOWN},
		{EKA_STRAFE_LEFT,	KEY_LEFT},
		{EKA_STRAFE_RIGHT,	KEY_RIGHT},
	};
	

	// create materials

	video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();
	s32 newMaterialType1 = 0;
	

	if (gpu)
	{
		MyShaderCallBack* mc = new MyShaderCallBack();

		
	
		
			

			newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
				vsFileName, "vs_main", video::EVST_VS_2_0,
				psFileName, "ps_main", video::EPST_PS_2_0,
				mc, video::EMT_SOLID);
			//:EMT_TRANSPARENT_ADD_COLOR);


		mc->drop();
	}



	// create test scene node 1, with the new created material type 1
scene::ISceneNode* test_scene = smgr->addOctTreeSceneNode(smgr->getMesh("./data/3ds/prim.3ds"));
	test_scene->setPosition( irr::core::vector3df(0,0,0) );
	test_scene->setScale( irr::core::vector3df(30.f,30.f,30.f) );
	test_scene->setRotation( irr::core::vector3df(0,0,0) );

	test_scene->setMaterialFlag(video::EMF_LIGHTING, false);

	//pig->getMaterial(0).setTexture(0, driver->getTexture("1.bmp"));                                                             
test_scene->getMaterial(0).setTexture(0, driver->getTexture("./data/texture/Hatch0.png"));
test_scene->getMaterial(0).setTexture(1, driver->getTexture("./data/texture/Hatch1.png"));
test_scene->getMaterial(0).setTexture(2, driver->getTexture("./data/texture/Hatch2.png"));
test_scene->getMaterial(0).setTexture(3, driver->getTexture("./data/texture/Hatch3.png"));
test_scene->getMaterial(0).setTexture(4, driver->getTexture("./data/texture/Hatch4.png"));
test_scene->getMaterial(0).setTexture(5, driver->getTexture("./data/texture/Hatch5.png"));
	
/*	test_scene->setMaterialTexture(0, driver->getTexture("./data/texture/Hatch0"));
    test_scene->setMaterialTexture(1, driver->getTexture("./data/texture/Hatch1"));
	test_scene->setMaterialTexture(2, driver->getTexture("./data/texture/Hatch2"));
	test_scene->setMaterialTexture(3, driver->getTexture("./data/texture/Hatch3"));
	test_scene->setMaterialTexture(4, driver->getTexture("./data/texture/Hatch4"));
    test_scene->setMaterialTexture(5, driver->getTexture("./data/texture/Hatch5"));
  */ test_scene->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
	// add a skybox

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	smgr->addSkyBoxSceneNode(
		driver->getTexture("./data/skybox/rays_up.png"),
		driver->getTexture("./data/skybox/rays_down.png"),
		driver->getTexture("./data/skybox/rays_south.png"),
		driver->getTexture("./data/skybox/rays_north.png"),
		driver->getTexture("./data/skybox/rays_west.png"),
		driver->getTexture("./data/skybox/rays_east.png"));

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// add a camera and disable the mouse cursor

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.2f, -1, keyMap, 8);
	cam->setPosition(core::vector3df(0,50,50));
	cam->setTarget(core::vector3df(0,0,0));
	cam->setFarValue(42000.0f);
	device->getCursorControl()->setVisible(true);

	/*
	Now draw everything. That's all.
	*/


	int lastFPS = -1;
/////////////////
	// Message 
	wchar_t *caption = L"Demo4. Hatch.";
	wchar_t *text = 
		L"Hatch\n\n"
		L"\nKeys:\n"
		L"   WASD/Cursor keys - Move camera\n"
		L"   Mouse - Orientate camera\n"
		L"   Escape - Exit demo\n";
	gui::IGUIWindow *messageBox = gui->addMessageBox(caption, text, true, gui::EMBF_OK, 0, MESSAGEBOX_ID);
	messageBox->setRelativePosition(core::position2d<s32>(265,100));
	cam->setInputReceiverEnabled(false);
int temp0=1;
core::vector2d<s32>pos;
	//////////////////////

while(!g_bQuit && device->run()) {
		if (device->isWindowActive()) {
			
		if (g_bMessageBoxClosed && temp) 
		        {
				device->getCursorControl()->setPosition(pos);
				cam->setInputReceiverEnabled(true);
				device->getCursorControl()->setVisible(false);
				fader->remove();
		        temp=0;
			    }
        
		driver->beginScene(true, true, video::SColor(255,0,0,0));
		smgr->drawAll();
		gui->drawAll();
		driver->endScene();
        if (temp0){pos=device->getCursorControl()->getPosition(); temp0=0;}
		int fps = driver->getFPS();
        
		if (lastFPS != fps)
		{
			core::stringw str = L"DEMO3. Shaders_toon [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		}}

	device->drop();

	return 0;
}
