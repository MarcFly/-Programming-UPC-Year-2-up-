#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Interactables.h"

class Button : public Interactable {
public:
	Button() {}

	Button(SDL_Rect& rect, float size) { coll_rect = this->image_rect = rect; this->scale = size; }

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool SpecificPreUpdate();
	bool SpecificPostUpdate();
	bool Draw();
	bool CleanUp();

	void OnClick();
	void OnHover();

public:

};

bool Button::Awake(pugi::xml_node& config)
{
	return true;
}

bool Button::Start()
{
	return true;
}

bool Button::SpecificPreUpdate()
{
	return true;
}

bool Button::SpecificPostUpdate()
{
	return true;
}

bool Button::Draw()
{
	return true;
}

bool Button::CleanUp()
{
	return true;
}

void Button::OnClick()
{

}

void Button::OnHover()
{

}

#endif //_BUTTON_H_