#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// TODO 2: Place the camera one unit up in Y and one unit to the right
	// experiment with different camera placements, then use LookAt()
	// to make it look at the center
	App->camera->Move({ 0, 1, 0 });

	// TODO 1: Create a Plane primitive. This uses the plane formula
	// so you have to express the normal of the plane to create 
	// a plane centered around 0,0. Make that it draw the axis for reference
	primitive_list.add(CreatePlane({ 0, 1, 0 }));
	primitive_list.getLast()->data->axis = true;

	// TODO 6: Draw a sphere of 0.5f radius around the center
	// Draw somewhere else a cube and a cylinder in wireframe
	primitive_list.add(CreateSphere({ 0,0,0 }, 0.5f));
	primitive_list.getLast()->data->wire = true;
	primitive_list.add(CreateCube({ 1,1,1 }, { 1,1,1 }));
	primitive_list.getLast()->data->wire = true;
	primitive_list.add(CreateCylinder({ -1,-1,-1 }, 1, 5));
	primitive_list.getLast()->data->wire = true;


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	// Make sure they are renderable and are being rendered
	p2List_item<Primitive*>* item = primitive_list.getFirst();
	while (item != NULL) {
		item->data->Render();
		item = item->next;
	}



	return UPDATE_CONTINUE;
}

