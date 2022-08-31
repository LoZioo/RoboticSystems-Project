#include <FixedGraph.h>

void FixedGraph::__addEdge(float x, float y, int node, int val){
	if(points.find(std::pair<float, float>(x, y)) != points.end()){
		adj_matrix[node][points[std::pair<float, float>(x, y)]] = val;
		adj_matrix[points[std::pair<float, float>(x, y)]][node] = val;
	}
}

void FixedGraph::__updateAdjVertex(float x, float y, int val){
	int node = -1;

	if(points.find(std::pair<float, float>(x, y)) != points.end())
		node = points[std::pair<float, float>(x, y)];
	
	else return;

	/**
	 * @brief Set left edge
	 * 
	 * + + +
	 * x - +
	 * + + +
	 */
	if(x - FIXED_GRAPH_STEP >= 0) __addEdge(x - FIXED_GRAPH_STEP, y, node, val);

	/**
	 * @brief Set right edge
	 * 
	 * + + +
	 * + - x
	 * + + +
	 */
	if(x + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT) __addEdge(x + FIXED_GRAPH_STEP, y, node, val);

	/**
	 * @brief Set top edge
	 * 
	 * + x +
	 * + - +
	 * + + +
	 */
	if(y - FIXED_GRAPH_STEP >= 0) __addEdge(x, y - FIXED_GRAPH_STEP, node, val);

	/**
	 * @brief Set bottom edge
	 * 
	 * + + +
	 * + - +
	 * + x +
	 */
	if(y + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT) __addEdge(x, y + FIXED_GRAPH_STEP, node, val);

	/**
	 * @brief Set bottom-left edge
	 * 
	 * + + +
	 * + - +
	 * x + +
	 */
	if(y + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT && x - FIXED_GRAPH_STEP >= 0) __addEdge(x - FIXED_GRAPH_STEP, y + FIXED_GRAPH_STEP, node, val);

	/**
	 * @brief Set bottom-right edge
	 * 
	 * + + +
	 * + - +
	 * + + x
	 */
	if(y + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT && x + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT) __addEdge(x + FIXED_GRAPH_STEP, y + FIXED_GRAPH_STEP, node, val);

	/**
	 * @brief Set top-left edge
	 * 
	 * x + +
	 * + - +
	 * + + +
	 */
	if(y - FIXED_GRAPH_STEP >= 0 && x - FIXED_GRAPH_STEP >= 0) __addEdge(x - FIXED_GRAPH_STEP, y - FIXED_GRAPH_STEP, node, val);

	/**
	 * @brief Set top-right edge
	 * 
	 * + + x
	 * + - +
	 * + + +
	 */
	if(y - FIXED_GRAPH_STEP >= 0 && x + FIXED_GRAPH_STEP <= FIXED_GRAPH_LIMIT) __addEdge(x + FIXED_GRAPH_STEP, y - FIXED_GRAPH_STEP, node, val);
}

int FixedGraph::__minDistance(int dist[], bool set[]){
	int min = INT_MAX, min_index;

	for(int v=0; v<FIXED_GRAPH_SIZE; v++)
		if(set[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;
	
	return min_index;
}

std::pair<float, float> FixedGraph::__roundPointCoordinates(float x, float y){
	x *= FIXED_GRAPH_TOL;
	y *= FIXED_GRAPH_TOL;

	x /= (FIXED_GRAPH_STEP * FIXED_GRAPH_TOL);
	y /= (FIXED_GRAPH_STEP * FIXED_GRAPH_TOL);

	x = round(x) * FIXED_GRAPH_STEP;
	y = round(y) * FIXED_GRAPH_STEP;

	x = constrain(x, 0, FIXED_GRAPH_LIMIT);
	y = constrain(y, 0, FIXED_GRAPH_LIMIT);

	return std::pair<float, float>(x, y);
}

FixedGraph::FixedGraph(){
	float x = 0; 
	float y = 0;
	int count = 0;

	for(int i=0; i<10; i++){
		y = 0;

		for(int j=0; j<10; j++){
			points.insert(std::pair<std::pair<float, float>, int>(std::pair<float, float>(x, y), count));
			vertex.insert(std::pair<int, std::pair<float, float>>(count++, std::pair<float, float>(x, y)));

			y += FIXED_GRAPH_STEP;
		}

		x += FIXED_GRAPH_STEP;
	}

	std::map<std::pair<float, float>, int>::iterator itr;

	for(itr=points.begin(); itr != points.end(); ++itr) {
		float x = itr->first.first;
		float y = itr->first.second;
		
		adj_matrix[itr->second][itr->second] = 1;
		setVertex(x, y);
	}
}

bool FixedGraph::dijkstra(float x, float y){
	int src = -1;

	parents_ready = false;
	
	if(points.find(std::pair<float, float>(x, y)) != points.end())
		src = points[std::pair<float, float>(x, y)];
	
	else return false;

	/**
	 * @brief dist[i] will hold the shortest distance from src to i
	 * 
	 */
	int dist[FIXED_GRAPH_SIZE];

	/**
	 * @brief set[i] will be true if vertex i is included in shortest
	 *        path tree or shortest distance from src to i is finalized
	 * 
	 */
	bool set[FIXED_GRAPH_SIZE];

	//Initialize all distances as INFINITE and stpSet[] as false
	for(int i=0; i<FIXED_GRAPH_SIZE; i++){
		dist[i] = INT_MAX;
		set[i] = false;
		parents[i] = INT_MAX;
	}

	dist[src] = 0;
	parents[src] = -1;

	for(int i=0; i<FIXED_GRAPH_SIZE - 1; i++){
		int u = __minDistance(dist, set);
		set[u] = true;

		for(int v=0; v<FIXED_GRAPH_SIZE; v++)
			if(
				!set[v] && adj_matrix[u][v] == 1 &&
				dist[u] != INT_MAX &&
				dist[u] + adj_matrix[u][v] <= dist[v]
			){
				parents[v] = u;
				dist[v] = dist[u] + adj_matrix[u][v];
			}
	}

	parents_ready = true;

	return true;
}

bool FixedGraph::getShortestPath(float x, float y, std::pair<float, float> *shortest_path, int &shortest_path_len){
	if(!parents_ready)
		return false;

	std::pair<float, float> dest = __roundPointCoordinates(x, y);

	//Set the destination vertex.
	int i = points[dest];

	//Destination can't be reached.
	if(i == INT_MAX)
		return false;
	
	shortest_path_len = 0;

	//Upside-down shortest path.
	while(i != -1){
		shortest_path[shortest_path_len++] = vertex[i];

		i = parents[i];
	}

	return true;
}
