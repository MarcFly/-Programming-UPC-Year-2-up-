#include "p2Defs.h"
#include "p2Log.h"
#include "p2List.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

struct SDL_Texture;

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	// Add Maps to the list
	hello = new Map_info;
	Hello2 = new Map_info;

	//AddMap(hello);
	//AddMap(Hello2);

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;

	// TODO 3.6: Iterate all tilesets and draw all their 
	// images in 0,0 (you should have only one tileset for now)
	p2List_item<Map_info*>* item_map = Maps.start;
	p2List_item<tileset_info*>* item_tileset = Maps.start->data->tilesets.start;

	while (item_map != nullptr) {

		//item_tileset = item_map->data->tilesets.start;

		while (item_tileset != nullptr) {

			App->render->Blit(item_tileset->data->tex, 0, 0);
			item_tileset = item_tileset->next;

		}

		item_map = item_map->next;
	}
	
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 3.2: Make sure you clean up any memory allocated
	// from tilesets / map
	Maps.clear();

	//map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);
	
	pugi::xml_document check_doc;

	pugi::xml_parse_result result =  check_doc.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	if (ret == true)
	{
		// TODO 3.3: Create and call a private function to load and fill
		// all your map data
		//pugi::xml_document new_map;

		//pugi::xml_parse_result result = map_file.load(file_name);

		//Map_info* tmp = new Map_info;
		//if (result.description() != nullptr)
		//p2List_item<Map_info*> tmp = new Map_info;

		pugi::xml_node root_node = check_doc.child("map");
		Map_info* item = new Map_info();
		//p2List_item<Map_info*>* item_map = Maps.start;

		LoadMapData(file_name, item, &root_node);
		Maps.add(item);

		//delete item;
	}

	// TODO 3.4: Create and call a private function to load a tileset
	// remember to support more any number of tilesets!


	if (ret == true)
	{
		// TODO 3.5: LOG all the data loaded
		// iterate all tilesets and LOG everything
	}
	
	ret = map_loaded;

	return ret;
}

void j1Map::AddMap(Map_info* name) {
	//Maps.add(name);
}

bool j1Map::LoadMapData(const char* path, Map_info* item_map, pugi::xml_node* map_node) {
	bool ret = true;

	//pugi::xml_node map_node = root_node->child("map");

	item_map->width = map_node->attribute("width").as_uint();
	item_map->height = map_node->attribute("height").as_uint();
	item_map->tilewidth = map_node->attribute("tilewidth").as_uint();
	item_map->tileheight = map_node->attribute("tileheight").as_uint();
	item_map->nextobjectid = map_node->attribute("nextobjectid").as_uint();

	uint test = map_node->child("tileset").attribute("firstgid").as_uint();

	if (map_node->child("tileset").attribute("firstgid").as_uint() == map_node->attribute("nextobjectid").as_uint()) {

		tileset_info* item_tileset = new tileset_info;
		pugi::xml_node tileset_node = map_node->child("tileset");

		while (tileset_node.attribute("firstgid").as_int() > 0) {

			LoadTilesetData(&tileset_node, item_tileset);

			tileset_node = tileset_node.parent().next_sibling("tileset");

			item_map->tilesets.add(item_tileset);

			//delete item_tileset;
		}

		map_loaded = true;
	}
	else
		map_loaded = false;

	return ret;
}

bool j1Map::LoadTilesetData(pugi::xml_node* data_node, tileset_info* item_tileset) {
	bool ret = true;

	item_tileset->firstgid = data_node->attribute("firstgid").as_uint();
	item_tileset->name = data_node->attribute("name").as_uint();
	item_tileset->tilewidth = data_node->attribute("tilewidth").as_uint();
	item_tileset->tileheight = data_node->attribute("tileheight").as_uint();
	item_tileset->spacing = data_node->attribute("spacing").as_uint();
	item_tileset->margin = data_node->attribute("margin").as_uint();

	item_tileset->tilecount = data_node->attribute("tilecount").as_uint();

	item_tileset->columns = data_node->attribute("columns").as_uint();


	// Image info
	*data_node = data_node->child("image");

	item_tileset->image_source = data_node->attribute("source").as_string();
	item_tileset->image_width = data_node->attribute("width").as_uint();
	item_tileset->image_height = data_node->attribute("height").as_uint();
	
	//item_tileset->tex = new SDL_Texture;
	item_tileset->tex = App->tex->Load(item_tileset->image_source);

	return ret;
}