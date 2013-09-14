#ifndef CPLEX_TO_CLUSTERS_H_
#define CPLEX_TO_CLUSTERS_H_

/* Includes */
#include <math.h>
#include "cluster_editing.h"
#include "graph.h"
#include "graph_to_cplex.h"
#include "defines.h"

/* Variables */
Results *results;

/* Functions */
int get_results();
void free_results();

void set_score();
int set_clusters();
void set_unsorted_clusters_id(int *vertex_clusters_id);
void create_clusters_array(int *vertex_clusters_id);
void sort_clusters(int *vertex_clusters_id);
int comp(const void* obj1,const void* obj2);
int new_ID_for_old_ID(int old_id);
void remove_edges();
void set_avg_weight(int *vertex_clusters_id);
void set_clusters_diameter();
int get_vertex_diameter(int vertex_ID);
void print_clusters();
void TwoVerticesOneEdgeCase();

#endif
