#include "shadermaterial.h"
#include "shadergroup.h"
#include <irrlicht.h>
#include <iostream>
#include <string>
#include <IGUIInOutFader.h>
using namespace irr;
#pragma comment(lib, "Irrlicht.lib")

IrrlichtDevice *device = 0;
ShaderGroup* hdr = 0;

bool g_bQuit = false;
bool temp=1;
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
			case KEY_F1:
				hdr->range += .1f;
				return true;
			case KEY_F2:
				hdr->range -= .1f;
				return true;
			case KEY_F3:
				hdr->focus += .02f;
				return true;
			case KEY_F4:
				hdr->focus -= .02f;
				return true;
			case KEY_F5:
				hdr->sampleDist0 += .0001f;
				hdr->sampleDist1 += .0001f;
				return true;
			case KEY_F6:
				hdr->sampleDist0 -= .0001f;
				hdr->sampleDist1 -= .0001f;
				return true;
			case KEY_F7:
				hdr->distanceScale += .00001f;
				return true;
			case KEY_F8:
				hdr->distanceScale -= .00001f;
				return true;
			}
		}
		return false;
	}
};


int main()
{
	//video Direct
	video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;



	MyEventReceiver receiver;

	//Display resolution
	core::dimension2d<u32> windowSize(1024,768);
                                               //bit, FScr,stencilB,vsyc,
	device = createDevice(driverType, windowSize, 32, false, true, true, &receiver);

	if (device == 0)
		return 1; // could not create selected driver.

gui::IGUIInOutFader* fader = device->getGUIEnvironment()->addInOutFader();
        fader->setColor(video::SColor(0,255,255,255));
        fader->fadeIn(8000);


	scene::ISceneManager *smgr = device->getSceneManager();
	video::IVideoDriver *driver = device->getVideoDriver();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	// Set a font
	gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont* font = env->getFont("fonts/font.png");
	if (font)
		skin->setFont(font);

	// Create a camera
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

	scene::ICameraSceneNode* cam = smgr->addCameraSceneNodeFPS(0, 100.0f, 0.2f, -1, keyMap, 8);
	cam->setPosition(core::vector3df(-280.0f,140.0f,540.0f));
	cam->setTarget(core::vector3df(0,100,400));
	cam->setFarValue(8000.0f);

	// Enable mouse pointer when click OK
	device->getCursorControl()->setVisible(true);


// sky_box
driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	scene::ISceneNode* skybox=smgr->addSkyBoxSceneNode(
		driver->getTexture("./data/skybox/blizzard_up.tga"),
		driver->getTexture("./data/skybox/blizzard_dn.tga"),
		driver->getTexture("./data/skybox/blizzard_rt.tga"),
		driver->getTexture("./data/skybox/blizzard_lf.tga"),
		driver->getTexture("./data/skybox/blizzard_ft.tga"),
		driver->getTexture("./data/skybox/blizzard_bk.tga"));
	
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);


// loading d3s scene(Mesh)
	scene::ISceneNode* test_scene = smgr->addOctTreeSceneNode(smgr->getMesh("./data/3ds/final2.3ds"));
	test_scene->setPosition( irr::core::vector3df(0,0,0) );
	test_scene->setScale( irr::core::vector3df(1.25,1.25,1.25) );
	test_scene->setRotation( irr::core::vector3df(0,0,0) );
	test_scene->setMaterialFlag(video::EMF_LIGHTING, false);

	// Shader
	hdr = new ShaderGroup(device, smgr);
	hdr->add(test_scene);

	// Message 
	wchar_t *caption = L"Demo2. Depth Of Field - Two trees";
	wchar_t *text = 
		L"Depth Of Field\n\n"
		L"In this demo You change camera focus manually.\n"
		L"\nKeys:\n"
		L"   F1/F2 - alter camera Range\n"
		L"   F3/F4 - alter camera Focus\n"
		L"   F5/F6 - alter Blur0 and Blur1\n"
		L"   F7/F8 - alter camera Distance\n"
		L"   WASD/Cursor keys - Move camera\n"
		L"   Mouse - Orientate camera\n"
		L"   Escape - Exit demo\n";
	gui::IGUIWindow *messageBox = env->addMessageBox(caption, text, true, gui::EMBF_OK, 0, MESSAGEBOX_ID);
	cam->setInputReceiverEnabled(false);
core::vector2d<s32> pos;
int temp0=1;
	while(!g_bQuit && device->run()) {
		if (device->isWindowActive()) {

			if (g_bMessageBoxClosed && temp) {
				device->getCursorControl()->setPosition(pos);
				cam->setInputReceiverEnabled(true);
				device->getCursorControl()->setVisible(false);
				fader->remove();
		        temp=0;
			}

			hdr->render();
    if (temp0){pos=device->getCursorControl()->getPosition(); temp0=0;}
			int fps = driver->getFPS();
			core::stringw str = L"DEMO2. FPS:";
			str += fps;
			str += "   range=";
			str += hdr->range;
			str += "   focus=";
			str += hdr->focus;
			str += "   blur0=";
			str += hdr->sampleDist0;
			str += "   blur1=";
			str += hdr->sampleDist1;
			str += "   dist=";
			str += hdr->distanceScale;
			device->setWindowCaption(str.c_str());
		}
	}

	device->drop();

	return 0;

}


