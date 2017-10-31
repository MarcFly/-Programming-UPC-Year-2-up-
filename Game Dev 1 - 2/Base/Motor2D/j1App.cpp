#include <iostream>
#include <sstream>

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Pathfinding.h"
#include "j1App.h"

// TODO 9.3: Measure the amount of ms that takes to execute:
// App constructor, Awake, Start and CleanUp
// LOG the result

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;//?
	
	TickTimer.Start();
	PerfTimer.Start();
	float time = TickTimer.Read();

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	pathfinding = new j1Pathfinding();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(scene);

	// render last to swap buffer
	AddModule(render);

	time = TickTimer.Read() - time;
	LOG("%f",time);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	float time = TickTimer.Read();

	bool ret = true;

	ret = LoadConfig();

	p2List_item<j1Module*>* item;
	item = modules.start;


	while(item != NULL && ret == true)
	{
		// TODO 1.7: Add a new argument to the Awake method to receive a pointer to a xml node.
		// If the section with the module name exist in config.xml, fill the pointer with the address of a valid xml_node
		// that can be used to read all variables from that section. Send nullptr if the section does not exist in config.xml
	
		ret = item->data->Awake(root_node.child(item->data->name.GetString()));
		item = item->next;
	}

	return ret;

	time = TickTimer.Read() - time;
	LOG("%f", time);
}

// Called before the first frame
bool j1App::Start()
{
	float time = TickTimer.Read();

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	time = TickTimer.Read() - time;
	LOG("%f", time);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

//----------------------------------------------
// TODO 1.3: Load config.xml file using load_file() method from the xml_document class.
// If everything goes well, load the top tag inside the xml_node property
// created in the last TODO
bool j1App::LoadConfig()
{
	bool ret = true;

	pugi::xml_parse_result result = document.load_file("config.xml");
	//LOG("%b", result);

	if (result == NULL){ //Check that it loaded
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
		ret = false;
	}
	else {
		root_node = document.child("config");
		app_config = root_node.child("app");
	}

	/*// TODO 2.3 Load savegame document and nodes
	pugi::xml_document savegame_doc;

	result = savegame_doc.load_file("savegame.xml");

	pugi::xml_node root_savegame_node = savegame_doc.child("savegame");

	//LOG("%b", result);

	if (result == NULL) { //Check that it loaded
		LOG("Could not load map xml file savegame.xml. pugi error: %s", result.description());
		ret = false;
	}
	else {
		root_savegame_node = savegame_doc.child("savegame");
	}*/
	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	// TODO 2.1: This is a good place to call load / Save functions, Delay load/save at the end of a frame
	if (trigger_save_module == true)
		Save();

	if (trigger_load_module == true)
		Load();
	
	trigger_load_module = false;
	trigger_save_module = false;

	// TODO 9.4: Now calculate:
	// Amount of frames since startup
	// Amount of time since game start (use a low resolution timer)
	// Average FPS for the whole game life
	// Amount of ms took the last update
	// Amount of frames during the last second

	//frames = PerfTimer.ReadTicks();
	avg_fps = frames / seconds_since_startup;
	seconds_since_startup = TickTimer.ReadSec();
	dt = 0.0f;
	last_frame_ms = PerfTimer.ReadTicks();
	last_fr = 0;
	frames++;

	p2SString title;
	title.create("Av.FPS: %.2f Last Frame Ms: %u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
		avg_fps, last_frame_ms, last_fr, dt, seconds_since_startup, frames);
	
	App->win->SetTitle(title.GetString());
	
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	j1Module* pModule = NULL;

	for(item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	float time = TickTimer.Read();

	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	time = TickTimer.Read() - time;
	LOG("%f", time);

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

float j1App::GetDT() const
{
	return 0.0f;
}
// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}


// TODO 2.3: Create a simulation of the xml file to read 

// TODO 2.4: Create a method to actually load an xml file
// then call all the modules to load themselves

bool j1App::Load() {
	bool ret = true;
	LOG("EYYYY LOADING BOYA/n");

	pugi::xml_document savegame_doc;

	pugi::xml_parse_result result = savegame_doc.load_file("savegame.xml");

	pugi::xml_node root_savegame_node = savegame_doc.child("savegame");

	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for (item = modules.start; item != NULL && ret == true; item = item->next)
	{
		ret = item->data->Load(root_savegame_node.child(item->data->name.GetString()));
	}

	return ret;
}

// TODO 2.7: Create a method to save the current state
const bool j1App::Save() {
	bool ret = true;
	LOG("EYYYYY SAVING BOYA/n");


	pugi::xml_document	savegame_doc;	//Crees el document en local
	savegame_doc.append_child("savegame");	//Crees el fill que sera desde el que s'accedeix a altres fills
	pugi::xml_node	root_savegame_node = savegame_doc.child("savegame");	//Converteixes el fill creat en el root node

	//	Iterem per tota la llista de moduls
	p2List_item<j1Module*>* item;
	item = modules.start;
	j1Module* pModule = NULL;

	for (item = modules.start ; item != NULL && ret == true; item = item->next)
	{
		root_savegame_node.append_child(item->data->name.GetString());	//Creem el fill del modul al que estem, des del node arrel
		ret = item->data->Save(root_savegame_node.child(item->data->name.GetString()));	//Cridem la funcio
	}

	savegame_doc.save_file("savegame.xml");	//Guardem el arxiu sencer, com a nou xml que sobreescriura el xml anterior (a menys que fem un sistema de noms per saves)

	return ret;
}



