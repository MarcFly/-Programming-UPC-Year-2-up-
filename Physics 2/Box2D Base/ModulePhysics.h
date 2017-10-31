#pragma once
#include "Module.h"
#include "Globals.h"

#include "Box2D\Box2D\Box2D.h"

//class b2World;
//struct b2BodyDef;

#define METERS_TO_PIXELS 100.0f*
#define PIXELS_TO_METERS 0.01f*

// Handout 2 TODO 6
// Module --------------------------------------
// TODO 3: Make module physics inherit from b2ContactListener
// then override void BeginContact(b2Contact* contact)
class PhysBody {
public:
	b2Body*	body;
	float width, height;

	// Handout 3 TODO 6: Add a pointer to a module that might want to listen to a collision from this body
	Module*	listener = nullptr;

	//Methods
	PhysBody() {}

	PhysBody(b2Body* body_) {
		body = body_;
	}

	PhysBody(b2Body* body_, float width_, float height_) {
		body = body_;
		width = width_;
		height = height_;
	}

	~PhysBody() {

	}

	b2Vec2 GetPos()
	{
		if (body->GetFixtureList()->GetShape()->GetType() == body->GetFixtureList()->GetShape()->e_circle)
			return {
				METERS_TO_PIXELS(body->GetPosition().x - body->GetFixtureList()->GetShape()->m_radius),
				METERS_TO_PIXELS(body->GetPosition().y - body->GetFixtureList()->GetShape()->m_radius)
		};

		else if (body->GetFixtureList()->GetShape()->GetType() == body->GetFixtureList()->GetShape()->e_polygon)
			return {
			METERS_TO_PIXELS(body->GetPosition().x - width),
			METERS_TO_PIXELS(body->GetPosition().y - height)
		};

		else
			return {
				METERS_TO_PIXELS(body->GetPosition().x),
				METERS_TO_PIXELS(body->GetPosition().y)
		};

	}

	float GetRotation()
	{
		return RADTODEG * body->GetAngle();
	}

	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

	
};

class ModulePhysics : public Module, private b2ContactListener
{
public:
	//b2BodyDef Circle_Base;
	//b2CircleShape groundCircle;

private:
	b2World* world;	//Game Physical World
	

public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnCollision(PhysBody* body1, PhysBody* body2);

	void BeginContact(b2Contact* contact);

	//P2 TODO 1.5 Function to create circle
	void RandomCircle(iPoint pos);

	//P2 TODO 2.1 Function to create box
	void RandomBox(iPoint pos);

	//P2 TODO 2.3 Function to create chains given points
	void GivenChain(const int* points, iPoint pos, int size);

	void CreateRectangle(int x, int y, int width, int height) {}
	void CreateRectangleSensor(int x, int y, int width, int height) {};

private:

	bool debug = true;

public:
	p2List<PhysBody> body_list;
};