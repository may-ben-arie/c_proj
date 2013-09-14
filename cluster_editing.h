#ifndef CLUSTER_EDITING_H
#define CLUSTER_EDITING_H

/* Bring in the CPLEX function declarations and the C library 
   header file stdio.h with include of cplex.h. */
#include <ilcplex/cplex.h>
#include <stdlib.h>

/* Bring in the declarations for the string functions */
#include <string.h>

/* Declare pointers for the variables and arrays that will contain
      the data which define the LP problem. */
char     	*probname;

/* parameters for CPXcopylp */
int 		numcols;
int 		numrows;
int 		objsen;
double 		*obj;
double 		*rhs;
char 		*sense;
int 		*matbeg;
int 		*matcnt;
int 		*matind;
double 		*matval;
double 		*lb;
double 		*ub;
double 		*rngval;

/* parameters for CPXchgctype */
int 		cnt;
int 		*indices;
char 		*ctype;

/* parameters for CPXsolution */
int 		solstat;
double 		objval;
double 		*x;

/* Include declaration for functions */
int  cluster();
void free_and_null (void **ptr);

#endif
