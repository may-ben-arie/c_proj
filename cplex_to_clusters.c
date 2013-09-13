/*
 * cplex_to_clustes.h
 */

#include "cplex_to_clusters.h"
#define IS_VALUE_1(X)((1-X)<0.00001)

int get_results(Graph *graph){
	results = malloc(sizeof(Results));
	if (results == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}

	set_score();
	if (set_clusters(graph)){
		return 1;
	}
	remove_edges(graph);
	return 0;
}

void set_score(){
	int i;
	results->score = 0;
	for (i=0;i<numcols;i++){
		if (edge_arr[i].weight > 0){
			results->score += edge_arr[i].weight;
		}
	}
	results->score -= objval;
	printf("%f clusters score\n",results->score);
}

int set_clusters(Graph *graph){
	int *vertex_clusters_id = malloc(graph->vertexCounter * sizeof(int));
	if (vertex_clusters_id == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	set_unsorted_clusters_id(graph,vertex_clusters_id);

	results->clusters = malloc(results->num_of_clusters * sizeof(Cluster));
	if (results->clusters == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	create_clusters_array(graph,vertex_clusters_id);
	sort_clusters(graph,vertex_clusters_id);
	set_avg_weight(vertex_clusters_id);
	set_clusters_diameter(graph);
	print_clusters();
	return 0;
}

void set_unsorted_clusters_id(Graph *graph, int *vertex_clusters_id){
	int i,j,index=0;
	//Initialize vertices clusters id
	for (i=0;i<graph->vertexCounter;i++){
		vertex_clusters_id[i] = -1;
	}

	//set vertices clusters id (unsorted)
	for (i=0;i<graph->vertexCounter;i++){
		if (vertex_clusters_id[i] != -1)
			continue;

		vertex_clusters_id[i] = index;
		for (j=0;j<numcols;j++){
			if (!IS_VALUE_1(x[j])){
				continue;
			}
			if (edge_arr[j].vertexID1 == i){
				vertex_clusters_id[edge_arr[j].vertexID2] = index;
			}
			else if (edge_arr[j].vertexID2 == i){
				vertex_clusters_id[edge_arr[j].vertexID1] = index;
			}
		}
		index++;
	}
	results->num_of_clusters = index;
	printf("%d clusters\n",index);
	print_arr_int(vertex_clusters_id,graph->vertexCounter);
}

void create_clusters_array(Graph *graph, int *vertex_clusters_id){
	int i,ver_ID1,ver_ID2;
	for (i=0;i<results->num_of_clusters;i++){
		results->clusters[i].oldID = i;
		results->clusters[i].ID = -1;
		results->clusters[i].num_of_ver = 0;
		results->clusters[i].score = 0;
	}

	for (i=0;i<graph->vertexCounter;i++){
		results->clusters[vertex_clusters_id[i]].num_of_ver++;
	}

	for (i=0;i<numcols;i++){
		ver_ID1 = edge_arr[i].vertexID1;
		ver_ID2 = edge_arr[i].vertexID2;
		if (vertex_clusters_id[ver_ID1] == vertex_clusters_id[ver_ID2] && edge_arr[i].weight > 0){
			results->clusters[vertex_clusters_id[ver_ID1]].score += edge_arr[i].weight;
		}
	}
}

void sort_clusters(Graph *graph,int *vertex_clusters_id){
	int i;
	//print_clusters();
	qsort(results->clusters,results->num_of_clusters,sizeof(Cluster),comp);
	for (i=0;i<results->num_of_clusters;i++){
			results->clusters[i].ID = i+1;
	}
	for (i=0;i<graph->vertexCounter;i++){
		vertex_clusters_id[i] = new_ID_for_old_ID(vertex_clusters_id[i]);
		GetVertex(graph,i)->clusterID = vertex_clusters_id[i];
	}
	print_clusters();
}

int comp(const void* obj1,const void* obj2){
	int n1,n2;
	double score1,score2;
	n1 = ((Cluster*)obj1)->num_of_ver;
	n2 = ((Cluster*)obj2)->num_of_ver;
	if (n1 == n2){
		score1=((Cluster*)obj1)->score;
		score2=((Cluster*)obj1)->score;
		return score2-score1;
	}
	return n2-n1;
}

int new_ID_for_old_ID(int old_id){
	int i;
	for (i=0;i<results->num_of_clusters;i++){
		if (results->clusters[i].oldID == old_id){
			return results->clusters[i].ID;
		}
	}
	return -1;
}

void remove_edges(Graph *graph){
	int i;
	for (i=0;i<numcols;i++){
		if (edge_arr[i].weight >0 && !IS_VALUE_1(x[i])){
			DeleteEdgeFromGraph(graph,edge_arr[i].vertexID1,edge_arr[i].vertexID2);
		}
	}
}

void set_avg_weight(int *vertex_clusters_id){
	int i,ver_ID1,ver_ID2;
	int num_between=0,num_within=0;
	results->avg_weight_between_clusters = 0;
	results->avg_weight_within_clusters = 0;

	for (i=0;i<numcols;i++){
		if (edge_arr[i].weight <= 0){
			continue;
		}
		ver_ID1 = edge_arr[i].vertexID1;
		ver_ID2 = edge_arr[i].vertexID2;
		if (vertex_clusters_id[ver_ID1] != vertex_clusters_id[ver_ID2]){
			num_between++;
			results->avg_weight_between_clusters+=edge_arr[i].weight;
		}else{
			num_within++;
			results->avg_weight_within_clusters+=edge_arr[i].weight;
		}
	}
	if (num_between > 0){
		results->avg_weight_between_clusters /= num_between;
	}
	if (num_within > 0){
		results->avg_weight_within_clusters /= num_within;
	}
	printf("\navg_weight_between_clusters=%f",results->avg_weight_between_clusters);
	printf("\navg_weight_within_clusters=%f",results->avg_weight_within_clusters);
	printf("\n");
}

int get_vertex_diameter(int vertex_ID, Graph *graph){
	int max = 0;
	int *queue,*dist;
	int queue_index = 0;
	int i,last_in_queue;
	int cluster_ID = GetVertex(graph,vertex_ID)->clusterID;
	Neighbor *neighbor;

	dist = malloc(graph->vertexCounter * sizeof(int));
	queue = malloc(graph->vertexCounter * sizeof(int));
	if (queue == NULL || dist == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	queue[0] = -1;

	for(i=0;i<graph->vertexCounter;i++){
		if (GetVertex(graph,i)->clusterID == cluster_ID){
			dist[i] = graph->vertexCounter;
		}else{
			dist[i] = -1;
		}
	}

	dist[vertex_ID] = 0;
	queue[queue_index] = vertex_ID;
	queue_index++;
	queue[queue_index] = -1;

	while(queue_index != 0){
		last_in_queue = queue[queue_index-1];
		queue[queue_index-1] = -1;
		queue_index--;
		neighbor = GetVertexOriginals(graph,last_in_queue);

		while (neighbor != NULL){
			if(dist[neighbor->vertexID] == graph->vertexCounter){
				dist[neighbor->vertexID] = dist[last_in_queue] + 1;
				queue[queue_index] = neighbor->vertexID;
				queue_index++;
				queue[queue_index] = -1;
			}
			neighbor = neighbor->next;
		}
	}

	for(i=0;i<graph->vertexCounter;i++){
		max = (max < dist[i] ? dist[i] : max);
	}

	free(queue);
	return max;
}

void set_clusters_diameter(Graph *graph){
	int i,j;
	int max_cluster,max_ver;
	for (i=0;i<results->num_of_clusters;i++){
		max_cluster = 0;
		max_ver = 0;

		for(j=0;j<graph->vertexCounter;j++){

			if (GetVertex(graph,j)->clusterID == i+1){
				max_ver = get_vertex_diameter(j,graph);
			}

			if (max_ver > max_cluster){
				max_cluster = max_ver;
			}
		}

		results->clusters[i].diameter = max_cluster;
	}
}

void print_clusters(){
	int i;
	printf("\n ---Clusters---\n");
	for (i=0;i<results->num_of_clusters;i++){
		printf("Cluster ID=%d oldID=%d size=%d score=%f diameter=%d\n",results->clusters[i].ID,results->clusters[i].oldID,results->clusters[i].num_of_ver,results->clusters[i].score,results->clusters[i].diameter);
	}

}
