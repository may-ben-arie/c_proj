#ifndef OUTPUT_FILES_H_
#define OUTPUT_FILES_H_

#include "graph.h"

char ** clusterColors;


void WriteResultsFile(FILE * resultsFile,Graph * graph);
void WriteXGMMLFiles(FILE * clusteringSolutionFile, FILE * bestClustersFile, Graph * graph);
void WriteIntro(FILE * file, char * label);
void WriteNode(FILE * file, Vertex * vertex);
void WriteEdge(FILE * file, Vertex * vertex, Vertex * vertexTo, Graph * graph);



#endif
