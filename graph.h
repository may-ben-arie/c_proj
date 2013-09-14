#ifndef GRAPH_H_
#define GRAPH_H_

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

/* Variables */
Graph *graph;

/* Functions - public */
void free_graph();
int create_graph(FILE * networkFile, double C);

/* Functions - private */
int add_vertex_to_graph(char * vertexName);
int add_neighbor_to_vertex(Neighbor *neighbors, int newVertexNeighborID, double edgeWeight);
int verify_add_edge_command(char *param1, char *param2,char *param3, char *param4);
int verify_add_vertex_command(char *param1, char *param2);
int initialize_graph(double C);

Neighbor * GetVertexNeighbors(Graph * graph, int vertexID);
Neighbor * GetVertexOriginals(Graph * graph, int vertexID);
Vertex * GetVertex(Graph * graph, int vertexID);
double GetEdgeWeightByVertices(Graph * graph, int vertexID1, int vertexID2);
double GetEdgeWeightByOriginals(Graph * graph, int vertexID1, int vertexID2);
void DeleteEdgeFromGraph(Graph * graph, int vertexID1, int vertexID2);
int IsNeighborByVertex(Graph * graph, int vertexID1, int vertexID2);



#endif
