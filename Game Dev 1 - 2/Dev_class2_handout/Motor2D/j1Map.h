#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Textures.h"

// TODO 3.2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ----------------------------------------------------

struct tileset_info {
	uint firstgid;
	p2SString name;
	uint tilewidth;
	uint tileheight;
	uint spacing;
	uint margin;

	uint tilecount;

	uint columns;

	// Image
	SDL_Texture* tex;
	const char*	image_source;
	uint		image_width;
	uint		image_height;
};

// TODO 3.1: Create a struct needed to hold the information to Map node
enum orientation {
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
	uint	orientation;
	uint	renderorder;
	uint	width;
	uint	height;
	uint	tilewidth;
	uint	tileheight;

	uint	nextobjectid;

	p2List<tileset_info*> tilesets;
	
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
	bool LoadMapData(const char* path, p2List_item<Map_info*>* item,pugi::xml_node* root_node);
	bool LoadTilesetData(pugi::xml_node* data_node, p2List_item<tileset_info*>* item_tileset);


public:

	// TODO 3.1: Add your struct for map info as public for now
	p2List<Map_info*> Maps;

	// MAPS that will be loaded
	Map_info*	hello;
	Map_info*	Hello2;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__