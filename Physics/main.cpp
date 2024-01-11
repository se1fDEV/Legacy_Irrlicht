#include <irrlicht.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <cstdlib>
#include <IGUIInOutFader.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

// Functions
static void CreateStartScene();
static void CreateBox(const btVector3 &TPosition, const vector3df &TScale, btScalar TMass);
static void CreateBox2(const btVector3 &TPosition, const vector3df &TScale, btScalar TMass);
static void CreateSphere(const btVector3 &TPosition, btScalar TRadius, btScalar TMass);
static void QuaternionToEuler(const btQuaternion &TQuat, btVector3 &TEuler);
static void UpdatePhysics(u32 TDeltaTime);
static void UpdateRender(btRigidBody *TObject);
static void ClearObjects();
static int GetRandInt(int TMax) { return rand() % TMax; }

// Globals
static bool Done = false;
static btDiscreteDynamicsWorld *World;
static IrrlichtDevice *irrDevice;
static IVideoDriver *irrDriver;
static ISceneManager *irrScene;
static IGUIEnvironment *irrGUI;
static IFileSystem *irrFile;
static ITimer *irrTimer;
static ILogger *irrLog;
static list<btRigidBody *> Objects;

bool g_bQuit = false;
bool temp=1;
// Flagged "true" when MessageBox is closed
bool g_bMessageBoxClosed = false;
// ID of message box
#define MESSAGEBOX_ID 1

// Event receiver
class EventReceiverClass : public IEventReceiver  {

public:

	virtual bool OnEvent(const SEvent &TEvent) {
//GUI events
		if (TEvent.EventType == EET_GUI_EVENT)
		{
			s32 id = TEvent.GUIEvent.Caller->getID();
			// Test for messagebox
			if (id == MESSAGEBOX_ID) {
				if ((TEvent.GUIEvent.EventType == gui::EGET_MESSAGEBOX_OK) || 
					(TEvent.GUIEvent.EventType == gui::EGET_MESSAGEBOX_CANCEL))
				{
					// MesageBox OK button was clicked on "OK" or closed(x)
					g_bMessageBoxClosed = true;
				}
			}
		}
		if(TEvent.EventType == EET_KEY_INPUT_EVENT && !TEvent.KeyInput.PressedDown) {
			switch(TEvent.KeyInput.Key) {
				case KEY_ESCAPE:
					g_bQuit = true;
				break;
				case KEY_F1:
					CreateBox(btVector3(GetRandInt(10) - 5.0f, 7.0f, GetRandInt(10) - 5.0f), vector3df(GetRandInt(3) + 0.5f, GetRandInt(3) + 0.5f, GetRandInt(3) + 0.5f), 1.0f);
				break;
				case KEY_F2:
					CreateSphere(btVector3(GetRandInt(10) - 5.0f, 7.0f, GetRandInt(10) - 5.0f), GetRandInt(5) / 5.0f + 0.2f, 1.0f);
				break;
				case KEY_F3:
					CreateStartScene();
				break;
				default:
					return false;
				break;
			}

			return true;
		}

		return false;
	}
};

int main() {

	// Initialize irrlicht
	EventReceiverClass Receiver;
	irrDevice = createDevice(video::EDT_DIRECT3D9, dimension2d<u32>(1024, 768), 32, false, true, true, &Receiver);
	irrGUI = irrDevice->getGUIEnvironment();
	irrTimer = irrDevice->getTimer();
	irrScene = irrDevice->getSceneManager();
	irrDriver = irrDevice->getVideoDriver();
    
    gui::IGUIInOutFader* fader = irrDevice->getGUIEnvironment()->addInOutFader();
        fader->setColor(video::SColor(0,0,0,0));
        fader->fadeIn(8000);






 IGUISkin* skin = irrGUI->getSkin();
	gui::IGUIFont* font = irrGUI->getFont("font.png");
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

	scene::ICameraSceneNode* cam = irrScene->addCameraSceneNodeFPS(0, 100.0f, 0.01f, -1, keyMap, 8);
	//cam->setPosition(core::vector3df(-280.0f,140.0f,540.0f));
	//cam->setTarget(core::vector3df(0,100,400));
	cam->setFarValue(8000.0f);

	irrDevice->getCursorControl()->setVisible(true);

	// Initialize bullet
	btDefaultCollisionConfiguration *CollisionConfiguration = new btDefaultCollisionConfiguration();
	btBroadphaseInterface *BroadPhase = new btAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000));
	btCollisionDispatcher *Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	btSequentialImpulseConstraintSolver *Solver = new btSequentialImpulseConstraintSolver();
	World = new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, Solver, CollisionConfiguration);
	
	// Add camera
	//ICameraSceneNode *Camera = irrScene->addCameraSceneNodeFPS(0, 100, 10);
	cam->setPosition(vector3df(9, 8, -12));
	cam->setTarget(vector3df(0, 0, 0));

	// Preload textures
	irrDriver->getTexture("spr.jpg");
	irrDriver->getTexture("box0.jpg");
	irrDriver->getTexture("box1.jpg");

	// Create text
	//IGUISkin *Skin = irrGUI->getSkin();
	//Skin->setColor(EGDC_BUTTON_TEXT, SColor(255, 255, 255, 255));
	//irrGUI->addStaticText(L"Hit 1 to create a box\nHit 2 to create a sphere\nHit x to reset", rect<s32>(0, 0, 200, 100), false);

	// Create the initial scene

