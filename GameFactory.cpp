// Game Factory for the OEParticleSim project.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// Class header
#include "GameFactory.h"

#include <Devices/SDLInput.h>
#include <Display/Viewport.h>
#include <Display/ViewingVolume.h>
#include <Display/Frustum.h>
#include <Display/SDLFrame.h>
#include <Logging/Logger.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Resources/ResourceManager.h>
#include <Resources/File.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Meta/OpenGL.h>

#include <string>
#include <fstream>

// from GenericHandler extension
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>

// Project files
#include "Dummy.h"
#include "Dwarf.h"
#include "Location.h"
#include "GoldStorage.h"
#include <Logic/LogicModule.h>
#include <Logic/BehaviorTree.h>
#include <Logic/Selector.h>
#include <Logic/Sequence.h>
#include <Logic/Task.h>
#include "Logic/GoToTasks.h"
#include <Logic/ProbabilitySelector.h>
#include <Logic/RandomSelector.h>
#include <Logic/DebugDecorator.h>
#include <Logic/LoopOnSuccessDecorator.h>
#include <Logic/WaitAction.h>
#include <Logic/Parallel.h>
#include "Logic/MineTasks.h"
#include <Logic/TestSuite.h>

// Additional namespaces (others are in the header).
using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Logic;

/**
 * Factory constructor.
 *
 * Initializes the different components so they are accessible when
 * the setup method is executed.
 */
GameFactory::GameFactory() {

    // Create a frame and viewport.
    this->frame = new SDLFrame(800, 600, 32);
    //this->frame    = new SDLFrame(1280, 800, 32, FRAME_FULLSCREEN);

    // Main viewport
    viewport = new Viewport(*frame);

    // Create a renderer.
    this->renderer = new Renderer();

    // Setup the camera
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(0,20,100));
    //viewport->SetViewingVolume(camera);

    // frustum hack
    Frustum* frustum = new Frustum(*camera);
    frustum->SetFar(1000);
    viewport->SetViewingVolume(frustum);

    // Add a rendering view to the renderer
    this->renderer->process.Attach(*(new RenderingView(*viewport)));
}

/**
 * Setup handler.
 *
 * This is the main setup method of the game factory. Here you can add
 * any non-standard modules and perform other setup tasks prior to the
 * engine start up.
 *
 * @param engine The game engine instance.
 */
