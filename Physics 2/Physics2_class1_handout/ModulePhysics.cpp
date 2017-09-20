#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

// TODO 1: Include Box 2 header and library
//#include "Box2D\Box2D\Box2D.h"
#ifdef _DEBUG
#pragma comment (lib,"Box2D/libx86/Debug/Box2D.lib")
#endif //_DEBUG

#ifdef _RELEASE
#pragma comment (lib,"Box2D/libx86/Release/Box2D.lib")
#endif // _RELEASE


ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	//TODO 2.2 Init the world

	world = new b2World({ 0,0 });
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	// TODO 2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	b2Vec2 temp_grav = { 0, -9.8f };

	world->SetGravity(temp_grav);

	// TODO 4: Create a a big static circle as "ground"
	Circle_Base.position.Set(200, 300);
	b2Body* groundBody = world->CreateBody(&Circle_Base);
	b2CircleShape groundCircle;
	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	world->Step(1.0f/60.0f, 6, 2);	//Time Step (using 60fps), velocity of iterations (8 better, 6 bit more optimal), position iterations (3 better, 2 more optimal)
									//Basically the iterations (vel and pos) are for accuracy purposes, more iterations more accuracy less performance
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// TODO 5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	/*
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// You will have to add more cases to draw boxes, edges, and polygons ...
			}
		}
	}*/

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// TODO 2.3 Delete the whole physics world!
	delete world;

	return true;
}
