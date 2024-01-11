#include <irrlicht.h>
#include <IGUIInOutFader.h>

using namespace irr;
using namespace video;
using namespace scene;
using namespace core;
using namespace io;
using namespace gui;

#pragma comment(lib,"irrlicht.lib")
bool g_bQuit = false;
bool temp=1;

//IrrlichtDevice* device = 0;
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

class CST : public irr::video::IShaderConstantSetCallBack
{
public:
	
	CST()
	{   MyEventReceiver receiver;
		device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(1024, 768), 32,false,true,true,&receiver);
		driver = device->getVideoDriver();
		smgr = device->getSceneManager();
		gui::IGUIEnvironment* gui = device->getGUIEnvironment();

gui::IGUIInOutFader* fader = device->getGUIEnvironment()->addInOutFader();
        fader->setColor(video::SColor(0,0,0,0));
        fader->fadeIn(10000); 

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

		camera = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.01f,-1, keyMap, 8);
		camera->setPosition(vector3df(50.0f, 10.0f, 50.0f));
		camera->setTarget(core::vector3df(0,30,0));
		device->getCursorControl()->setVisible(true);

		ISceneNode* pig = smgr->addMeshSceneNode(smgr->getMesh("pig.3ds"));
		pig->setScale(core::vector3df(0.3f, 0.2f, 0.3f));
		pig->getMaterial(0).setTexture(0, driver->getTexture("1.bmp"));
		pig->getMaterial(0).setTexture(1, driver->getTexture("cel.png"));
		pig->getMaterial(0).TextureLayer[1].BilinearFilter = false;

		

		video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();

		s32 celMat = gpu->addHighLevelShaderMaterialFromFiles(
			"cel.hlsl", "vertexMain", EVST_VS_2_0,
			"cel.hlsl", "pixelMain", EPST_PS_2_0, this);
		IGUISkin* skin = gui->getSkin();
	IGUIFont* font = gui->getFont("font.png");
	if (font)
		skin->setFont(font);

	
	

		pig->setMaterialType((video::E_MATERIAL_TYPE)celMat);
		//pig->setMaterialFlag(video::EMF_LIGHTING, false);
		lightPosition = vector3df(10, 10, 10);
int lastFPS = -1;
/////////////////
	// Message 
	wchar_t *caption = L"Demo6. Cell shading.";
	wchar_t *text = 
		L"Cell shading\n"
		L"\Pink pig\n"
		L"\nKeys:\n"
		L"   WASD/Cursor keys - Move camera\n"
		L"   Mouse - Orientate camera\n"
		L"   Escape - Exit demo\n";
	IGUIWindow *messageBox = gui->addMessageBox(caption, text, true, gui::EMBF_OK, 0, MESSAGEBOX_ID);
	messageBox->setRelativePosition(core::position2d<s32>(385,200));
	camera->setInputReceiverEnabled(false);
int temp0=1;
core::vector2d<s32>pos;
	//////////////////////



		while(!g_bQuit && device->run()) {
		if (device->isWindowActive()) {
			
		if (g_bMessageBoxClosed && temp) 
		        {
				device->getCursorControl()->setPosition(pos);
				camera->setInputReceiverEnabled(true);
				device->getCursorControl()->setVisible(false);
				fader->remove();
		        temp=0;
			    }

			driver->beginScene(true, true, SColor(0xff88aada));
			smgr->drawAll();
			gui->drawAll();
			driver->endScene();
			 if (temp0){pos=device->getCursorControl()->getPosition(); temp0=0;}
int fps = driver->getFPS();
			if (lastFPS != fps)
		{
			core::stringw str = L"DEMO6. Cell shading, Pig [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		}}
	}

	~CST()
	{
		if(device)
		{
			device->closeDevice();
			device->run();
			device->drop();
			device = NULL;
		}
	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		matrix4 world = driver->getTransform(ETS_WORLD);

		matrix4 invWorld;
		world.getInverse(invWorld);

		vector3df lightPosOS;
		invWorld.transformVect(lightPosOS, lightPosition);
		services->setVertexShaderConstant("mLightPos", &lightPosOS.X, 3);

		vector3df camPosOS;
		invWorld.transformVect(camPosOS, camera->getPosition());
		services->setVertexShaderConstant("mCamPos", &camPosOS.X, 3);
	
		matrix4 wvp = driver->getTransform(ETS_PROJECTION);
		wvp *= driver->getTransform(ETS_VIEW);
		wvp *= world;

		services->setVertexShaderConstant("mWorldViewProj", wvp.pointer(), 16);
	}

	ICameraSceneNode* camera;
	IVideoDriver* driver;
	ISceneManager* smgr;
	IrrlichtDevice* device;
	vector3df lightPosition;
};

int main()
{
	CST app;
	return 0;
}