bool GameFactory::SetupEngine(IGameEngine& engine) {
    // First we set the resources directory
    string resourcedir = "./projects/BehaviorSim/data/";
    DirectoryManager::AppendPath(resourcedir);
    logger.info << "Resource directory: " << resourcedir << logger.end;

    // load the resource plug-ins
    ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());
    ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());

    // Setup input handling
    SDLInput* input = new SDLInput();
    engine.AddModule(*input);

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler();
    quit_h->BindToEventSystem();

    // Register the handler as a listener on up and down keyboard events.
    MoveHandler* move_h = new MoveHandler(*camera);
    engine.AddModule(*move_h);
    move_h->BindToEventSystem();
    
    camera->SetPosition(Vector<3,float>(30.0, 350.0, 350.0));
    camera->LookAt(     Vector<3,float>(30.0,   0.0, 80.0));

    // Create scene root
    ISceneNode* root = new SceneNode();
    this->renderer->SetSceneRoot(root);
    
    TestSuite* testSuite = new TestSuite();
    testSuite->TestAll();

	IModelResource* skyboxRes = &*ResourceManager<IModelResource>::Create("Skybox2/Skybox.obj");
	skyboxRes->Load();
	TransformationNode* skyboxTrans = new TransformationNode();
	skyboxTrans->AddNode(skyboxRes->GetSceneNode());
	root->AddNode(skyboxTrans);
	
	IModelResource* groundRes = &*ResourceManager<IModelResource>::Create("Ground/Ground.obj");
	groundRes->Load();
	root->AddNode(groundRes->GetSceneNode());

    Location* homeLocation    = new Location(Vector<3,float>(   0.0, 1.0,  150.0), Vector<3,float>(0.0, 0.8, 0.2), 80.0);
    Location* mine1Location   = new Location(Vector<3,float>(-160.0, 1.0, -150.0), Vector<3,float>(0.7, 0.6, 0.1));
    Location* mine2Location   = new Location(Vector<3,float>( 170.0, 1.0,  190.0), Vector<3,float>(0.7, 0.6, 0.1));
    Location* lakeLocation    = new Location(Vector<3,float>( 160.0, 1.0, -160.0), Vector<3,float>(0.0, 0.2, 0.8));
    Location* depositLocation = new Location(Vector<3,float>( -70.0, 1.0,  220.0), Vector<3,float>(0.9, 0.2, 0.2));
    
    GoldStorage* mine1   = new GoldStorage(mine1Location, 100, 100);
    GoldStorage* mine2   = new GoldStorage(mine2Location, 70, 70, 2, 2);
    GoldStorage* deposit = new GoldStorage(depositLocation, 0, 150);
    
    root->AddNode(homeLocation->GetRenderNode());
    root->AddNode(mine1->GetRenderNode());
    root->AddNode(mine2->GetRenderNode());
    root->AddNode(lakeLocation->GetRenderNode());
    root->AddNode(deposit->GetRenderNode());

	// add stuff to scene graph
	int numDwarfs = 15;
	float dwarfRadius = 30.0;
	Dwarf* dwarf = NULL;
	TransformationNode* transNode = NULL;
	for (int i = 0; i < numDwarfs; i++) {
		dwarf = new Dwarf();
		transNode = dwarf->GetTransformationNode();
		
		Vector<3,float> position(
			cos(((float)i/numDwarfs)*PI*2)*dwarfRadius, 
			0.0, 
			sin(((float)i/numDwarfs)*PI*2)*dwarfRadius
			);
		
		
		transNode->SetPosition(position);
		root->AddNode(transNode);
		
	    ProbabilitySelector* dwarfBehavior = new ProbabilitySelector();
	    LogicModule* logic = new LogicModule(new BehaviorTree(dwarfBehavior));
	    engine.AddModule(*logic);
	    
   	    Sequence* mineGold = new Sequence();
	    Sequence* quenchThirst = new Sequence();
	    Sequence* takeNap = new Sequence();
	    dwarfBehavior->AddTask(mineGold, 5);
   	    dwarfBehavior->AddTask(quenchThirst, 2);
   	    dwarfBehavior->AddTask(takeNap, 2);
	    
	    float speed = 0.07;
	    float digGoldDelay = 1.0;
	    float unloadGoldDelay = 0.5;

		/***************************************
		  Mine gold behavior
		 ***************************************/
		// Mine selection
	    RandomSelector* gotoMine = new RandomSelector();
	    Sequence* selectMine1 = new Sequence();
	    selectMine1->AppendTask(new IsGoldStorageNotDepletedCond(mine1));
	    selectMine1->AppendTask(new GoToGoldStorageAction(dwarf, mine1, speed));

   	    Sequence* selectMine2 = new Sequence();
	    selectMine2->AppendTask(new IsGoldStorageNotDepletedCond(mine2));
   	    selectMine2->AppendTask(new GoToGoldStorageAction(dwarf, mine2, speed));

   	    gotoMine->AppendTask(selectMine1);
   	    gotoMine->AppendTask(selectMine2);
   	    
   	    // Dig gold until dwarf cannot carry any more or mine is depleted
   	    Sequence* digGold = new Sequence();
   	    digGold->AppendTask(new WaitAction(digGoldDelay));
   	    digGold->AppendTask(new MineAction(dwarf));
   	    
   	    // Bring the gold back home
   	    Sequence* gotoDeposit = new Sequence();
   	    gotoDeposit->AppendTask(new GoToGoldStorageAction(dwarf, deposit, speed));
   	    
   	    // Unload the gold
   	    Sequence* unloadGold = new Sequence();
   	    unloadGold->AppendTask(new WaitAction(unloadGoldDelay));
   	    unloadGold->AppendTask(new UnloadGoldAction(dwarf, deposit));
   	    
   	    // Add the composite behaviors to the main "mine gold" behavior
   	    mineGold->AppendTask(new IsGoldStorageNotFullCond(deposit)); // TODO: Remove the conditions from the sequences and make them into parallels
   	    mineGold->AppendTask(gotoMine);
   	    mineGold->AppendTask(new LoopOnSuccessDecorator(digGold));
   	    mineGold->AppendTask(new IsGoldStorageNotFullCond(deposit)); // TODO: Remove the conditions from the sequences and make them into parallels
   	    mineGold->AppendTask(gotoDeposit);
   	    mineGold->AppendTask(new LoopOnSuccessDecorator(unloadGold));
	    
		/***************************************
		  Quench thirst behavior
		 ***************************************/
  	    quenchThirst->AppendTask(new GoToLocationAction(transNode, lakeLocation, speed));
  	    quenchThirst->AppendTask(new DrinkAction(dwarf));

		/***************************************
		  Take nap behavior
		 ***************************************/  	    
  	    takeNap->AppendTask(new GoToLocationAction(transNode, homeLocation, speed));
  	    takeNap->AppendTask(new SleepAction(dwarf));
	}

    // Return true to signal success.
    return true;
}

// Other factory methods. The returned objects are all created in the
// factory constructor.
IFrame*      GameFactory::GetFrame()    { return this->frame;    }
IRenderer*   GameFactory::GetRenderer() { return this->renderer; }
