#ifndef _TextBox_H_
#define _TextBox_H_

#include "Interactables.h"
#include "Label.h"

class TextBox : public Interactable {
public:
	TextBox() { text = nullptr; }

	TextBox(SDL_Rect& rect, float size) { coll_rect = this->image_rect = rect; this->scale = size; text = new Label(rect, size); }

	bool Start();
	bool SpecificPreUpdate();
	bool SpecificPostUpdate();

	bool Draw();
	bool CleanUp()
	{
		bool ret = true;

		if (last_input != nullptr)
			delete last_input;
		last_input = nullptr;

		delete text;

		return ret;
	}

	void OnClick() { IsActive = true; }
	void OnHover() {};

	void SetInactive() { IsActive = false; }

public:
	bool IsActive = false;
	char* last_input;
	Label* text;
};

bool TextBox::Start()
{
	bool ret = true;
	
	return true;
}
bool TextBox::SpecificPreUpdate()
{
	if(IsActive)
		last_input = new char(App->input->GetPressedKey());
	
	return true;
}

bool TextBox::SpecificPostUpdate()
{
	if (IsActive)
	{
		text->content.create("%s%s", text->content.GetString(), last_input);

		delete last_input;
		last_input = nullptr;
	}

	delete last_input;

	return true;
}

bool TextBox::Draw()
{
	bool ret = true;


	text->Draw();

	return ret;
}
#endif //_TextBox_H_