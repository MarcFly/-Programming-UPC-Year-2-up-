#ifndef _LABEL_H_
#define _LABEL_H_

#include "UI_Elements.h"


class Label : public UI_Element {
public:
	Label() {}

	Label(SDL_Rect& rect, float size) { image_rect = rect; scale = size; }

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

bool Label::SpecificPreUpdate()
{
	return true;
}

bool Label::SpecificPostUpdate()
{
	return true;
}

bool Label::Draw()
{
	return true;
}

bool Label::CleanUp()
{
	return true;
}

#endif //_LABEL_H_