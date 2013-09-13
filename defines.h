/*
 * defines.h
 *
 *  Created on: Sep 9, 2013
 *      Author: mbenarie
 */

#ifndef DEFINES_H_
#define DEFINES_H_

typedef struct Edge {
	int vertexID1;
	int vertexID2;
	double weight;
} Edge;

typedef struct Vertex {
	char vertexName[500];
	int vertexID;
	struct Neighbor * neighbors;
	struct Neighbor * originals;
	struct Vertex * next;
	int clusterID;
} Vertex;

typedef struct Neighbor {
    int vertexID;
    double weight;
    struct Neighbor * next;
} Neighbor;

typedef struct Graph {
	Vertex * vertices;
	int vertexCounter;
	int edgeCounter;
	int edgeOriginal;
	double C;

} Graph;

typedef struct Cluster {
	int ID;
	int oldID;
	int num_of_ver;
	double score;
	int diameter;
} Cluster;

typedef struct Results {
	double score;
	double avg_weight_between_clusters;
	double avg_weight_within_clusters;
	Cluster *clusters;
	int num_of_clusters;
} Results;

#endif /* DEFINES_H_ */
