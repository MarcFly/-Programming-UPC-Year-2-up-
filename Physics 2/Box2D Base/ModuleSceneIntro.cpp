#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

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
	
	// P2 TODO 1.5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius
	//iPoint pos = { groundCircle.m_p.x, groundCircle.m_p.y - 1000 };
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		App->physics->RandomCircle({ App->input->GetMouseX(), App->input->GetMouseY() });

	// P2 TODO 2.1 When press 2, create box on mouse position
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		App->physics->RandomBox({ App->input->GetMouseX(), App->input->GetMouseY() });

	// P2 TODO 2.3 Create a chain given vertices, convert pixels to meters
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		int points[10] = {
			33, 280,
			118, 71,
			350, 23,
			435, 195,
			33, 280
		};
		App->physics->GivenChain(&points[0], { App->input->GetMouseX(), App->input->GetMouseY() }, sizeof(points) / sizeof(int));
	}

	return UPDATE_CONTINUE;
}
