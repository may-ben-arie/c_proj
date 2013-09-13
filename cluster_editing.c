/*------------------------------------------------------------------------*/
/*  File: cluster_editing.c                                                       */
/*  Version 1.0                                                           */
/*------------------------------------------------------------------------*/
/*  Written for Software Project Class.                                   */
/*  Based uopn qpex1.c                                                    */
/*  Permission is expressly granted to use this example in the            */
/*  course of developing applications that use ILOG products.             */
/*------------------------------------------------------------------------*/

/* cluster_editing.c - Entering and optimizing cluster editing integer linear programming problem */
/* You may and should alter the file */

#include "cluster_editing.h"
#include "ilcplex/cplex.h"

/* This routine initializes the cplex enviorement, sets screen as an output for cplex errors and notifications, 
   and sets parameters for cplex. It calls for a mixed integer program solution and frees the environment.
   To Do:
   Declare the parameters for the problem and fill them accordingly. After creating the program thus, copy it into cplex. 
   Define any integer or binary variables as needed, and change their type before the call to CPXmipopt to solve problem. 
   Use CPXwriteprob to output the problem in lp format, in the name of cluster_editing.lp.
   Read solution (both objective function value and variables assignment). 
   Communicate to pass the problem and the solution between the modules in the best way you see. 
 */
int cluster()
{
	/* Declare and allocate space for the variables and arrays where we
      will store the optimization results including the status, objective
      value and variable values. */

	CPXENVptr p_env              = NULL;
	CPXLPptr  p_lp               = NULL;
	int       status;

	/* Initialize the CPLEX environment */
	p_env = CPXopenCPLEX (&status);

	/* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message. Note that CPXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXgeterrorstring. For other CPLEX routines, the errors will
      be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON.  */

	if ( p_env == NULL ) {
		char  errmsg[1024];
		fprintf (stderr, "Error: Could not open CPLEX environment.\n");
		CPXgeterrorstring (p_env, status, errmsg);
		fprintf (stderr, "%s", errmsg);
		goto TERMINATE;
	}

	/* Turn on output to the screen */
	status = CPXsetintparam (p_env, CPX_PARAM_SCRIND, CPX_ON);
	if ( status ) {
		fprintf (stderr,
				"Error: Failure to turn on screen indicator, error %d.\n", status);
		goto TERMINATE;
	}

	/* Create the problem. */
	p_lp = CPXcreateprob (p_env, &status, probname);

	/* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of 
      failure, an error message will have been written to the error 
      channel from inside CPLEX. The setting of
      the parameter CPX_PARAM_SCRIND causes the error message to
      appear on stdout.  */

	if ( p_lp == NULL ) {
		fprintf (stderr, "Error: Failed to create problem.\n");
		goto TERMINATE;
	}

	/* Use CPXcopylp to transfer the ILP part of the problem data into the cplex pointer lp */
	CPXcopylp (p_env, p_lp, numcols, numrows, objsen, obj, rhs, sense, matbeg, matcnt, matind, matval, lb, ub, 0);
	CPXchgctype(p_env, p_lp, cnt, indices, ctype);

	/* Optimize the problem. */
	status = CPXmipopt (p_env, p_lp);
	if ( status ) {
		fprintf (stderr, "Error: Failed to optimize problem.\n");
		goto TERMINATE;
	}

	status = CPXsolution(p_env, p_lp, &solstat, &objval, x, NULL, NULL, NULL);
	if ( status ) {
			fprintf (stderr, "Error: Failed to get solution variables.\n");
			goto TERMINATE;
		}

	/* Write a copy of the problem to a file.
      Please put into probname the following string: Output Directory + "clustering_solution.lp" to create clustering_solution.lp in your output directory */
	status = CPXwriteprob (p_env, p_lp, probname, NULL);
	if ( status ) {
		fprintf (stderr, "Error: Failed to write LP to disk.\n");
		goto TERMINATE;
	}


	TERMINATE:

	/* Free up the problem as allocated by CPXcreateprob, if necessary */
	if ( p_lp != NULL ) {
		status = CPXfreeprob (p_env, &p_lp);
		if ( status ) {
			fprintf (stderr, "Error: CPXfreeprob failed, error code %d.\n", status);
		}
	}

	/* Free up the CPLEX environment, if necessary */
	if ( p_env != NULL ) {
		status = CPXcloseCPLEX (&p_env);

		/* Note that CPXcloseCPLEX produces no output,
         so the only way to see the cause of the error is to use
         CPXgeterrorstring.  For other CPLEX routines, the errors will
         be seen if the CPX_PARAM_SCRIND indicator is set to CPX_ON. */

		if ( status ) {
			char  errmsg[1024];
			fprintf (stderr, "Could not close CPLEX environment.\n");
			CPXgeterrorstring (p_env, status, errmsg);
			fprintf (stderr, "%s", errmsg);
		}
	}
	return (status);
}  


/* This simple routine frees up the pointer *ptr, and sets *ptr to NULL */
void free_and_null (void **ptr)
{
	if (*ptr){
		free(*ptr);
		*ptr = NULL;
	}
} 


