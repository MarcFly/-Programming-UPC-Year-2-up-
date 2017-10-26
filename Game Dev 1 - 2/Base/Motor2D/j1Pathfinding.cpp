#include "j1App.h"
#include "j1Pathfinding.h"
#include "j1Map.h"

/// PATHFINDING

bool j1Pathfinding::Awake(const pugi::xml_node& config) {
	bool ret = true;

	start = { 19,4 };
	ResetNav(); // Start BFS

	return ret;
}

bool j1Pathfinding::CleanUp() {
	if (tile_x != nullptr)
		App->tex->UnLoad(tile_x);

	pfrontier.Clear();
	frontier.Clear();
	path.clear();
	visited.clear();
	breadcrumbs.clear();

	return true;
}

// BFS

iPoint j1Pathfinding::PropagateBFS() {
	// TODO 7.1 If frontier queue contains elements
	// pop the last one and calculate its 4 neighbors
	iPoint curr;
	if (frontier.Pop(curr)) //Put actual frontier into curr, while it checks if there are frontiers left
	{
		iPoint neighbours[4]; //Create the 4 neighbours every tile has (N E S W)
		neighbours[0].create(curr.x + 1, curr.y + 0); // E
		neighbours[1].create(curr.x + 0, curr.y + 1); // S
		neighbours[2].create(curr.x - 1, curr.y + 0); // W
		neighbours[3].create(curr.x + 0, curr.y - 1); // N

		for (uint i = 0; i < 4; ++i)
		{
			// TODO 7.2: For each neighbor, if not visited, add it
			// to the frontier queue and visited list
			if (visited.find(neighbours[i]) == -1 && IsWalkable(neighbours[i].x, neighbours[i].y)) //Checks for visited tiles (if visited they don't go in), use .find to find a neighbour in the list
			{

				frontier.Push(neighbours[i]);	//Add them as a frontier
				visited.add(neighbours[i]);		//Add the neighbour that you just visited

												// TODO 7.Homework, breadcrumbs are added everytime that it comes from somewhere
				breadcrumbs.add(curr);
			}
		}
	}

	return curr;
}

iPoint j1Pathfinding::PropagateDijkstra() {

	// TODO 7.1 If frontier queue contains elements
	// pop the last one and calculate its 4 neighbors
	iPoint curr;
	if (pfrontier.Pop(curr)) //Put actual frontier into curr, while it checks if there are frontiers left
	{
		iPoint neighbours[4]; //Create the 4 neighbours every tile has (N E S W)
		neighbours[0].create(curr.x + 1, curr.y + 0); // E
		neighbours[1].create(curr.x + 0, curr.y + 1); // S
		neighbours[2].create(curr.x - 1, curr.y + 0); // W
		neighbours[3].create(curr.x + 0, curr.y - 1); // N

		for (uint i = 0; i < 4; ++i)
		{

			// TODO 7.2: For each neighbor, if not visited, add it
			// to the frontier queue and visited list
			if (visited.find(neighbours[i]) == -1 && MovementCost(neighbours[i].x, neighbours[i].y) != -1) //Checks for visited tiles (if visited they don't go in), use .find to find a neighbour in the list
			{
				int new_cost = cost_so_far[curr.x][curr.y] + MovementCost(neighbours[i].x, neighbours[i].y);

				if (cost_so_far[neighbours[i].x][neighbours[i].y] == 0 || new_cost <= cost_so_far[neighbours[i].x][neighbours[i].y]) {
					cost_so_far[neighbours[i].x][neighbours[i].y] = new_cost;
					pfrontier.Push(neighbours[i], new_cost);	//Add them as a frontier
					visited.add(neighbours[i]);		//Add the neighbour that you just visited

													// TODO 7.Homework, breadcrumbs are added everytime that it comes from somewhere
					breadcrumbs.add(curr);
				}
			}
		}
	}

	return curr;
}

