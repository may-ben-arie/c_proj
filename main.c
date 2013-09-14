#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "graph.h"
#include "graph_to_cplex.h"
#include "cplex_to_clusters.h"
#include "output_files.h"

FILE * networkFile;
FILE * resultsFile;
FILE * clusteringSolutionFile;
FILE * bestClustersFile;

void VerifyProjectArguments(int argc, char *argv[]) {

	if(argc != 4) {
		perror("Error: Wrong Number of parameters.\n");
		exit(1);
	}

	if(argv[1][strlen(argv[1])-1] != '/') {
		perror("Error: Missing slash in input folder parameter.\n");
		exit(1);
	}

	if(argv[2][strlen(argv[2])-1] != '/') {
		perror("Error: Missing slash in output folder parameter.\n");
		exit(1);
	}

	if (atof(argv[3]) < 0 || atof(argv[3]) > 1) {
		perror("Error: C parameter must be between 0 and 1 (inclusive)\n");
		exit(1);
	}
}

void OpenAndVerifyFolder(char* folder) {
	DIR *directory;

	directory = opendir(folder);

	if(directory == NULL) {
		perror("Error: with Input or Output folder.\n");
		exit(1);
	}

	if(errno == EACCES) {
		perror("Error: No Permission for Input or Output folder.\n");
		exit(1);
	}

	if(errno == ENAMETOOLONG) {
		perror("Error: Too long path for Input or Output folder.\n");
		exit(1);
	}

	if(errno == ENOENT) {
		perror("Error: Input or Output folder does not exist.\n");
		exit(1);
	}

	if(errno == ENOTDIR) {
		perror("Error: Input or Output exists but is not a folder.\n");
		exit(1);
	}

	closedir(directory);

}

void VerifyFile(FILE * file, char * fileName) {

	if(file == NULL) {
		printf("Error: %s %s %s","Error with", fileName, "file.\n");
	}

	if(errno == EACCES) {
		printf("Error: %s %s %s", "No permission for", fileName, "file.\n");
		exit(1);
	}

	if(errno == ENOENT && !strcmp(fileName,"network")) {
		printf("Error: network file does not exist.\n");
		exit(1);
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

void free_all_and_close(){
	free_graph();
	free_cplex_data();
	free_results();

	fclose(networkFile);
	fclose(resultsFile);
	fclose(clusteringSolutionFile);
	fclose(bestClustersFile);
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

	if (create_graph(networkFile, C)){
		free_all_and_close();
		exit(1);
	}

	if (graph_to_cplex(graph, argv[2], C)){
		free_all_and_close();
		exit(1);
	}

	if (cluster()){
		free_all_and_close();
		exit(1);
	}

	if (get_results(graph)){
		free_all_and_close();
		exit(1);
	}

	strcpy(resultsFilePath,argv[2]);
	strcat(resultsFilePath,"results");

	resultsFile = fopen(resultsFilePath,"w");
	VerifyFile(resultsFile,"results");

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

	free_all_and_close();
	return 0;
}

