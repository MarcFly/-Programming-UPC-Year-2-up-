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
	world->SetContactListener(this);
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

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{

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
	body_list.clear();

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

	body_list.add(PhysBody(body));
	body->SetUserData(body_list.getLast());
}

void ModulePhysics::RandomBox(iPoint pos) {

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x, PIXELS_TO_METERS pos.y);

	b2PolygonShape Rnd;
	//int sth = groundCircle.m_radius;
	b2Vec2 size = { 0.1f + (rand() % 100) / 200.0f, 0.1f + (rand() % 100) / 200.0f };

	Rnd.SetAsBox(size.x, size.y);

	b2FixtureDef fixt;
	fixt.shape = &Rnd;
	fixt.density = 1.0f; // P2 TODO 2.2 Density makes shapes behave normally

	b2Body* body = world->CreateBody(&bodydef);
	body->CreateFixture(&fixt);

	body_list.add(PhysBody(body, size.x, size.y));
	body->SetUserData(body_list.getLast());
}

void ModulePhysics::GivenChain(const int* points, iPoint pos, int size) {

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(PIXELS_TO_METERS pos.x, PIXELS_TO_METERS pos.y);
	
	b2Body* body = world->CreateBody(&bodydef);
		
	b2Vec2* p = new b2Vec2[size / 2];
	for (int i = 0; i < size / 2; i++) {
		p[i].x = PIXELS_TO_METERS(*points);
		p[i].y = PIXELS_TO_METERS(*++points);
		points++;
	}

	b2ChainShape chain;
	
	chain.CreateLoop(p, size/2);
	delete[] p;

	b2FixtureDef fixt;
	fixt.shape = &chain;
	
	body->CreateFixture(&fixt);
	
	body_list.add(PhysBody(body));
	body->SetUserData(body_list.getLast());
}

bool PhysBody::Contains(int x, int y) const
{
	// Handout 3 TODO 1: Write the code to return true in case the point
	// is inside ANY of the shapes contained by this body
	bool ret = false;

	b2Fixture* item = body->GetFixtureList();
	while (item->GetDensity() >= 0)
	{
		if (item->GetShape()->TestPoint(body->GetTransform(), { PIXELS_TO_METERS x, PIXELS_TO_METERS y }))
			ret = true;
		item++;
	}


	return ret;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	// Handout 3 TODO 2: Write code to test a ray cast between both points provided. If not hit return -1
	// if hit, fill normal_x and normal_y and return the distance between x1,y1 and it's colliding point
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1 = { PIXELS_TO_METERS x1, PIXELS_TO_METERS y1 };
	input.p2 = { PIXELS_TO_METERS x2, PIXELS_TO_METERS y2 };
	input.maxFraction = 1.6f;

	b2Fixture* item = body->GetFixtureList();
	while (item->GetDensity() >= 0)
	{
		if (item->GetShape()->TestPoint(body->GetTransform(), input.p2)) {
			item->GetShape()->RayCast(&output, input, body->GetTransform(), 0);
			ret = output.fraction * output.normal.LengthSquared();
		}
		item++;
	}

	return ret;
}

// Handout 3 TODO 3

// Handout 3 TODO 4:  Begin contact from user data, which is their respective PhysBody
// Handout 3 TODO 7: Call the listeners that are not NULL
void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::OnCollision(PhysBody* body1, PhysBody* body2)
{

}