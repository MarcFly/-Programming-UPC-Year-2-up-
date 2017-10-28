#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include <time.h>



// P2 TODO 1.1: Include Box 2 header and library
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

	//P2 TODO 1.2.2 Init the world

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

	// P2 TODO 1.2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	b2Vec2 temp_grav = { 0.0f, 9.8f };

	world = new b2World(temp_grav);

	// P2 TODO 1.4: Create a a big static circle as "ground"
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
	// P2 TODO 1.3: Update the simulation ("step" the world)
	world->Step(1.0f/60.0f, 8, 3);	//Time Step (using 60fps), velocity of iterations (8 better, 6 bit more optimal), position iterations (3 better, 2 more optimal)
									//Basically the iterations (vel and pos) are for accuracy purposes, more iterations more accuracy less performance
	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// P2 TODO 1.5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius
	//iPoint pos = { groundCircle.m_p.x, groundCircle.m_p.y - 1000 };
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		RandomCircle({App->input->GetMouseX(), App->input->GetMouseY()});
		
	// P2 TODO 2.1 When press 2, create box on mouse position
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		RandomBox({ App->input->GetMouseX(), App->input->GetMouseY() });

	// P2 TODO 2.3 Create a chain given vertices, convert pixels to meters
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		int points[24] = { 
			-38, 80,
			-44, -54,
			-16, -60,
			-16, -17,
			19, -19,
			19, -79,
			61, -77,
			57, 73,
			17, 78,
			20, 16,
			-25, 13,
			-9, 72 };
		GivenChain(&points[0], { App->input->GetMouseX(), App->input->GetMouseY() }, 24);
	}

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

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
				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for (int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if (i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for (int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if (i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// P2 TODO 1.2.3 Delete the whole physics world!
	delete world;

	return true;
}

void ModulePhysics::RandomCircle(iPoint pos) {

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x, PIXELS_TO_METERS pos.y);

	b2CircleShape Rnd;
	//int sth = groundCircle.m_radius;
	Rnd.m_radius = 0.1f + (rand()%100)/200.0f;

	b2FixtureDef fixt;
	fixt.shape = &Rnd;
	fixt.density = 1.0f;

	b2Body* body = world->CreateBody(&bodydef);
	body->CreateFixture(&fixt);

}

void ModulePhysics::RandomBox(iPoint pos) {

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x, PIXELS_TO_METERS pos.y);

	b2PolygonShape Rnd;
	//int sth = groundCircle.m_radius;
	Rnd.SetAsBox(0.1f + (rand() % 100) / 200.0f, 0.1f + (rand() % 100) / 200.0f);

	b2FixtureDef fixt;
	fixt.shape = &Rnd;
	fixt.density = 1.0f; // P2 TODO 2.2 Density makes shapes behave normally

	b2Body* body = world->CreateBody(&bodydef);
	body->CreateFixture(&fixt);
}

void ModulePhysics::GivenChain(const int* points, iPoint pos, int size) {

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x, PIXELS_TO_METERS pos.y);

	b2ChainShape chain;
	//int sth = groundCircle.m_radius;
	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; i++) {
		p[i].x = PIXELS_TO_METERS(points[i * 2]);
		p[i].y = PIXELS_TO_METERS(points[i * 2 + 1]);
	}

	chain.CreateLoop(p, size / 2);

	b2FixtureDef fixt;
	fixt.shape = &chain;
	//fixt.density = 1.0f; // P2 TODO 2.2 Density makes shapes behave normally

	b2Body* body = world->CreateBody(&bodydef);
	body->CreateFixture(&fixt);

	delete p;
}