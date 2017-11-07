#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#define BOUNCER_TIME 200

struct PhysBody3D;
class Cube;
class Primitive;
class Plane;
class Sphere;
class Cylinder;
class Line;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

	Plane* CreatePlane(const vec3& pos) {
		Plane* ret = new Plane(pos.x, pos.y, pos.z,0);
		return ret;
	}

	Cube* CreateCube(const vec3& pos, const vec3& size, const float& angle = 0,  const vec3& rotation = { 1,1,1 }) {
		Cube* ret = new Cube(size.x, size.y, size.z);
		ret->SetPos(pos.x, pos.y, pos.z);
		ret->SetRotation(angle, rotation);
		return ret;
	}

	Sphere* CreateSphere(const vec3& pos, const float& radius, const float& angle = 0,  const vec3& rotation = { 1,1,1}) {
		Sphere* ret = new Sphere(radius);
		ret->SetPos(pos.x, pos.y, pos.z);
		ret->SetRotation(angle, rotation);
		return ret;
	}

	Cylinder* CreateCylinder(const vec3& pos, const float& radius, const float& height, const float& angle = 0,  const vec3& rotation = { 1,1,1 }) {
		Cylinder* ret = new Cylinder(radius, height);
		ret->SetPos(pos.x, pos.y, pos.z);
		ret->SetRotation(angle, rotation);
		return ret;
	}

	Line* CreateLine(const vec3& origin, const vec3& destination) {
		Line* ret = new Line();
		ret->origin = origin;
		ret->destination = destination;
		return ret;
	}

public:

	p2List<Primitive*>	primitive_list;



};
