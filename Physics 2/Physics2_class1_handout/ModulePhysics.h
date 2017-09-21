#pragma once
#include "Module.h"
#include "Globals.h"

#include "Box2D\Box2D\Box2D.h"

//class b2World;
//struct b2BodyDef;

#define METERS_TO_PIXELS 0.1*
#define PIXELS_TO_METERS 10*

class ModulePhysics : public Module
{
public:
	b2BodyDef Circle_Base;
	b2Body* groundBody;
	b2CircleShape groundCircle;

private:
	b2World* world;	//Game Physical World
	

public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

private:

	bool debug = true;
};