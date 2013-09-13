#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "graph.h"
#include "graph_to_cplex.h"
#include "cplex_to_clusters.h"
#include "output_files.h"

#define LINE_LENGTH 500
#define PATH_LENGTH 100

void VerifyProjectArguments(int argc, char *argv[]) {

	if(argc != 4) {
		printf("Error: Wrong Number of parameters.\n");
		printf("Missing input folder, output folder or C value.\n");
		exit(1);
	}

	if(argv[1][strlen(argv[1])-1] != '/') {
		printf("Error: Missing slash in input folder parameter.\n");
		exit(1);
	}

	if(argv[2][strlen(argv[2])-1] != '/') {
		printf("Error: Missing slash in output folder parameter.\n");
		exit(1);
	}

	if (atof(argv[3]) < 0 || atof(argv[3]) > 1) {
		printf("Error: C parameter must be between 0 and 1 (inclusive)\n");
		exit(1);
	}
}

void OpenAndVerifyFolder(char* folder) {
	DIR *directory;

	directory = opendir(folder);

	if(directory == NULL) {
		printf("Error with Input or Output folder.\n");
	}

	if(errno == EACCES) {
		printf("No Permission for Input or Output folder.\n");
		exit(1);
	}

	if(errno == ENAMETOOLONG) {
		printf("Too long path for Input or Output folder.\n");
		exit(1);
	}

	if(errno == ENOENT) {
		printf("Input or Output folder does not exist.\n");
		exit(1);
	}

	if(errno == ENOTDIR) {
		printf("Input or Output exists but is not a folder.\n");
		exit(1);
	}

	closedir(directory);

}

void VerifyFile(FILE * file, char * fileName) {

	if(file == NULL) {
		printf("%s %s %s","Error with", fileName, "file.\n");
	}

	if(errno == EACCES) {
		printf("%s %s %s", "No permission for", fileName, "file.\n");
		exit(1);
	}

	if(errno == ENOENT && !strcmp(fileName,"network")) {
		printf("network file does not exist.\n");
		exit(1);
	}
}

void InitializeGraph(Graph * graph, double C) {

	graph->vertices = NULL;
	graph->vertexCounter = 0;
	graph->edgeCounter = 0;
	graph->edgeOriginal = 0;
	graph->C = C;

}

void ReadNetworkFile(FILE * networkFile, Graph * graph) {

	int lineNumber = 0;
	char line[LINE_LENGTH];
	char command[LINE_LENGTH];
	char param1[LINE_LENGTH], param2[LINE_LENGTH], param3[LINE_LENGTH];

    while(fgets(line, sizeof line, networkFile) != NULL ) {
    	sscanf(line,"%s %s %s %s",command, param1, param2, param3);
    	lineNumber++;
    	if (!strcmp(command, "add_vertex")) {
    		if (VerifyNewVertexToGraph(graph,param1,param2,param3)) {
    			AddVertexToGraph(graph,param1);
    		}
    	}
    	else if (!strcmp(command, "add_edge")) {
    		if (VerifyNewNeighborToGraph(graph,param1,param2,param3)) {
    			sprintf(param3,"%.3f",atof(param3));
    			graph->edgeCounter = graph->edgeCounter + 1;
    			graph->edgeOriginal = graph->edgeOriginal + 1;
    			AddNeighborToVertex(GetVertexNeighbors(graph, atoi(param1)), atoi(param2), atof(param3));
				AddNeighborToVertex(GetVertexNeighbors(graph, atoi(param2)), atoi(param1), atof(param3));

				AddNeighborToVertex(GetVertexOriginals(graph, atoi(param1)), atoi(param2), atof(param3));
				AddNeighborToVertex(GetVertexOriginals(graph, atoi(param2)), atoi(param1), atof(param3));
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
}

void PrintGraph(Graph * graph) {
	Vertex * head = graph->vertices;
	Neighbor * neighbors;
	while(head != NULL) {
		printf("%s %d %s %s\n","Vertex ID:", head->vertexID, ", Vertex Name:", head->vertexName);
		neighbors = GetVertexNeighbors(graph,head->vertexID);
		neighbors = neighbors->next;
		while(neighbors != NULL) {
			printf("%s %d %s %d %s %f\n","There is an edge from Vertex:", head->vertexID, " to Vertex:",
					neighbors->vertexID, " with weight:", neighbors->weight);
			neighbors = neighbors->next;
		}
		head = head->next;
	}
}

/* Main function of the project
	@Params:
	i. Input folder (relative path + slash) - argv[1]
	ii. Output folder (relative path + slash) - argv[2]
	iii. C (minus C will be assigned as weight for all ghost edges) - argv[3]
*/
int main(int argc, char *argv[]) {

	char networkFilePath[PATH_LENGTH];
	char resultsFilePath[PATH_LENGTH];
	FILE * networkFile;
	FILE * resultsFile;
	FILE * clusteringSolutionFile;
	FILE * bestClustersFile;
	double C;

	VerifyProjectArguments(argc, argv);

	C = atof(argv[3]);

	OpenAndVerifyFolder(argv[1]);

	OpenAndVerifyFolder(argv[2]);

	strcpy(networkFilePath,argv[1]);
	strcat(networkFilePath,"network");

	networkFile = fopen(networkFilePath,"r");
	VerifyFile(networkFile, "network");

	strcpy(resultsFilePath,argv[2]);
	strcat(resultsFilePath,"results");

	resultsFile = fopen(resultsFilePath,"w");
	VerifyFile(resultsFile,"results");

	Graph *graph = (Graph*)malloc(1 * sizeof(Graph));
	if (graph == NULL) {
			perror("Error: failed allocating memory for the graph.\n");
			exit(1);
		}

	InitializeGraph(graph, C);

	ReadNetworkFile(networkFile, graph);

	//for debugging only
	PrintGraph(graph);

	if (graph_to_cplex(graph,argv[2],C)){
		return 1;
	}

	if (cluster()){
		free_cplex_data();
		return 1;
	}

	get_results(graph);

	WriteResultsFile(resultsFile,graph);

	strcpy(resultsFilePath,argv[2]);
	strcat(resultsFilePath,"clustering_solution.xgmml");
	clusteringSolutionFile = fopen(resultsFilePath,"w");
	VerifyFile(clusteringSolutionFile,"clustering_solution.xgmml");

	strcpy(resultsFilePath,argv[2]);
	strcat(resultsFilePath,"best_clusters.xgmml");
	bestClustersFile = fopen(resultsFilePath,"w");
	VerifyFile(bestClustersFile,"best_clusters.xgmml");

	WriteXGMMLFiles(clusteringSolutionFile, bestClustersFile, graph);

	free_cplex_data();

	fclose(networkFile);
	fclose(resultsFile);
	fclose(clusteringSolutionFile);
	fclose(bestClustersFile);


	return 0;
}

