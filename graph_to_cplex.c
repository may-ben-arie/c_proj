#include "graph_to_cplex.h"

int graph_to_cplex(Graph *graph, char *output_path, double ghost_value) {
	if (graph == NULL || output_path == NULL){
		perror("Error: Null pointer for required data.");
		return 1;
	}

	/* set probname */
	if (set_probname(output_path)) {
		free_cplex_data();
		return 1;
	}

	/* set numcols & numrows */
	if (set_numcols_numrows(graph)) {
		free_cplex_data();
		return 1;
	}

	/* set edge_arr */
	if (set_edge_arr(graph,ghost_value)) {
		free_cplex_data();
		return 1;
	}

	/* set objsen & obj */
	if (set_objsen_obj()) {
		free_cplex_data();
		return 1;
	}

	/* set sense & rhs */
	if (set_sense_rhs()) {
		free_cplex_data();
		return 1;
	}

	/* set matbeg * matcnt & matind & matval */
	if (set_matbeg_matcnt_matind_matval(graph)){
		free_cplex_data();
		return 1;
	}

	/* set lb & ub */
	if (set_lb_ub()) {
		free_cplex_data();
		return 1;
	}

	/* set cnt & indices & *ctype */
	if (set_cnt_indices_ctype()) {
		free_cplex_data();
		return 1;
	}

	x = malloc(numcols * sizeof(double));
	if (x == NULL){
		perror("Error: Failed to allocate memory");
		free_cplex_data();
		return 1;
	}

	return 0;
}

