#include "j1Player.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"

j1Player::j1Player()
{
	name.create("player");
}

j1Player::~j1Player()
{

}

bool j1Player::Awake(const pugi::xml_node& config)
{
	bool ret = true;

	tick_cap = config.child("ticks").attribute("value").as_uint();

	return ret;
}

bool j1Player::Start()
{
	bool ret = true;

	return ret;
}

bool j1Player::CleanUp()
{
	bool ret = true;

	return ret;
}
bool j1Player::UpdateTick(float dt)
{
	bool ret = true;

	App->test_ticks++;



	return ret;
}

bool j1Player::Update(float dt)
{
	bool ret = true;

	return ret;
}