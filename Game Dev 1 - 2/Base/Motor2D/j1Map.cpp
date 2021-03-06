#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
//#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include <string>
#include "j1Pathfinding.h"

struct SDL_Texture;

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(const pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if (map_loaded == false)
		return;

	/// TODO 3.6: Iterate all tilesets and draw all their 
	/// images in 0,0 (you should have only one tileset for now)

	// TODO 4.5 Loop to draw all tilesets + Blit
	// TODO 4.9 Complete draw function for layers and tiles

	iPoint pos = { 0,0 };

	if (Maps != nullptr) { //Check there is a FindRectmap
		
		tileset_info* item_tileset; //Start tileset list
	
		p2List_item<layer_info*>* item_layer = Maps->layers.start; //Start layer

		while (item_layer != nullptr) { //Check there are layers

			item_tileset = GetTilesetFromTileId(*item_layer->data->data);
			
			uint* p = item_layer->data->data; // reset data pointing to data[0]

			for (int i = 0; i < item_layer->data->height; i++) {
				for (int j = 0; j < item_layer->data->width; j++) {
					if (*p != 0) {
						iPoint pos = MapToWorld(j, i);

						App->render->Blit(
							item_tileset->image.tex,
							pos.x - item_tileset->tileoffset.x,
							pos.y- item_tileset->tileoffset.y,
							&item_tileset->GetRect(*p));
					}
					p++;
				}
			}

		item_layer = item_layer->next;
		}
	
	}

	DrawNav();
	
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// TODO 3.2: Make sure you clean up any memory allocated
	// from tilesets / map
	delete Maps;

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
		pugi::xml_node root_node = check_doc.child("map");
		Maps = new Map_info();

		LoadMapData(root_node, *Maps);
	}

	// TODO 3.4: Create and call a private function to load a tileset
	// remember to support more any number of tilesets!

	if (ret == true) {
		pugi::xml_node tileset_node = check_doc.child("map").child("tileset");

		if (tileset_node.attribute("firstgid").as_uint() != 0) {

			while (tileset_node.attribute("firstgid").as_int() > 0) {
				

				tileset_info* item_tileset = new tileset_info;
				ret = LoadTilesetData(tileset_node, *item_tileset);
				tileset_node = tileset_node.next_sibling("tileset");

			
				Maps->tilesets.add(item_tileset);


			}

			map_loaded = true;
		}
		else {
			map_loaded = false;
			LOG("There is no tileset!/n");
		}

	}

	if (ret == true) {
		pugi::xml_node layer_node = check_doc.child("map").child("layer");

		while (layer_node.attribute("name").as_string() != "") {
			layer_info* item_layer = new layer_info;
			ret = LoadLayerData(layer_node, *item_layer);
			layer_node = layer_node.next_sibling("layer");
			Maps->layers.add(item_layer);

		}
	}

	if (ret == true)
	{
		// TODO 3.5: LOG all the data loaded
		// iterate all tilesets and LOG everything
		//App->pathfinding->SetMap(Maps->width, Maps->height, Maps->layers.start->data->data);
	}
	
	ret = map_loaded;

	return ret;
}

// Load INFO--------------------------------------------------------------------------------------------------------------------
bool j1Map::LoadMapData(const pugi::xml_node& map_node, Map_info& item_map) {
	bool ret = true;

	// Orientation
	p2SString cmp;
	cmp.create(map_node.attribute("orientation").as_string());

	if (cmp == "orthogonal") item_map.map_type = orthogonal;
	else if (cmp == "isometric") item_map.map_type = isometric;
	else if (cmp == "staggered") item_map.map_type = staggered;
	else if (cmp == "hexagonal") item_map.map_type = hexagonal;
	else item_map.map_type = unknown_;

	// Renderorder
	cmp.Clear();
	cmp.create(map_node.attribute("renderorder").as_string());

	if (cmp == "right-down") item_map.renderorder = right_down;
	else if (cmp == "right-up") item_map.renderorder = right_up;
	else if (cmp == "left-down") item_map.renderorder = left_down;
	else if (cmp == "left-up") item_map.renderorder = left_up;
	else item_map.renderorder = unknown;

	item_map.width = map_node.attribute("width").as_uint();
	item_map.height = map_node.attribute("height").as_uint();
	item_map.tilewidth = map_node.attribute("tilewidth").as_uint();
	item_map.tileheight = map_node.attribute("tileheight").as_uint();
	item_map.nextobjectid = map_node.attribute("nextobjectid").as_uint();

	return ret;
}

