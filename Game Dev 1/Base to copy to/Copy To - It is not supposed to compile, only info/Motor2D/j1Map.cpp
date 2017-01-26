#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>

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

	return ret;
}

bool j1Map::Start()
{
	tile_x = App->tex->Load("maps/x.png");
	return true;
}

//H7.8 Map Exploration/Pathfinding

void j1Map::ResetPath()
{
	frontier.Clear();
	visited.clear();
	breadcrumbs.clear();
	frontier.Push(iPoint(target.x, target.y), 0);
	visited.add(iPoint(target.x, target.y));
	breadcrumbs.add(iPoint(target.x, target.y));
	memset(cost_so_far, 0, sizeof(uint) * COST_MAP * COST_MAP);
}

void j1Map::Path(int x, int y)
{
	path.Clear();
	if (target_obt == true) {
		iPoint goal = WorldToMap(x, y);
		iPoint start(target.x, target.y);
		// TODO 2: Follow the breadcrumps to goal back to the origin
		// add each step into "path" dyn array (it will then draw automatically)
		if (visited.find(goal) != -1) { //makes sure the goal has been found
			while (goal != start) { //conditional, goal is not start
				path.PushBack(goal); //puts goal at the end of path 1st time, then puts more breadcrumbs
				goal = breadcrumbs[visited.find(goal)]; //goal now is a breadcrumb in the direction of goal?
			}
			path.PushBack(goal); //?????
		}
	}

}

void j1Map::PropagateDijkstra()
{
	// TODO 3: Taking BFS as a reference, implement the Dijkstra algorithm
	// use the 2 dimensional array "cost_so_far" to track the accumulated costs
	// on each cell (is already reset to 0 automatically)
	if (target_obt == true) {
		if (visited.find(target) == -1) { //Target is new start, so it checks that it has been set somewhere
			iPoint curr;
			if (frontier.Pop(curr)) //Put actual frontier into curr, while it checks if there are frontiers left
			{
				iPoint neighbors[4]; //Create the 4 neighbours every tile has
				neighbors[0].create(curr.x + 1, curr.y + 0);
				neighbors[1].create(curr.x + 0, curr.y + 1);
				neighbors[2].create(curr.x - 1, curr.y + 0);
				neighbors[3].create(curr.x + 0, curr.y - 1);

				for (uint i = 0; i < 4; ++i)
				{
					if (MovementCost(neighbors[i].x, neighbors[i].y) != -1 && visited.find(neighbors[i]) == -1) //checks if walkable and if visited
					{
						int new_cost = cost_so_far[curr.x][curr.y] + MovementCost(neighbors[i].x, neighbors[i].y); //Calculates new cost = cost before + added cost for that neighbour
						if (cost_so_far[neighbors[i].x][neighbors[i].y] == 0 || new_cost < cost_so_far[neighbors[i].x][neighbors[i].y]) { //Checks that cost_so_far has been initialized, or that cost_so_far > new cost so that it is added (if cost is bigger, then it is not optimal?)
							cost_so_far[neighbors[i].x][neighbors[i].y] = new_cost; //Puts new cost
							frontier.Push(neighbors[i], new_cost); //Pushes the neighbor + cost from target to that neighbour, to be later calculated
							visited.add(neighbors[i]); //adds neighbour
							breadcrumbs.add(curr); //adds only current (why?)
						}
					}
				}
			}
		}
	}
}

void j1Map::PropagateAStar(int method)
{
	// TODO 3: Taking BFS as a reference, implement the Dijkstra algorithm
	// use the 2 dimensional array "cost_so_far" to track the accumulated costs
	// on each cell (is already reset to 0 automatically)
	if (target_obt == true) {
		if (visited.find(target) == -1) {
			iPoint curr;
			if (frontier.Pop(curr))
			{
				iPoint neighbors[4];
				neighbors[0].create(curr.x + 1, curr.y + 0);
				neighbors[1].create(curr.x + 0, curr.y + 1);
				neighbors[2].create(curr.x - 1, curr.y + 0);
				neighbors[3].create(curr.x + 0, curr.y - 1);

				for (uint i = 0; i < 4; ++i)
				{
					if (MovementCost(neighbors[i].x, neighbors[i].y) > 0 && visited.find(neighbors[i]) == -1)
					{
						int dist = 0;
						switch (method)
						{
						case 1:
							dist = neighbors[i].DistanceManhattan(target);
							break;
						case 2:
							dist = neighbors[i].DistanceTo(target);
							break;
						case 3:
							dist = neighbors[i].DistanceNoSqrt(target);
							break;
						default:
							break;
						}
						int new_cost = cost_so_far[curr.x][curr.y] + MovementCost(neighbors[i].x, neighbors[i].y) + dist;
						if (cost_so_far[neighbors[i].x][neighbors[i].y] == 0 || new_cost < cost_so_far[neighbors[i].x][neighbors[i].y]) {
							cost_so_far[neighbors[i].x][neighbors[i].y] = new_cost;
							frontier.Push(neighbors[i], new_cost);
							visited.add(neighbors[i]);
							breadcrumbs.add(curr);
						}
					}
				}
			}
		}
	}
}

int j1Map::MovementCost(int x, int y) const
{
	int ret = -1;

	if (x >= 0 && x < data.width && y >= 0 && y < data.height)
	{
		int id = data.layers.start->next->data->Get(x, y); //Get tile id (0 = not water?, or walkable meta layer?)

		if (id == 0)
			ret = 3;
		else
			ret = 0;
	}

	return ret;
}

