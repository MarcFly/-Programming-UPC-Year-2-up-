#ifndef _INTERACTABLES_H_
#define _INTERACTABLES_H_

#include "UI_Elements.h"

class Interactable : public UI_Element {
public:

	virtual bool SpecificPreUpdate() {return true;}

	bool PreUpdate()
	{
		bool ret = true;

		is_hover = CheckHover();

		if (is_hover) {
			is_click = CheckClick();
			is_hover = !is_click;
		}

		ret = SpecificPreUpdate();

		return ret;
	}

	virtual bool SpecificPostUpdate() { return true; }

	bool PostUpdate()
	{
		bool ret = true;

		if (is_click)
			OnClick();
		else if (is_hover)
			OnHover();

		ret = SpecificPostUpdate();

		return ret;
	}

public:
	bool CheckHover()
	{
		bool ret = true;
		
		SDL_Rect result;
		SDL_Rect mouse = { 0,0,2,2 };
		App->input->GetMousePosition(mouse.x, mouse.y);

		SDL_IntersectRect( &mouse, &coll_rect, &result);

		ret = (result.w > 0 && result.h > 0);

		return ret; 
	};

	bool CheckClick()
	{
		bool ret = false;

		if (CheckHover() && App->input->GetMouseButtonDown(0) == KEY_DOWN) {
			OnClick();

			return ret;
		}

		return ret;
	}

	virtual void OnClick() {};
	virtual void OnHover() {};

public:
	SDL_Rect coll_rect;
	bool is_hover = false;
	bool is_click = false;
};

#endif //_INTERACTABLES_H_