void j1Pathfinding::PropagateAStar(const iPoint& pos) {

	ResetNav();

	if (MovementCost(pos.x, pos.y) != -1) {
		
		iPoint curr;
	
		while (curr != pos) {
			if (pfrontier.Pop(curr)) //Put actual frontier into curr, while it checks if there are frontiers left
			{
				iPoint neighbours[4]; //Create the 4 neighbours every tile has (N E S W)
				neighbours[0].create(curr.x + 1, curr.y + 0); // E
				neighbours[1].create(curr.x + 0, curr.y + 1); // S
				neighbours[2].create(curr.x - 1, curr.y + 0); // W
				neighbours[3].create(curr.x + 0, curr.y - 1); // N

				for (uint i = 0; i < 4; ++i)
				{

					int new_cost = cost_so_far[curr.x][curr.y] + MovementCost(neighbours[i].x, neighbours[i].y) + neighbours[i].DistanceNoSqrt(pos);

					// TODO 7.2: For each neighbor, if not visited, add it
					// to the frontier queue and visited list
					if (MovementCost(neighbours[i].x, neighbours[i].y) != -1 && (cost_so_far[neighbours[i].x][neighbours[i].y] == 0 || new_cost <= cost_so_far[neighbours[i].x][neighbours[i].y])) //Checks for visited tiles (if visited they don't go in), use .find to find a neighbour in the list
					{
						
						pfrontier.Push(neighbours[i], new_cost);	//Add them as a frontier
						breadcrumbs.add(curr);
						// Calculate last distance and new distance

						cost_so_far[neighbours[i].x][neighbours[i].y] = new_cost;
						
					}
				}
			}

		}

		CreatePath(pos);
	}

}

// Cutre Version for bfs
bool j1Pathfinding::IsWalkable(int x, int y) const {
	bool ret = true;

	ret = (x >= 0 && x < App->map->Maps->width &&
		y >= 0 && y < App->map->Maps->height);

	if (ret == true) {
		p2List_item<layer_info*>* item = App->map->Maps->layers.start;
		while (item->data->draw_mode != 2)
			item = item->next;

		ret = (item->data->data[y * App->map->Maps->width + x] == 0);
	}

	return ret;
}

int	 j1Pathfinding::MovementCost(int x, int y) const {

	int ret = -1;

	if (x >= 0 && x < App->map->Maps->width && y >= 0 && y < App->map->Maps->height)
	{
		int id = App->map->Maps->layers.start->next->data->Get(x, y);

		if (id == 0)
			ret = 3;
		else
			ret = -1;
	}

	return ret;
}



// Settings makers
void j1Pathfinding::ResetNav() {
	pfrontier.Clear();
	frontier.Clear();
	visited.clear();
	breadcrumbs.clear();
	path.clear();

	for (int i = 0; i < COST_MAP; i++)
		for (int j = 0; j < COST_MAP; j++)
			cost_so_far[i][j] = 0;

	pfrontier.Push(start, 0);
	frontier.Push(start);
	visited.add(start);
	breadcrumbs.add(start);
	path.add(start);
}

void j1Pathfinding::SetStart(const iPoint& pos) {
	if (IsWalkable(pos.x, pos.y))
		start = pos;
}

// Full Propagation
void j1Pathfinding::PropagateToBFS(const iPoint& pos) {
	ResetNav();
	if (IsWalkable(pos.x, pos.y))
	{
		// PropagateBFS until you reach the goal
		iPoint test = start;
		while (pos != test) {
			test = PropagateBFS();
		}

		CreatePath(pos);

	}
}

void j1Pathfinding::PropagateToDijkstra(const iPoint& pos) {
	ResetNav();
	if (MovementCost(pos.x, pos.y) != -1)
	{
		// PropagateBFS until you reach the goal
		iPoint test = start;
		while (pos != test) {
			test = PropagateDijkstra();
		}

		CreatePath(pos);

	}
}

void j1Pathfinding::CreatePath(const iPoint& pos) {

	// Create Path to goal
	path.clear();
	iPoint goal(pos.x, pos.y );
	if (visited.find(goal) != -1) {
		while (goal != start) {
			path.add(goal);
			goal = breadcrumbs[visited.find(goal)];
		}

		path.add(goal);
	}
}

int j1Pathfinding::CreateFPath(const iPoint& origin, const iPoint& destination) {
	int ret = 0;
	/*
	if ()
	// Create Path to goal
	path.clear();
	iPoint goal(pos.x, pos.y);
	iPoint f(0, 0);
	if (visited.find(goal) != -1) {
		while (goal != start) {
			path.add(goal);

			

			goal = breadcrumbs[visited.find(goal)];
		}

		path.add(goal);
	}

	return ret;
	*/
}