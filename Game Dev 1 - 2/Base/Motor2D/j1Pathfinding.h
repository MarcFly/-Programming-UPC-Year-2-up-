#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Queue.h"
#include "p2PQueue.h"
#include "p2Point.h"
#include "p2List.h"
#include "p2DynArray.h"
#include "j1Textures.h"


#define COST_MAP 100

struct PathNode {
	uchar	data;
	iPoint	pos;

	// Methods
	int	 MoveCost() const {
		int ret = -1;
		if (pos.x >= 0 && pos.y < App->map->Maps->width && pos.y >= 0 && pos.y < App->map->Maps->height)
		{
			if (data == 0)
				ret = 3;
			else
				ret = -1;
		}
		return ret;
	}

	// 
};

struct PathList {
	p2List<PathNode>	nodes;
	// What?
};

class j1Pathfinding : public j1Module
{
// Pathfinding

public:

	// Called before render is available
	bool Awake(const pugi::xml_node& conf);

	// Called before quitting
	bool CleanUp();

	// Set map info for usage
	void SetMap(uint width, uint height, uchar* data);
	
	// Path things
	int CreateFPath(const iPoint& origin, const iPoint& destination);
	const p2DynArray<iPoint>* GetLastPath() const;
	bool IsWalkable(int x, int y) const;
	void ResetNav();
	void CreatePath(const iPoint& pos);
	
	void SetStart(const iPoint& pos);
	int	 MovementCost(int x, int y) const;

	// Propagation
	iPoint PropagateBFS();
	void PropagateToBFS(const iPoint& pos);

	iPoint PropagateDijkstra();
	void PropagateToDijkstra(const iPoint& pos);

	void PropagateAStar(const iPoint& pos);

public:
	//Pathfinding~ Hacked bullshit
	iPoint				start;
	p2Queue<iPoint>		frontier;
	p2PQueue<iPoint>	pfrontier;
	p2List<iPoint>		visited;
	p2List<iPoint>		breadcrumbs;
	p2List<iPoint>		path;

	uint				cost_so_far[COST_MAP][COST_MAP];

	SDL_Texture*		tile_x = nullptr;

	//Pathfinding Proper Bullshit
};

#endif //__j1PATHFINDING_H__