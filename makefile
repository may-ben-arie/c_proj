CPLEX=cplex125
CPARCH=x86-64_sles10_4.1

# General flags for compilation:
CFLAGS+= -ansi -Wall -g
# Cplex header files:
CFLAGS+= -I/usr/local/stow/${CPLEX}/lib/${CPLEX}/include

# Linkage flags for cplex:
LDFLAGS+= -L/usr/local/stow/${CPLEX}/lib/${CPLEX}/lib/${CPARCH}/static_pic -lilocplex -lcplex

# General flags for linkage:
LDFLAGS+= -lm -lpthread -lgraph

all: cluster

clean:
	rm main.o graph.o graph_to_cplex.o cplex_to_clusters.o output_files.o cluster_editing.o cluster

cluster: main.o cluster_editing.o graph.o graph_to_cplex.o cplex_to_clusters.o output_files.o 
	gcc -o cluster main.o graph.o graph_to_cplex.o cplex_to_clusters.o output_files.o cluster_editing.o $(CFLAGS) $(LDFLAGS)

main.o: main.c graph.h graph_to_cplex.h cplex_to_clusters.h output_files.h
	gcc -c main.c $(CFLAGS)

cluster_editing.o: cluster_editing.h cluster_editing.c
	gcc -c cluster_editing.c $(CFLAGS)
	
graph.o: graph.c graph.h defines.h
	gcc -c graph.c $(CFLAGS)

graph_to_cplex.o: graph_to_cplex.c graph_to_cplex.h graph.h cluster_editing.h defines.h
	gcc -c graph_to_cplex.c $(CFLAGS)

cplex_to_clusters.o: cplex_to_clusters.c cplex_to_clusters.h graph.h graph_to_cplex.h cluster_editing.h defines.h
	gcc -c cplex_to_clusters.c $(CFLAGS)

output_files.o: output_files.c output_files.h graph.h graph_to_cplex.h cplex_to_clusters.h defines.h
	gcc -c output_files.c $(CFLAGS)
	
#TO DO:	
#
# 1. For each something.c you prepared, copy and alter the lines:
# something.o: something's dependencies (something.c and all of the .h files that something.c is using)
#	gcc -c something.c $(CFLAGS)
# 2. Add something.o to the rm command
# 3. Add something.o to the dependencies of cluster
# 4. Add something.o to the linkage command
# Notice, the executable name will be 'cluster'.
# If you are not planning on using libxml for creating the xgmml file you can delete the following lines:
# CFLAGS+= -I/usr/include/libxml2
# LDFLAGS+= -lxml2
# (You do not have to delete, but it might be easier to compile).
#
# If you just opened cluster.zip and you are trying to make all for the first time, you will recieve a linkage error. 
# This is because you do not have a main(). Create the main function and try again.

