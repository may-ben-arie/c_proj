#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"

int add_vertex_to_graph(char * vertexName) {
	Vertex *temp,*right;
	temp = malloc(sizeof(Vertex));
	if(temp == NULL){
	   printf("Error: Unable to allocate memory for a new vertex.\n");
	  return 1;
	}
	temp->next = NULL;
	temp->neighbors = NULL;
	temp->originals = NULL;

	temp->vertexID = graph->vertexCounter;
	graph->vertexCounter = graph->vertexCounter + 1;
	strcpy(temp->vertexName, vertexName);
	if(graph->vertices == NULL) {
		graph->vertices = temp;
	}
	else {
		right = graph->vertices;
		while(right->next != NULL) {
			right = right->next;
		}
		right->next = temp;
		right = temp;
		right->next = NULL;
	}
	return 0;
}

int add_neighbor_to_vertex(Vertex *vertex, int newVertexNeighborID, double edgeWeight)
{
    Neighbor *temp,*temp2,*right;
    if (edgeWeight == 0) {
    	printf("Error: edge weight must be positive.\n");
    	return 0;
    }

    temp = malloc(sizeof(Neighbor));
    temp2 = malloc(sizeof(Neighbor));
    if(temp == NULL || temp2 == NULL) {
    	printf("Error: Unable to allocate memory for vertex neighbor.\n");
    	return 1;
    }

    temp->next = NULL;
    temp->vertexID = newVertexNeighborID;
    temp->weight = edgeWeight;
    temp2->next = NULL;
    temp2->vertexID = newVertexNeighborID;
    temp2->weight = edgeWeight;

    if (vertex->neighbors == NULL){
    	vertex->neighbors = temp;
    	vertex->originals = temp2;
    	return 0;
    }

    right = vertex->neighbors;
    while(right->next != NULL) {
    	right = right->next;
    }
    right->next = temp;
    right = temp;
    right->next = NULL;

    right = vertex->originals;
    while(right->next != NULL) {
       	right = right->next;
       }
    right->next = temp2;
    right = temp2;
    right->next = NULL;
    return 0;
}


Neighbor * GetVertexNeighbors(Graph * graph, int vertexID) {
	Vertex * vertex = graph->vertices;
	while(vertex != NULL) {
		if(vertex->vertexID == vertexID) {
			return (vertex->neighbors);
		}
		vertex = vertex->next;
	}
	return NULL;
}

Neighbor * GetVertexOriginals(Graph * graph, int vertexID) {
	Vertex * vertex = graph->vertices;
	while(vertex != NULL) {
		if(vertex->vertexID == vertexID) {
			return (vertex->originals);
		}
		vertex = vertex->next;
	}
	return NULL;
}

Vertex * GetVertex(Graph * graph, int vertexID) {
	Vertex * vertex = graph->vertices;
	while(vertex != NULL) {
		if(vertex->vertexID == vertexID) {
			return vertex;
		}
		vertex = vertex->next;
	}
	return NULL;
}

double get_edge_weight_by_vertices(int vertexID1, int vertexID2) {
	Neighbor * neighbors = GetVertexNeighbors(graph, vertexID1);
	while(neighbors != NULL) {
		if(neighbors->vertexID == vertexID2) {
			return neighbors->weight;
		}
		neighbors = neighbors->next;
	}
	return -1.0;
}

double GetEdgeWeightByOriginals(Graph * graph, int vertexID1, int vertexID2) {
	Neighbor * neighbors = GetVertexOriginals(graph, vertexID1);
	while(neighbors != NULL) {
		if(neighbors->vertexID == vertexID2) {
			return neighbors->weight;
		}
		neighbors = neighbors->next;
	}
	return -1.0;
}


void DeleteNeighborByVertex(int vertexID, int neighborID) {
	Vertex *vertex = GetVertex(graph, vertexID);
	Neighbor * temp = vertex->neighbors;
	Neighbor * prev = NULL;
	while(temp != NULL) {
		if(temp->vertexID == neighborID) {
			if(temp == vertex->neighbors) {
				vertex->neighbors = temp->next;
				free(temp);
				return;
			}
			else {
				prev->next = temp->next;
				free(temp);
				return;
			}
		}
		prev = temp;
		temp = temp->next;

	}
}

int IsNeighborByVertex(Graph * graph, int vertexID1, int vertexID2) {
	Neighbor * neighbors = GetVertexNeighbors(graph, vertexID1);
	while(neighbors != NULL) {
		if(neighbors->vertexID == vertexID2) return 1;
		neighbors = neighbors->next;
	}
	return 0;
}

void delete_edge(int vertexID1, int vertexID2) {
	if(IsNeighborByVertex(graph, vertexID1, vertexID2) && IsNeighborByVertex(graph, vertexID2, vertexID1)) {
		DeleteNeighborByVertex(vertexID1,vertexID2);
		DeleteNeighborByVertex(vertexID2,vertexID1);
		graph->edgeCounter--;
	}
}


int verify_add_vertex_command(char *param1, char *param2) {
	Vertex * vertex = graph->vertices;
	if (!strcmp(param1,"\0")) {
		printf("%s","Error: The command \"add_vertex\" has no parameters.\n");
		return 1;
	}
	if (strcmp(param2,"\0")) {
		printf("%s %s %s","Error: The command \"add_vertex\" has more than 1 parameter. First Parameter:",param1, "\n");
		return 1;
	}
	while(vertex != NULL) {
		if(!strcmp(vertex->vertexName,param1)) {
			printf("%s %s %s","Error: A vertex named",param1,"already exists.\n");
			return 1;
		}
		vertex = vertex->next;
	}
	return 0;
}