int set_probname(char* output_path) {
	probname = calloc(strlen(output_path) + strlen("clustering_solution.lp")+1,sizeof(char));
	if (probname == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	strcat(probname, output_path);
	strcat(probname,"clustering_solution.lp");
	return 0;
}

int set_numcols_numrows(Graph *graph) {
	int x = graph->vertexCounter;

	numcols = x*(x-1) / 2;
	numrows = 3*x*(x-1)*(x-2) / 6;

	return 0;
}

int set_edge_arr(Graph *graph, double ghost_value) {
	int edge_id = 0;
	int i,j;
	edge_arr = malloc(numcols * sizeof(Edge));
	if (edge_arr == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	for (i=0;i<graph->vertexCounter-1;i++){
		for (j=i+1;j<graph->vertexCounter;j++){
			edge_arr[edge_id].vertexID1 = i;
			edge_arr[edge_id].vertexID2 = j;
			edge_arr[edge_id].weight = GetEdgeWeightByVertices(graph,i,j);
			if (edge_arr[edge_id].weight == -1)
				edge_arr[edge_id].weight = -1 * ghost_value;
			edge_id++;
		}
	}
	return 0;
}

int set_objsen_obj() {
	int i;
	objsen = CPX_MAX;
	obj = malloc (numcols*sizeof(double));
	if (obj == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}
	for (i=0;i<numcols;i++){
		obj[i] = edge_arr[i].weight;
	}
	return 0;
}

int set_sense_rhs() {
	int i;
	sense = malloc(numrows * sizeof(char));
	rhs = malloc(numrows * sizeof(double));
	if (sense == NULL || rhs == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}

	for (i=0;i<numrows;i++) {
		sense[i] = 'L';
		rhs[i] = 1;
	}
	return 0;
}

int set_matbeg_matcnt_matind_matval(Graph *graph){
	int i,j,k,con,edge_i_j,edge_i_k,edge_j_k;
	matbeg = malloc(numcols * sizeof(int));
	matcnt = malloc(numcols * sizeof(int));
	matind = malloc(3 * numrows * sizeof(int));
	matval = malloc(3 * numrows * sizeof(double));
	int* indexes = malloc(numcols * sizeof(int));
	if (matbeg == NULL || matcnt == NULL || matind == NULL || matval == NULL || indexes == NULL) {
		perror("Error: Failed to allocate memory");
		return 1;
	}

	for (i=0;i<numcols;i++){
		matbeg[i] = 3 * (graph->vertexCounter-2) * i;
		matcnt[i] = 3 * (graph->vertexCounter-2);
		indexes[i] = 3 * (graph->vertexCounter-2) * i;
	}

	con = 0;
	for (i=0;i<graph->vertexCounter-2;i++){
		for (j=i+1;j<graph->vertexCounter-1;j++){
			for (k=j+1;k<graph->vertexCounter;k++){
				edge_i_j = get_edge_id(i,j);
				edge_i_k = get_edge_id(i,k);
				edge_j_k = get_edge_id(j,k);

				matval[indexes[edge_i_j]] = 1;
				matval[indexes[edge_i_k]] = 1;
				matval[indexes[edge_j_k]] = -1;
				matind[indexes[edge_i_j]] = con;
				matind[indexes[edge_i_k]] = con;
				matind[indexes[edge_j_k]] = con;

				matval[indexes[edge_i_j]+1] = 1;
				matval[indexes[edge_i_k]+1] = -1;
				matval[indexes[edge_j_k]+1] = 1;
				matind[indexes[edge_i_j]+1] = con+1;
				matind[indexes[edge_i_k]+1] = con+1;
				matind[indexes[edge_j_k]+1] = con+1;

				matval[indexes[edge_i_j]+2] = -1;
				matval[indexes[edge_i_k]+2] = 1;
				matval[indexes[edge_j_k]+2] = 1;
				matind[indexes[edge_i_j]+2] = con+2;
				matind[indexes[edge_i_k]+2] = con+2;
				matind[indexes[edge_j_k]+2] = con+2;

				indexes[edge_i_j] += 3;
				indexes[edge_i_k] += 3;
				indexes[edge_j_k] += 3;
				con += 3;
			}
		}
	}

	return 0;
}

void print_arr_int(int *matind, int size){
	int i;
	for (i=0;i<size;i++){
		printf("%d,",matind[i]);
	}
	printf("\n");
}
void print_arr_double(double *matind, int size){
	int i;
	for (i=0;i<size;i++){
		printf("%f,",matind[i]);
	}printf("\n");

}

int set_lb_ub(){
	int i;
	lb = malloc(numrows * sizeof(double));
	ub = malloc(numrows * sizeof(double));
	if (lb == NULL || ub == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}

	for (i=0;i<numrows;i++) {
		lb[i] = 0;
		ub[i] = 1;
	}
	return 0;
}

int set_cnt_indices_ctype(){
	int i;

	cnt = numcols;
	indices = malloc(numcols * sizeof(int));
	ctype = malloc(numcols * sizeof(char));
	if (indices == NULL || ctype == NULL){
		perror("Error: Failed to allocate memory");
		return 1;
	}

	for (i=0;i<numcols;i++) {
		indices[i] = i;
		ctype[i] = 'B';
	}
	return 0;
}

void free_cplex_data(){
	free_and_null((void **)&obj);
	free_and_null((void **)&sense);
	free_and_null((void **)&rhs);
	free_and_null((void **)&matbeg);
	free_and_null((void **)&matcnt);
	free_and_null((void **)&matind);
	free_and_null((void **)&matval);
	free_and_null((void **)&lb);
	free_and_null((void **)&ub);
	free_and_null((void **)&indices);
	free_and_null((void **)&ctype);
	free_and_null((void **)&x);
	free_and_null((void **)&edge_arr);
	free_and_null((void **)&x);
}

int get_edge_id(int ver1,int ver2){
	int i;
	Edge edge;
	if (edge_arr == NULL)
		return -1;

	for (i=0;i<numcols;i++){
		edge = edge_arr[i];
		if ((edge.vertexID1 == ver1 && edge.vertexID2 == ver2) ||
				(edge.vertexID1 == ver2 && edge.vertexID2 == ver1))
			return i;
	}
	return -1;
}