bool j1Map::LoadTilesetData(const pugi::xml_node& tileset_node, tileset_info& item_tileset) {
	bool ret = true;

	// Tileset info
	item_tileset.firstgid = tileset_node.attribute("firstgid").as_uint();
	item_tileset.name = tileset_node.attribute("name").as_string();
	item_tileset.tilewidth = tileset_node.attribute("tilewidth").as_uint();
	item_tileset.tileheight = tileset_node.attribute("tileheight").as_uint();
	item_tileset.spacing = tileset_node.attribute("spacing").as_uint();
	item_tileset.tileoffset.x = tileset_node.child("tileoffset").attribute("x").as_uint();
	item_tileset.tileoffset.y = tileset_node.child("tileoffset").attribute("y").as_uint();
	item_tileset.margin = tileset_node.attribute("margin").as_uint();

	item_tileset.tilecount = tileset_node.attribute("tilecount").as_uint();

	// Image info
	item_tileset.image.image_source.create("%s%s",folder.GetString(), tileset_node.child("image").attribute("source").as_string());
	item_tileset.image.image_width = tileset_node.child("image").attribute("width").as_uint();
	item_tileset.image.image_height = tileset_node.child("image").attribute("height").as_uint();

	item_tileset.image.tex = App->tex->Load(item_tileset.image.image_source.GetString());

	item_tileset.columns = item_tileset.image.image_width / item_tileset.tilewidth;

	// Load terrains
	for (int i = item_tileset.firstgid; i <= item_tileset.firstgid  + item_tileset.tilecount; i++) {

		terrain_info* item_terrain = new terrain_info;
		ret = LoadTerrainData(tileset_node, i, *item_terrain, item_tileset);
		item_tileset.terrains.add(item_terrain);

	}

	return ret;
}

// Load Terrain Data
bool j1Map::LoadTerrainData(const pugi::xml_node& tileset_node, const int& id, terrain_info& item_terrain, tileset_info& item_tileset) {
	
	item_terrain.id = id;

	item_terrain.Tex_Pos = new SDL_Rect(item_tileset.GetRect(item_terrain.id));

	// Create a switch depending on Id to create later collision data for later creation
	// There should be a way of detecting which type of layer we are in, and then load properties, no matter what type it is
	// Then load different properties depending on it, how tho?
	// Recommended with an int and just do a switch to choose laoding functions, no enum needed (you can comment what type of layer it is)
	// Or do with string and cmp (more memory tho)
	// switch(
	//pugi::xpath_node_set terrain_node = tileset_node.select_node("/tile[@id='%d']", id); Test the select node method to load a specific node instead of iterating to find


	return true;
}

// Load Layer Data
bool j1Map::LoadLayerData(const pugi::xml_node& layer_node, layer_info& item_layer) {
	
	item_layer.name = layer_node.attribute("name").as_string();
	item_layer.width = layer_node.attribute("width").as_uint();
	item_layer.height = layer_node.attribute("height").as_uint();

	item_layer.draw_mode = layer_node.child("properties").child("property").attribute("value").as_int();

	//Load all tiles in layer data
	pugi::xml_node tile_node = layer_node.child("data").child("tile");
	pugi::xml_node gid_check = layer_node.previous_sibling("tileset").previous_sibling("tileset").last_child();
	

	if (tile_node.attribute("gid").as_int() <= -1) LOG("There are no valid tiles, RETARDTIST!!!!\n");

	item_layer.size = item_layer.width * item_layer.height;
	item_layer.data = new uint[item_layer.size];
	
	uint* p = item_layer.data; // TO go through a "pointer" list you have to go through while going in another pointer so you are not changing where its pointing to the original one but giving the value
	for (int i = 0; i < item_layer.size; i++) {
		*p = tile_node.attribute("gid").as_uint();
		tile_node = tile_node.next_sibling("tile");
		p++;
	}

	if (item_layer.draw_mode == 2)
		App->pathfinding->SetMap(item_layer.width, item_layer.height, item_layer.data);

	return true;
}


// Position Funcitons------------------------------------------------------------------

iPoint j1Map::MapToWorld(int x, int y) const {

	if (Maps->map_type == orthogonal) {
		return { 
			x * (int)Maps->tilewidth,
			y * (int)Maps->tileheight 
		};
	}
	else if (Maps->map_type == isometric) {
		return{ 
			((x - y) * (((int)Maps->tilewidth) / 2)),
			abs(x + y) * (((int)Maps->tileheight) / 2)
		};
	}
	else
		return { 0,0 };
}