int isNumeric (const char * s)
{
	char * p;
	if (s == NULL || *s == '\0' || isspace(*s)) {
      return 0;
	}
    strtod (s, &p);
    return *p == '\0';
}

int verify_add_edge_command(char *param1, char *param2,char *param3, char *param4) {
	int vertexID1, vertexID2;
	char weight[500];
	double realWeight;

	if (strcmp(param4,"\0")){
		printf("%s","Error: The command \"add_edge\" has more than 3 parameters.\n");
		return 1;
	}
	if (!strcmp(param1,"\0") || !strcmp(param2,"\0") || !strcmp(param3,"\0")) {
		printf("%s","Error: The command \"add_edge\" has less than 3 parameters.\n");
		return 1;
	}
	if(!isNumeric(param1) || !isNumeric(param2) || !isNumeric(param3)) {
		printf("%s","Error: At least on of the parameters of the command \"add_edge\" is not numeric.\n");
		return 1;
	}
	vertexID1 = atoi(param1);
	vertexID2 = atoi(param2);
	sprintf(weight,"%.3f",atof(param3));
	realWeight = atof(weight);
	if(vertexID1 < 0 || vertexID2 < 0) {
		printf("%s","Error: There is an edge with a negative ID for vertex.\n");
		return 1;
	}
	if(realWeight < 0) {
		printf("%s","Error: There is an edge with a negative weight.\n");
		return 1;
	}
	if(realWeight == 0) {
		printf("%s","Error: There is an edge with a zero weight.\n");
		return 1;
	}
	if(realWeight > 1) {
		printf("%s","Error: There is an edge with a weight greater than 1.\n");
		return 1;
	}
	if(vertexID1 == vertexID2) {
		printf("%s %d%s","Error: There is a self loop of vertex", vertexID1,".\n");
		return 1;
	}
	if(vertexID1 >= graph->vertexCounter) {
		printf("%s%d,%d%s%d%s","Error: Trying to add an edge (",vertexID1,vertexID2,") where vertex ",vertexID1," does not exists.\n");
		return 1;
	}
	if(vertexID2 >= graph->vertexCounter) {
		printf("%s%d,%d%s%d%s","Error: Trying to add an edge (",vertexID1,vertexID2,") where vertex ",vertexID2," does not exists.\n");
		return 1;
	}
	if(get_edge_weight_by_vertices(vertexID1,vertexID2) > 0) {
		printf("%s%d,%d%s","Error: Edge (",vertexID1,vertexID2,") already exists.\n");
		return 1;
	}
	return 0;
}

int create_graph(FILE * networkFile, double C) {
	int lineNumber = 0;
	char line[LINE_LENGTH];
	char command[LINE_LENGTH];
	char param1[LINE_LENGTH], param2[LINE_LENGTH], param3[LINE_LENGTH] ,param4[2];

	if (initialize_graph(C)){
		return 1;
	}

	while(fgets(line, sizeof(line), networkFile) != NULL) {
		param1[0] = '\0';param2[0] = '\0';param3[0] = '\0';param4[0] = '\0';
		sscanf(line,"%s %s %s %s %s",command, param1, param2, param3, param4);

		lineNumber++;

		if (!strcmp(command, "add_vertex")) {
			if (verify_add_vertex_command(param1,param2)) {
				continue;
			}
			if (add_vertex_to_graph(param1)){
				return 1;
			}
		}

		else if (!strcmp(command, "add_edge")) {
			if (verify_add_edge_command(param1,param2,param3,param4)) {
				continue;
			}
			sprintf(param3,"%.3f",atof(param3));
			graph->edgeCounter = graph->edgeCounter + 1;
			graph->edgeOriginal = graph->edgeOriginal + 1;

			if (add_neighbor_to_vertex(GetVertex(graph, atoi(param1)), atoi(param2), atof(param3))){
				return 1;
			}

			if (add_neighbor_to_vertex(GetVertex(graph, atoi(param2)), atoi(param1), atof(param3))){
				return 1;
			}
		}
		else {
			printf("%s %d %s","Error: Wrong command on line", lineNumber, "of the network file.\n");
		}
		strcpy(command,"");
		strcpy(param1,"");
		strcpy(param2,"");
		strcpy(param3,"");
	}

	return 0;
}

int initialize_graph(double C) {
	graph = calloc(1, sizeof(Graph));
	if (graph == NULL) {
		perror("Error: failed allocating memory for the graph.\n");
		return 1;
	}
	graph->vertices = NULL;
	graph->vertexCounter = 0;
	graph->edgeCounter = 0;
	graph->edgeOriginal = 0;
	graph->C = C;
	return 0;
}

void free_graph(){
	Vertex *next_ver,*temp_ver;
	Neighbor *next_edge, *temp_edge;
	if (graph == NULL){
		return;
	}

	next_ver = graph->vertices;
	while (next_ver != NULL){

		next_edge = next_ver->neighbors;
		while (next_edge != NULL){
			temp_edge = next_edge;
			next_edge = next_edge->next;
			free(temp_edge);
		}

		next_edge = next_ver->originals;
		while (next_edge != NULL){
			temp_edge = next_edge;
			next_edge = next_edge->next;
			free(temp_edge);
		}

		temp_ver = next_ver;
		next_ver = next_ver->next;
		free(temp_ver);
	}

	free(graph);
}
