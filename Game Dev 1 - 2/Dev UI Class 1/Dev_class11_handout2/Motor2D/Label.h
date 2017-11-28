#ifndef _LABEL_H_
#define _LABEL_H_

#include "UI_Elements.h"


class Label : public UI_Element {
public:
	Label() {}

	Label(SDL_Rect& rect, float size) { this->image_rect = rect; this->scale = size; }

	Label(_TTF_Font* f, const char* c) { content.create(c); font = f; }

	~Label() { content.Clear(); font = nullptr; }

public:
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool SpecificPreUpdate();
	bool SpecificPostUpdate();
	bool Draw();
	bool CleanUp();

public:
	p2SString content;
	_TTF_Font* font = nullptr;

};

bool Label::Awake(pugi::xml_node& config)
{
	return true;
}

bool Label::Start()
{
	return true;
}

bool SpecificPreUpdate()
{
	return true;
}

bool SpecificPostUpdate()
{
	return true;
}

bool Draw()
{
	return true;
}

bool CleanUp()
{
	return true;
}

#endif //_LABEL_H_