/**
 * @file FixedGraph.h
 * @author Davide Scalisi, Luigi Seminara
 * @brief Implementation of the fixed graph algorithm
 * 				using Dijkstra as shortest path algorithm.
 * 
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef FIXED_GRAPH_H
#define FIXED_GRAPH_H

#include <Arduino.h>

#include <map>
#include <iterator>
#include <limits.h>
#include <math.h>

//Bottom-right point: (1.5, 1.5).
#define FIXED_GRAPH_WIDTH	16
#define FIXED_GRAPH_SIZE	(FIXED_GRAPH_WIDTH * FIXED_GRAPH_WIDTH)

#define FIXED_GRAPH_STEP	0.1
#define FIXED_GRAPH_TOL		100

//Border x and y.
#define FIXED_GRAPH_LIMIT	((FIXED_GRAPH_WIDTH * FIXED_GRAPH_STEP) - FIXED_GRAPH_STEP)

class FixedGraph{
	private:
		std::map<std::pair<float, float>, int> points;
		std::map<int, std::pair<float, float>> vertex;

		uint8_t adj_matrix[FIXED_GRAPH_SIZE][FIXED_GRAPH_SIZE] = {0};

		bool parents_ready = false;			//Shortest path ready after dijkstra.
		int parents[FIXED_GRAPH_SIZE];	//Shortest path array.

		/**
		 * @brief Change the value of an edge's graph
		 * 
		 * @param x change x
		 * @param y change y
		 * @param node current node of the graph
		 * @param val new value of the edge
		 */
		void __addEdge(float x, float y, int node, int val);

		/**
		 * @brief Change the value of an edge's graph
		 * 
		 * @param x change x
		 * @param y change y
		 * @param val set = 1, clear = 0
		 */
		void __updateAdjVertex(float x, float y, int val);

		/**
		 * @brief A utility function to find the vertex with minimum 
		 *        distance value, from the set of vertices not yet 
		 *        included in shortest path tree
		 * 
		 * @param dist 
		 * @param set 
		 * @return int 
		 */
		int __minDistance(int dist[], bool set[]);

		/**
		 * @brief Convert an arbitrary (x, y) point to the
		 * 				nearest point in the fixed graph
		 * 
		 * @param x 
		 * @param y 
		 */
		std::pair<float, float> __roundPointCoordinates(float x, float y);

	public:
		/**
		 * @brief Construct a new Graph object
		 * 
		 */
		FixedGraph();

		/**
		 * @brief Set the edges weights of a node
		 *  
		 * @param x change x
		 * @param y change y
		 */
		void setVertex(float x, float y)		{	__updateAdjVertex(x, y, 1);	}

		/**
		 * @brief Clear the edges weights of a node
		 *  
		 * @param x change x
		 * @param y change y
		 */
		void clearVertex(float x, float y)	{	__updateAdjVertex(x, y, 0);	}

		/**
		 * @brief Shortest path from current position to all vertex
		 * 
		 * @param x source x
		 * @param y source y
		 */
		bool dijkstra(float x, float y);

		/**
		 * @brief Function to retrive the shortest path
		 * 				from the firstly specified source (dijkstra)
		 * 				to the current specified destination (x, y)
		 * 
		 * @param x destination x
		 * @param y destination y
		 * @param shortest_path destination shortest path array
		 * @param shortest_path_len shortest_path length
		 */
		bool getShortestPath(float x, float y, std::pair<float, float> *shortest_path, int &shortest_path_len);
};

#endif
