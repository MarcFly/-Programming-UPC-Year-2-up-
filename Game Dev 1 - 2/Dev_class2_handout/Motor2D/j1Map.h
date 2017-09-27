#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Textures.h"

/*struct properties {
	
	struct property
	{
		p2SString name;
		bool value;
	};

	p2List<property*> list;

	~properties()
	{
		p2List_item<property*>* item = list.start;

		while (item != list.end) {
			RELEASE(*item);
			item++;
		}
		
	}
};

struct layer_info {
	p2SString	name;
	uint		width;
	uint		height;
	uint*		data;
	properties properties;

	layer_info() : data(NULL) {}

	~layer_info() { RELEASE(data); }

	inline uint Get(int x, int y) const { return data[(y*width) + x]; }

};

enum tile_types {
	unknown = -1,
	walkable,
	unwalkable,
	water,
	jumpable
	//keep adding types as you encounter them
};

struct tile_info{
	properties properties;
	int id;
	tile_types type;
};*/

// TODO 3.2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ----------------------------------------------------

struct tileset_info {
	uint		firstgid;
	p2SString	name;
	
	uint tilewidth;
	uint tileheight;
	uint spacing;
	uint margin;
	
	uint tilecount;
	uint columns;

	// Image
	
	SDL_Texture*	tex;
	const char*		image_source;
	uint			image_width;
	uint			image_height;

	//SDL_Rect GetTileRect(int id) const;
	//tile_info* GetTileType(int tile_id) const {};
};

// TODO 3.1: Create a struct needed to hold the information to Map node
enum orientation {
	unknown = -1,
	orthogonal = 0,
	isometric,
	staggered,
	hexagonal
};

enum renderorder {
	right_down = 0,
	right_up,
	left_down,
	left_up
};

struct Map_info {
	pugi::xml_document map_file;

	orientation	map_type;
	//uint		renderorder;
	SDL_Color	bg_color;

	uint		width;
	uint		height;
	uint		tilewidth;
	uint		tileheight;

	uint	nextobjectid;

	p2List<tileset_info*> tilesets;
	//p2List<layer_info*> layers;

	//Map_info() {};
	
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Add a new map to list
	void AddMap(Map_info* name);

private:

	// TODO 3.3.2 Functions/Methods to load map data
	bool LoadMapData(const char* path, Map_info* item,pugi::xml_node* root_node);
	bool LoadTilesetData(pugi::xml_node* data_node, tileset_info* item_tileset);


public:

	// TODO 3.1: Add your struct for map info as public for now
	p2List<Map_info*> Maps;

	// MAPS that will be loaded
	Map_info*	hello;
	Map_info*	Hello2;

private:

	//pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__