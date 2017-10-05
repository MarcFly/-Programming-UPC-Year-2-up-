#pragma once
#include "Module.h"
#include "Globals.h"

#include "Box2D\Box2D\Box2D.h"

//class b2World;
//struct b2BodyDef;

#define METERS_TO_PIXELS 100*
#define PIXELS_TO_METERS 0.01*

class ModulePhysics : public Module
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

	//P2 TODO 1.5 Function to create circle
	void RandomCircle(iPoint pos);

	//P2 TODO 2.1 Function to create box
	void RandomBox(iPoint pos);

	//P2 TODO 2.3 Function to create chains given points
	void GivenChain(const int* points, iPoint pos, int size);

private:

	bool debug = true;
};