//irrScene->addLightSceneNode(0, core::vector3df(2, 5, -2), SColorf(4, 4, 4, 1));
	
CreateStartScene();

    irrGUI->addStaticText(
		L"Press 'F1' to create box\nPress 'F2' to create sphere\nPress 'F3' to clear",
		core::rect<s32>(10,10,315,85), true, true, 0, -1, true);

int lastFPS = -1;

	// Main loop
	wchar_t *caption = L"Demo5. Simple Physics";
	wchar_t *text = 
		L"Simple Physics\n\n"
		L"In this demo you can create boxes and spheres.\n"
		L"\nKeys:\n"
		L"   F1 - create box\n"
		L"   F2 - create sphere\n"
		L"   F3 - clear\n"
		L"   Escape - Exit demo\n";
	IGUIWindow *messageBox = irrGUI->addMessageBox(caption, text, true, gui::EMBF_OK, 0, MESSAGEBOX_ID);
	cam->setInputReceiverEnabled(false);

int temp0=1;
core::vector2d<s32> pos;
	u32 TimeStamp = irrTimer->getTime(), DeltaTime = 0;
	while(!g_bQuit && irrDevice->run()) {
		if (irrDevice->isWindowActive()) {
		
			if (g_bMessageBoxClosed && temp) {
				irrDevice->getCursorControl()->setPosition(pos);
				cam->setInputReceiverEnabled(true);
				irrDevice->getCursorControl()->setVisible(false);
				fader->remove();
		        temp=0;
			}

		DeltaTime = irrTimer->getTime() - TimeStamp;
		TimeStamp = irrTimer->getTime();

		UpdatePhysics(DeltaTime);
        if (temp0){pos=irrDevice->getCursorControl()->getPosition(); temp0=0;}
		irrDriver->beginScene(true, true, SColor(0xff88aadd));
		irrScene->drawAll();
		irrGUI->drawAll();
		irrDriver->endScene();
		//irrDevice->run();
		 if (temp0){pos=irrDevice->getCursorControl()->getPosition(); temp0=0;}
		int fps = irrDriver->getFPS();
        
		if (lastFPS != fps)
		{
			core::stringw str = L"Demo5. Simple Physics [";
			str += irrDriver->getName();
			str += "] FPS:";
			str += fps;

			irrDevice->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}
	}

	ClearObjects();
	delete World;
	delete Solver;
	delete Dispatcher;
	delete BroadPhase;
	delete CollisionConfiguration;

	irrDevice->drop();

	return 0;
}

// Runs the physics simulation.
// - TDeltaTime tells the simulation how much time has passed since the last frame so the simulation can run independently of the frame rate.
void UpdatePhysics(u32 TDeltaTime) {

	World->stepSimulation(TDeltaTime * 0.001f, 60);

	// Relay the object's orientation to irrlicht
	for(list<btRigidBody *>::Iterator Iterator = Objects.begin(); Iterator != Objects.end(); ++Iterator) {

		UpdateRender(*Iterator);
	}	
}

// Creates a base box
void CreateStartScene() {

	ClearObjects();
    CreateBox2(btVector3(0.0f, 0.0f, 0.0f), vector3df(4.0f, 0.5f, 15.0f), 0.0f);
	CreateBox2(btVector3(0.0f, -2.0f, 0.0f), vector3df(9.0f, 0.5f, 9.0f), 0.0f);
	CreateBox2(btVector3(0.0f, -4.0f, 0.0f), vector3df(40.0f, 0.5f, 40.0f), 0.0f);
}

