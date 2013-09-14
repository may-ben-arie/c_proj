#ifndef GRAPH_H_
#define GRAPH_H_

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

/* Variables */
Graph *graph;

/* Functions */
void free_graph();
int create_graph(FILE * networkFile, double C);
double get_edge_weight_by_vertices(int vertexID1, int vertexID2);
int add_vertex_to_graph(char *vertexName);
int add_neighbor_to_vertex(Vertex *vertex, int newVertexNeighborID, double edgeWeight);
int verify_add_edge_command(char *param1, char *param2,char *param3, char *param4);
int verify_add_vertex_command(char *param1, char *param2);
int initialize_graph(double C);
void delete_edge(int vertexID1, int vertexID2);


Neighbor * GetVertexNeighbors(Graph * graph, int vertexID);
Neighbor * GetVertexOriginals(Graph * graph, int vertexID);
Vertex * GetVertex(Graph * graph, int vertexID);
double GetEdgeWeightByOriginals(Graph * graph, int vertexID1, int vertexID2);

int IsNeighborByVertex(Graph * graph, int vertexID1, int vertexID2);


#endif
