#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"



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

	circle = App->textures->Load("pinball/wheel.png");
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");

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

	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	// P2 TODO 1.5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius
	//iPoint pos = { groundCircle.m_p.x, groundCircle.m_p.y - 1000 };
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		App->physics->RandomCircle(mouse);
	}

	// P2 TODO 2.1 When press 2, create box on mouse position
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		App->physics->RandomBox(mouse);
	}

	// P2 TODO 2.3 Create a chain given vertices, convert pixels to meters
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		int points[10] = {
			33, 280,
			118, 71,
			350, 23,
			435, 195,
			33, 281
		};
		App->physics->GivenChain(&points[0], mouse , sizeof(points) / sizeof(4));
	}


	// Ray Cast?
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	Draw(mouse, normal, ray_hit);

	DrawRay(mouse, normal, ray_hit);
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw(iPoint mouse, fVector& normal, int& ray_hit)
{
	p2List_item<PhysBody>* item = App->physics->body_list.getFirst();

	while (item != nullptr) {
		if (item->data.Contains(App->input->GetMouseX(), App->input->GetMouseY())) {
			if (item->data.body->GetFixtureList()->GetShape()->GetType() == item->data.body->GetFixtureList()->GetShape()->e_circle)
			{
				App->renderer->Blit(circle, item->data.GetPos().x, item->data.GetPos().y);
			}
			else if (item->data.body->GetFixtureList()->GetShape()->GetType() == item->data.body->GetFixtureList()->GetShape()->e_chain)
			{
				App->renderer->Blit(rick, item->data.GetPos().x, item->data.GetPos().y);
			}
			else if (item->data.body->GetFixtureList()->GetShape()->GetType() == item->data.body->GetFixtureList()->GetShape()->e_polygon)
			{
				if (ray_on)
				{
					int hit = item->data.RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
					if (hit >= 0)
						ray_hit = hit;
				}

				App->renderer->Blit(box, item->data.GetPos().x, item->data.GetPos().y, 0, 1.0f, item->data.GetRotation());
			}
		}

		item = item->next;
	}

}

void ModuleSceneIntro::DrawRay(iPoint mouse, fVector normal, int ray_hit)
{
	if (ray_on == true)
	{
		fVector destination(mouse.x - ray.x, mouse.y - ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if (normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}
}

void ModuleSceneIntro::OnCollision(PhysBody* body1, PhysBody* body2)
{

	App->audio->PlayFx(bonus_fx);

}
