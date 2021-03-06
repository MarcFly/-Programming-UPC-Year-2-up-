#include "p2Defs.h"
#include "p2Log.h"
#include "j1Gui.h"

#include "Label.h"
#include "Image.h"
#include "Button.h"
#include "HyperLink.h"
#include "TextBox.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& config)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = config.child("atlas").attribute("file").as_string("");

	pugi::xml_node object_node = config.child("objects").child("obj");

	while (object_node.attribute("type").as_string() != "")
	{
		SDL_Rect img_rect = {
			object_node.attribute("x").as_int(),
			object_node.attribute("y").as_int(),
			object_node.attribute("w").as_int(),
			object_node.attribute("h").as_int() };

		objects.add(CreateElement(
			img_rect,
			object_node.attribute("size").as_float(),
			object_node.attribute("type").as_int()));

		objects.end->data->Awake(object_node);

		object_node = object_node.next_sibling("obj");
	}
	


	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	bool ret = true;

	p2List_item<UI_Element*>* item = objects.start;

	for (int i = 0; i < objects.count(); i++) {
		if (item->data != nullptr) {
			ret = item->data->Start();
			item->data->point_atlas = atlas;
		}
		item = item->next;
	}

	return ret;

}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool ret = true;

	p2List_item<UI_Element*>* item = objects.start;

	for (int i = 0; i < objects.count(); i++){
		if (item->data != nullptr)
			ret = item->data->PreUpdate();
		item = item->next;
	}

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	bool ret = true;

	p2List_item<UI_Element*>* item = objects.start;

	for (int i = 0; i < objects.count(); i++) {
		if (item->data != nullptr)
			ret = item->data->PostUpdate();
		item = item->next;
	}

	return ret;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	bool ret = true;

	ret = CleanUI();

	return ret;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

bool j1Gui::CleanUI()
{
	bool ret = true;

	p2List_item<UI_Element*>* item = objects.start;

	for (int i = 0; i < objects.count(); i++) {
		if(item->data != nullptr)
			ret = item->data->CleanUp();
		item = item->next;
	}

	objects.clear();

	return ret;
}

UI_Element* j1Gui::CreateElement(SDL_Rect& rect, float size, int type)
{
	switch (type) {
	case (int)label:
		return (new Label(rect,size));
	case (int)image:
		return (new Image(rect, size));
	case (int)textbox:
		return (new TextBox(rect, size));
	case (int)button:
		return (new Button(rect, size));
	case (int)hyperlink:
		return (new HyperLink(rect, size));
	default:
		return nullptr;
	}

	return nullptr;
}
// class Gui ---------------------------------------------------