void j1Map::PropagateBFS()
{
	// TODO 1: Record the direction to the previous node 
	// with the new list "breadcrumps"
	if (target_obt == true) {
		iPoint curr; //Base iPoint, where we are
		if (frontier.Pop(curr)) //Put actual frontier into curr, while it checks if there are frontiers left
		{
			iPoint neighbors[4]; //Create the 4 neighbours every tile has
			neighbors[0].create(curr.x + 1, curr.y + 0);
			neighbors[1].create(curr.x + 0, curr.y + 1);
			neighbors[2].create(curr.x - 1, curr.y + 0);
			neighbors[3].create(curr.x + 0, curr.y - 1);

			for (uint i = 0; i < 4; ++i)
			{
				if (MovementCost(neighbors[i].x, neighbors[i].y) > 0) //movement costs checks if it actually is a walkable tile
				{
					if (visited.find(neighbors[i]) == -1) //Checks for visited tiles (if visited they don't go in)
					{
						frontier.Push(neighbors[i], 0); //Add them as a frontier
						visited.add(neighbors[i]); //Add as a neighbour, why?
						breadcrumbs.add(curr); //Leave breadcrumb, to make path just gets the tiles you go through?
					}
				}
			}
		}
	}
}

void j1Map::DrawPath()
{
	if (target_obt == true) {
		iPoint point;

		// Draw visited
		p2List_item<iPoint>* item = visited.start;

		while (item)
		{
			point = item->data;
			TileSet* tileset = GetTilesetFromTileId(26);

			SDL_Rect r = tileset->GetTileRect(26);
			iPoint pos = MapToWorld(point.x, point.y);

			App->render->Blit(tileset->texture, pos.x, pos.y, &r);

			item = item->next;
		}

		// Draw frontier
		for (uint i = 0; i < frontier.Count(); ++i)
		{
			point = *(frontier.Peek(i));
			TileSet* tileset = GetTilesetFromTileId(25);

			SDL_Rect r = tileset->GetTileRect(25);
			iPoint pos = MapToWorld(point.x, point.y);

			App->render->Blit(tileset->texture, pos.x, pos.y, &r);
		}

		// Draw path
		for (uint i = 0; i < path.Count(); ++i)
		{
			iPoint pos = MapToWorld(path[i].x, path[i].y);
			App->render->Blit(tile_x, pos.x, pos.y);
		}
	}
}
//

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	p2List_item<MapLayer*>* item = data.layers.start;

	//H7 TODO 4: Make sure we draw all the layers and not just the first one

	for(; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if(layer->properties.Get("Nodraw") != 0)
			continue;

		//H4 TODO 6: Iterate all tilesets and draw all their 
		// images in 0,0 (you should have only one tileset for now)

		//H5 TODO 5: Prepare the loop to draw all tilesets + Blit
		// We draw 1 tile each time

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				//H5 TODO 9: Complete the draw function

				int tile_id = layer->Get(x, y); // Each have a x, y; and transform to 1 dimension
				if(tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id); // Later we read tileset sprite id

					SDL_Rect r = tileset->GetTileRect(tile_id); // Then find out their tileset Rect
					iPoint pos = MapToWorld(x, y); // Now find tile place in theworld

					App->render->Blit(tileset->texture, pos.x, pos.y, &r); // Now we can Blit!
				}
				//
			}
		}
	}
	//
}

//H7 TODO 7: Our custom properties should have one method
// to ask for the value of a custom property
int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while(item)
	{
		if(item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{

	//H7 TODO 3: Complete this method so we pick the right
	// Tileset based on a tile id

	p2List_item<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;

	while(item)
	{
		if(id < item->data->firstgid) //The attribute firstgid sets the first tile id that this tileset contains
		{
			set = item->prev->data;
			break;
		}
		set = item->data;
		item = item->next;
	}

	//

	return set;
}

//H5 TODO 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	
	//H6 TODO 1 Case Isometric
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	//

	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}
//

//H4 TODO x?
iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);
	//H6 TODO 2: Add orthographic world to map coordinates
	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	//

	//H6 TODO 3: Add the case for isometric maps to WorldToMap
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2) - 1;
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	//

	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

// TODO 7: Create a method that receives a tile id and returns it's Rectfind the Rect associated with a specific tile id
//Orthogonal: ret.x = x * tile_width   ret.y = y * tile_height
//Isometric:  ret.x = (x - y) * (tile width * 0.5f)      ret.y = (x + y) * (tile_height * 0.5f)
SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}
//

// Called before quitting, Clean up all things
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	char* buf;
	int size = App->fs->Load(tmp.GetString(), &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);

	RELEASE(buf);

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		//H4 TODO 3: Create and call a private function to load and fill
		// all your map data
		ret = LoadMap();
		//
	}

	// Load all tilesets info ----------------------------------------------
	//H4 TODO 4: Create and call a private function to load a tileset
	// remember to support more any number of tilesets!
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}
	//


	// Load layer info ----------------------------------------------
	//H5 TODO 4: Iterate all layers and load each of them
	//Similar to Load tilesets
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.add(lay);
	}
	//

	//H4 TODO 5: LOG all the data loaded
	// iterate all tilesets and LOG everything
	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}
	//

	map_loaded = ret;

	return ret;
}

// Load map general properties
//H4 TODO 3: Create and call a private function to load and fill
// all your map data
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}
//


bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	//H7 TODO 6: Fill in the method to fill the custom properties from an xml_node

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.add(p);
		}
	}
	//

	return ret;
}

/*

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	p2List_item<MapLayer*>* item;
	item = data.layers.start;

	for(item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;
				
				if(tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

*/