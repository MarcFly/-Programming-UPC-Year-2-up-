#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Queue.h"
#include "p2Point.h"
#include "j1Module.h"

// ----------------------------------------------------
//H7 TODO 5: Create a generic structure to hold properties
struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while(item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	//H7 TODO 7
	int Get(const char* name, int default_value = 0) const;
	//

	p2List<Property*>	list;
};

// ----------------------------------------------------
//H5 TODO 1: Create a struct for the map layer
struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}
	//H5 TODO 6: Inside struct for the layer, create a short method to get the value of x,y
	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
	//
};

// ----------------------------------------------------
//H4 TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------

//H4 TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;

	//H5 TODO 2: Add a list/array of layers to the map!
	p2List<MapLayer*>	layers;
	//
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

	//H5 TODO 8
	iPoint MapToWorld(int x, int y) const;
	//

	iPoint WorldToMap(int x, int y) const;

	//H7.5 BFS
	void PropagateBFS();
	void DrawBFS();
	bool IsWalkable(int x, int y) const;
	void ResetBFS();


	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);

	//H5 TODO 3: Create a method that loads a single layer
	//bool LoadLayer(pugi::xml_node& node, your_layer_struct* layer);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	//

	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;

	/// BFS
	p2Queue<iPoint>		frontier;
	p2List<iPoint>		visited;

};

#endif // __j1MAP_H__