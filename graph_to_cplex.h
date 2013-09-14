/*
 * graph_to_cplex.h
 *
 */

#ifndef GRAPH_TO_CPLEX_H_
#define GRAPH_TO_CPLEX_H_

/* Includes */
#include "cluster_editing.h"
#include "graph.h"
#include "ilcplex/cplex.h"
#include "defines.h"

/* Variables */
Edge *edge_arr;

/* Functions - public*/
int graph_to_cplex(Graph *graph, char *output_path, double ghost_value);
void free_cplex_data();

/* Functions - private*/
int set_probname(char* output_path);
int set_numcols_numrows(Graph *graph);
int set_objsen_obj();
int set_edge_arr(Graph *graph, double ghost_value);
int set_sense_rhs();
int set_matbeg_matcnt_matind_matval(Graph *graph);
int set_lb_ub();
int set_cnt_indices_ctype();
int get_edge_id(int ver1,int ver2);

/* Functions - debug only*/
void print_arr_int(int *matind, int size);
void print_arr_double(double *matind, int size);

#endif /* GRAPH_TO_CPLEX_H_ */
