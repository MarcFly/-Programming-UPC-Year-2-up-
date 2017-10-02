#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include <time.h>



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
	srand(time(NULL));

	LOG("Creating Physics 2D environment");

	// TODO 2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	b2Vec2 temp_grav = { 0.0f, 9.8f };

	world = new b2World(temp_grav);

	// TODO 4: Create a a big static circle as "ground"
	b2BodyDef Circle_Base;
	Circle_Base.position.Set(5.0f, 4.5f);
	Circle_Base.type = b2_staticBody;
	
	b2CircleShape groundCircle;
	groundCircle.m_radius = 1.5f;
	
	b2FixtureDef CircleFixture;
	CircleFixture.shape = &groundCircle;
	//CircleFixture.density = 1;

	b2Body* groundBody = world->CreateBody(&Circle_Base);
	groundBody->CreateFixture(&CircleFixture);
	//groundBody->SetType(b2_staticBody);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	world->Step(1.0f/60.0f, 8, 3);	//Time Step (using 60fps), velocity of iterations (8 better, 6 bit more optimal), position iterations (3 better, 2 more optimal)
									//Basically the iterations (vel and pos) are for accuracy purposes, more iterations more accuracy less performance
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// TODO 5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius
	//iPoint pos = { groundCircle.m_p.x, groundCircle.m_p.y - 1000 };
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		RandomCircle({App->input->GetMouseX(), App->input->GetMouseY()});
		
	/*if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;*/

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	
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
	}

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

void ModulePhysics::RandomCircle(iPoint pos) {

	

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x,PIXELS_TO_METERS pos.y);

	

	b2CircleShape Rnd;
	//int sth = groundCircle.m_radius;
	Rnd.m_radius = 0.1 + rand()%1;

	b2FixtureDef fixt;
	fixt.shape = &Rnd;
	//fixt.density = 500;

	b2Body* body = world->CreateBody(&bodydef);
	body->CreateFixture(&fixt);

}