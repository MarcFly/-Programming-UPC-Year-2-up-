#pragma once
#include "Module.h"
#include "Globals.h"

#include "Box2D\Box2D\Box2D.h"

//class b2World;
//struct b2BodyDef;

class ModulePhysics : public Module
{
public:
	b2BodyDef Circle_Base;

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

	bool debug;
};