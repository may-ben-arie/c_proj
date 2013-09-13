#include <stdio.h>
#include "output_files.h"
#include "graph_to_cplex.h"
#include "cplex_to_clusters.h"
#include "defines.h"
#include "graph.h"

void WriteResultsFile(FILE * resultsFile,Graph * graph) {
	Vertex * vertex = graph->vertices;
	Vertex * temp;
	Neighbor * neighbor;
	int i;

	fprintf(resultsFile,"Cluster Editing Optimal Solution\n");
	fprintf(resultsFile,"score: %.3f\n",results->score);
	fprintf(resultsFile,"number of clusters: %d\n",results->num_of_clusters);
	fprintf(resultsFile,"cost for ghost: %.3f\n",graph->C);
	fprintf(resultsFile,"\n");
	fprintf(resultsFile,"The clustered network:\n");
	fprintf(resultsFile,"%d vertices:\n",graph->vertexCounter);
	while(vertex != NULL) {
		fprintf(resultsFile,"%s %d\n",vertex->vertexName, vertex->clusterID);
		vertex = vertex->next;
	}
	fprintf(resultsFile,"%d edges:\n",graph->edgeOriginal);

	vertex = graph->vertices;
	while(vertex != NULL) {
		neighbor = GetVertexOriginals(graph,vertex->vertexID);
		while(neighbor != NULL) {
			if(vertex->vertexID < neighbor->vertexID) {
				temp = GetVertex(graph,neighbor->vertexID);
				fprintf(resultsFile,"%s-%s %.3f\n",vertex->vertexName, temp->vertexName, neighbor->weight);
			}
			neighbor = neighbor->next;
		}
		vertex = vertex->next;
	}

	fprintf(resultsFile,"\n");
	fprintf(resultsFile,"Clustering statistics for the %d clusters:\n",results->num_of_clusters);
	fprintf(resultsFile,"Average weight of an edge within clusters: %.3f\n",results->avg_weight_within_clusters);
	fprintf(resultsFile,"Average weight of an edge between clusters: %.3f\n",results->avg_weight_between_clusters);
	for (i=0; i<results->num_of_clusters; i++) {
		fprintf(resultsFile,"Cluster %d score: %.3f diameter: %d\n",
				(results->clusters)[i].ID, (results->clusters)[i].score, (results->clusters)[i].diameter);
	}
}

void WriteIntro(FILE * file, char * label) {
	fprintf(file,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(file,"<graph label=\"%s\" directed=\"0\" ",label);
	fprintf(file,"xmlns:cy=\"http://www.cytoscape.org\" ");
	fprintf(file,"xmlns:dc=\"http://purl.org/dc/elements/1.1/\" ");
	fprintf(file,"xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" ");
	fprintf(file,"xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.cs.rpi.edu/XGMML\">");
}

void WriteNode(FILE * file, Vertex * vertex) {
	int clusterClass = vertex->clusterID-1;
	if(clusterClass > 9) clusterClass = 9;
	fprintf(file, "<node label=\"%s\" id=\"%d\">",vertex->vertexName,vertex->vertexID);
	fprintf(file, "<graphics type=\"ELLIPSE\" fill=\"%s\" width=\"5\" ",clusterColors[clusterClass]);
	fprintf(file, "cy:nodeLabel=\"%s\" cy:borderLineType=\"solid\"/></node>",vertex->vertexName);
}

void WriteEdge(FILE * file, Vertex * vertex, Vertex * vertexTo, Graph * graph) {
	fprintf(file, "<edge label=\"%s (pp) %s\" ",vertex->vertexName, vertexTo->vertexName);
	fprintf(file, "source=\"%d\" target=\"%d\">",vertex->vertexID, vertexTo->vertexID);
	fprintf(file, "<graphics cy:edgeLabel=\"weight=%.3f\"/></edge>",GetEdgeWeightByOriginals(graph, vertex->vertexID,vertexTo->vertexID));
}


void WriteXGMMLFiles(FILE * clusteringSolutionFile, FILE * bestClustersFile, Graph * graph) {
	clusterColors = (char *[]){"#0000FF", "#8A2BE2", "#006400", "#A52A2A", "#FF4500",
							   "#FF69B4", "#7FFF00", "#00FFFF", "#FFD700", "#C0C0C0"};
	Vertex * vertex = graph->vertices;
	Vertex * vertexTo;
	Neighbor * neighbor;

	WriteIntro(clusteringSolutionFile,"clustering_solution");
	WriteIntro(bestClustersFile,"best_clusters");

	while(vertex != NULL) {
		WriteNode(clusteringSolutionFile, vertex);
		if(vertex->clusterID <= 5) {
			WriteNode(bestClustersFile, vertex);
		}
		vertex = vertex->next;
	}

	vertex = graph->vertices;
	while(vertex != NULL) {
		neighbor = GetVertexOriginals(graph,vertex->vertexID);
		while(neighbor != NULL) {
			if(vertex->vertexID < neighbor->vertexID) {
				vertexTo = GetVertex(graph,neighbor->vertexID);
				WriteEdge(clusteringSolutionFile, vertex, vertexTo, graph);
				if(vertex->clusterID <= 5 && vertexTo->clusterID <= 5) {
					WriteEdge(bestClustersFile, vertex, vertexTo, graph);
					}
			}
			neighbor = neighbor->next;
		}
		vertex = vertex->next;
	}

	fprintf(clusteringSolutionFile,"</graph>");
	fprintf(bestClustersFile,"</graph>");
}