// Create a box rigid body
void CreateBox(const btVector3 &TPosition, const vector3df &TScale, btScalar TMass) {
	
	ISceneNode *Node = irrScene->addCubeSceneNode(1.0f);
	Node->setScale(TScale);
Node->setMaterialFlag(EMF_LIGHTING, 0);
	Node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, irrDriver->getTexture("box0.jpg"));

	// Set the initial position of the object
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(TPosition);

	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	// Create the shape
	btVector3 HalfExtents(TScale.X * 0.5f, TScale.Y * 0.5f, TScale.Z * 0.5f);
	btCollisionShape *Shape = new btBoxShape(HalfExtents);

	// Add mass
	btVector3 LocalInertia;
	Shape->calculateLocalInertia(TMass, LocalInertia);

	// Create the rigid body object
	btRigidBody *RigidBody = new btRigidBody(TMass, MotionState, Shape, LocalInertia);

	// Store a pointer to the irrlicht node so we can update it later
	RigidBody->setUserPointer((void *)(Node));

	// Add it to the world
	World->addRigidBody(RigidBody);
	Objects.push_back(RigidBody);
}

void CreateBox2(const btVector3 &TPosition, const vector3df &TScale, btScalar TMass) {
	
	ISceneNode *Node = irrScene->addCubeSceneNode(1.0f);
	Node->setScale(TScale);
Node->setMaterialFlag(EMF_LIGHTING, 0);
	Node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, irrDriver->getTexture("box1.jpg"));

	// Set the initial position of the object
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(TPosition);

	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	// Create the shape
	btVector3 HalfExtents(TScale.X * 0.5f, TScale.Y * 0.5f, TScale.Z * 0.5f);
	btCollisionShape *Shape = new btBoxShape(HalfExtents);

	// Add mass
	btVector3 LocalInertia;
	Shape->calculateLocalInertia(TMass, LocalInertia);

	// Create the rigid body object
	btRigidBody *RigidBody = new btRigidBody(TMass, MotionState, Shape, LocalInertia);

	// Store a pointer to the irrlicht node so we can update it later
	RigidBody->setUserPointer((void *)(Node));

	// Add it to the world
	World->addRigidBody(RigidBody);
	Objects.push_back(RigidBody);
}


// Create a sphere rigid body
void CreateSphere(const btVector3 &TPosition, btScalar TRadius, btScalar TMass) {
	
	ISceneNode *Node = irrScene->addSphereSceneNode(TRadius, 32);
Node->setMaterialFlag(EMF_LIGHTING, 0);
	Node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	Node->setMaterialTexture(0, irrDriver->getTexture("spr.jpg"));

	// Set the initial position of the object
	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(TPosition);

	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	// Create the shape
	btCollisionShape *Shape = new btSphereShape(TRadius);

	// Add mass
	btVector3 LocalInertia;
	Shape->calculateLocalInertia(TMass, LocalInertia);

	// Create the rigid body object
	btRigidBody *RigidBody = new btRigidBody(TMass, MotionState, Shape, LocalInertia);

	// Store a pointer to the irrlicht node so we can update it later
	RigidBody->setUserPointer((void *)(Node));

	// Add it to the world
	World->addRigidBody(RigidBody);
	Objects.push_back(RigidBody);
}

// Converts a quaternion to an euler angle
void QuaternionToEuler(const btQuaternion &TQuat, btVector3 &TEuler) {
	btScalar W = TQuat.getW();
	btScalar X = TQuat.getX();
	btScalar Y = TQuat.getY();
	btScalar Z = TQuat.getZ();
	float WSquared = W * W;
	float XSquared = X * X;
	float YSquared = Y * Y;
	float ZSquared = Z * Z;

	TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
	TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
	TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
	TEuler *= RADTODEG;
}

// Passes bullet's orientation to irrlicht
void UpdateRender(btRigidBody *TObject) {
	ISceneNode *Node = static_cast<ISceneNode *>(TObject->getUserPointer());

	// Set position
	btVector3 Point = TObject->getCenterOfMassPosition();
	Node->setPosition(vector3df((f32)Point[0], (f32)Point[1], (f32)Point[2]));

	// Set rotation
	btVector3 EulerRotation;
	QuaternionToEuler(TObject->getOrientation(), EulerRotation);
	Node->setRotation(vector3df(EulerRotation[0], EulerRotation[1], EulerRotation[2]));
}

// Removes all objects from the world
void ClearObjects() {

	for(list<btRigidBody *>::Iterator Iterator = Objects.begin(); Iterator != Objects.end(); ++Iterator) {
		btRigidBody *Object = *Iterator;

		// Delete irrlicht node
		ISceneNode *Node = static_cast<ISceneNode *>(Object->getUserPointer());
		Node->remove();

		// Remove the object from the world
		World->removeRigidBody(Object);

		// Free memory
		delete Object->getMotionState();
		delete Object->getCollisionShape();
		delete Object;
	}

	Objects.clear();
}