#ifndef _TextBox_H_
#define _TextBox_H_

#include "Interactables.h"
#include "Label.h"

class TextBox : public Interactable {
public:
	TextBox() {}

	TextBox(SDL_Rect& rect, float size) { coll_rect = image_rect = rect; scale = size; }

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool SpecificPreUpdate();
	bool SpecificPostUpdate();

	bool Draw();
	bool CleanUp()
	{
		bool ret = true;

		return ret;
	}

	void OnClick();
	void OnHover() {};

public:
	bool IsActive = false;
	Label text;
	iPoint text_offset;
};

bool TextBox::Awake(pugi::xml_node& config)
{
	text.Awake(config);

	position = { config.attribute("posx").as_int(), config.attribute("posy").as_int() };

	text_offset = { config.attribute("offx").as_int(), config.attribute("offy").as_int() };

	text.scale = scale;

	coll_rect.x = position.x;
	coll_rect.y = position.y;

	return true;

}

bool TextBox::Start()
{
	bool ret = true;
	
	return true;
}

bool TextBox::SpecificPreUpdate()
{	
	if ((state == Leave || state == Out) && is_click) {
		IsActive = false;
		App->input->StopBuffer();
	}

	return true;
}

bool TextBox::SpecificPostUpdate()
{
	if (IsActive)
	{
		if(App->input->GetTextBuffer() != "")
			text.content.create("%s%s", text.content.GetString(), App->input->GetTextBuffer());
	}

	Draw();
	return true;
}

bool TextBox::Draw()
{
	bool ret = true;

	App->render->Blit(point_atlas, position.x, position.y, &image_rect);
	text.Draw({ position.x + text_offset.x, position.y + text_offset.y });

	return ret;
}

void TextBox::OnClick()
{
	IsActive = true; 
	App->input->StartBuffer();
}
#endif //_TextBox_H_