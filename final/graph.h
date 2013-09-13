#ifndef GRAPH_H_
#define GRAPH_H_

#include <stdio.h>
#include <stdlib.h>
#include "defines.h"

void AddNeighborToVertex(Neighbor *head, int newVertexNeighborID, double edgeWeight);
void AddVertexToGraph(Graph * graph, char * vertexName);
Neighbor * GetVertexNeighbors(Graph * graph, int vertexID);
Neighbor * GetVertexOriginals(Graph * graph, int vertexID);
Vertex * GetVertex(Graph * graph, int vertexID);
double GetEdgeWeightByVertices(Graph * graph, int vertexID1, int vertexID2);
double GetEdgeWeightByOriginals(Graph * graph, int vertexID1, int vertexID2);
int VerifyNewVertexToGraph(Graph* graph, char *param1, char *param2,char *param3);
int VerifyNewNeighborToGraph(Graph* graph, char *param1, char *param2,char *param3);
void DeleteEdgeFromGraph(Graph * graph, int vertexID1, int vertexID2);
int IsNeighborByVertex(Graph * graph, int vertexID1, int vertexID2);



#endif