iPoint j1Map::WorldToMap(int rx, int ry) const {

	if (Maps->map_type == orthogonal) {
		return {
			rx / (int)Maps->tilewidth,
			ry / (int)Maps->tileheight 
		};
	}
	else if (Maps->map_type == isometric) {
		float half_w = Maps->tilewidth * 0.5f;
		float half_h = Maps->tileheight * 0.5f;

		return{ 
			(int)((((rx - half_w)/ half_w) + ((ry)/ half_h)) / 2),
			(int)((((ry)/ half_h) - ((rx - half_w)/ half_w)) / 2)
		};
	}
	else
		return { 0,0 };
}


// Find Functions

tileset_info* j1Map::GetTilesetFromTileId(int gid) const
{
	p2List_item<tileset_info*>* item = Maps->tilesets.start;

	while (Maps->tilesets.count() > 1 && (gid < item->data->firstgid || gid >= item->data->firstgid + item->data->tilecount))
		item = item->next; // This only works if layer only works over 1 tileset, if multiple tilesets in a layer, you have to do it before every blit?

	return item->data;

}

void j1Map::DrawNav() {
	iPoint point;

	// Draw visited
	p2List_item<iPoint>* item = App->pathfinding->visited.start;

	while (item)
	{
		point = item->data;
		tileset_info* tileset = GetTilesetFromTileId(25); //Get green rect

		SDL_Rect r = tileset->GetRect(25);
		iPoint pos = MapToWorld(point.x, point.y);

		App->render->Blit(tileset->image.tex, pos.x, pos.y - tileset->tileoffset.y, &r);

		item = item->next;
	}

	// Draw frontier
	for (uint i = 0; i < App->pathfinding->frontier.Count(); ++i)
	{
		point = *(App->pathfinding->frontier.Peek(i));
		tileset_info* tileset = GetTilesetFromTileId(26); //Get the red rect

		SDL_Rect r = tileset->GetRect(26);
		iPoint pos = MapToWorld(point.x, point.y);

		App->render->Blit(tileset->image.tex, pos.x - tileset->tileoffset.x, pos.y - tileset->tileoffset.y, &r);
	}

	// Draw pfrontier
	for (uint i = 0; i < App->pathfinding->pfrontier.Count(); ++i)
	{
		point = *(App->pathfinding->pfrontier.Peek(i));
		tileset_info* tileset = GetTilesetFromTileId(26); //Get the red rect

		SDL_Rect r = tileset->GetRect(26);
		iPoint pos = MapToWorld(point.x, point.y);

		App->render->Blit(tileset->image.tex, pos.x - tileset->tileoffset.x, pos.y - tileset->tileoffset.y, &r);
	}

	DrawPath();
	DrawNPath();
}

void j1Map::DrawPath() {
	// Draw Path

	p2List_item<iPoint>* item = App->pathfinding->path.start;
	while (item)
	{
		tileset_info* tileset = GetTilesetFromTileId(26); //Get green rect

		SDL_Rect r = tileset->GetRect(26);
		iPoint pos = MapToWorld(item->data.x, item->data.y);

		App->render->Blit(tileset->image.tex, pos.x, pos.y - tileset->tileoffset.y, &r);

		item = item->next;
	}

}

void j1Map::DrawNPath() {
	
	for (int i = 0; i < App->pathfinding->closed_test.Count(); i++) {
		iPoint pos_ = App->pathfinding->closed_test[i];
		tileset_info* tileset = GetTilesetFromTileId(26); //Get green rect

		SDL_Rect r = tileset->GetRect(25);
		iPoint pos = MapToWorld(pos_.x, pos_.y);

		App->render->Blit(tileset->image.tex, pos.x, pos.y - tileset->tileoffset.y, &r);
	}

	for (int i = 0; i < App->pathfinding->last_path.Count(); i++) {
		iPoint pos_ = App->pathfinding->last_path[i];
		tileset_info* tileset = GetTilesetFromTileId(26); //Get green rect

		SDL_Rect r = tileset->GetRect(25);
		iPoint pos = MapToWorld(pos_.x, pos_.y);

		App->render->Blit(tileset->image.tex, pos.x, pos.y - tileset->tileoffset.y, &r);
	}
}