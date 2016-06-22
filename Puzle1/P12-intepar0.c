/*******************************************************************************
	P12-intepar0.c 
	Integral of a function by sums of areas of trapezoids
	Using broadcast for data sending and Reduce for data receiving

	>>> TO DO: MODIFY AND COMPLETE <<<

*********************************************************************************/
#include <mpi.h>
#include <stdio.h>

double   t0, t1;

void   Read_data(double* a_ptr, double* b_ptr, int* n_ptr, int pid);
double Integrate(double a_loc, double b_loc, int n_loc, double w);
double f(double x);


int main(int argc, char** argv){
  int         pid, npr, root;    	// Identifier, number of process and root node
  double      a, b, w, a_loc, b_loc;    
  int         n, n_loc, remainder;   
  double      resul, resul_loc;		// Result of the integral: global and local


// MPI Initializations
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &npr);


// Reading parameters and distributing them to all processes  
//	>>> TO DO: MODIFY THE READ_DATA FUNCTION <<<
  Read_data(&a, &b, &n, pid, npr);

// Dividing the calculation among the processes  
  w = (b-a) / n;       
  n_loc = n / npr;
  remainder = n % npr;

  if (pid < remainder) n_loc = n_loc + 1;

  a_loc = a + pid * n_loc * w;
  if (pid >= remainder) a_loc = a_loc + remainder * w;
  b_loc = a_loc + n_loc * w;


// Local calculation of the integral
   resul_loc = Integrate(a_loc, b_loc, n_loc, w);

/* 
Adding the partial results,

   Description of parameters:
	sendbuf  - local result of integral
	recvbuf  - total result of integral
    count    - 1 element in send buffer per porcess
    datatype - We are using double precission
    op       - We will compute a sume
    root     - the process 0
    comm     - All the processes active 
*/
  MPI_Reduce(&resul_loc, &resul, 1, MPI_DOUBLE	, MPI_SUM, 0, MPI_COMM_WORLD);

// Print results
  if (pid == 0){
    t1 = MPI_Wtime();

    printf("\n  Integral (= ln x+1 + atan x), from %1.1f to %1.1f, %d trap.) = %1.12f\n",a,b,n,resul);
    printf("  Execution time (%d proc.) = %1.3f ms \n\n", npr, (t1-t0)*1000);
  }

//MPI Finalisation
  MPI_Finalize();
  return (0);
} /*  main  */



// FUNCION Read_data (SERIAL VERSION)
void Read_data(double* a_ptr, double* b_ptr, int* n_ptr, int pid){
  float a, b;
  float buf[3];
  
  if (pid == 0){
	printf("\n  Introduce a, b (limits) and n (num. of trap.)  \n");
	scanf("%f %f %d", &a, &b, n_ptr);
	buf[0] = a; 	
	buf[1] = b;
	buf[2] = (float)*n_ptr;
  }
   //Distribute read values
   MPI_Bcast(&buf,3,MPI_FLOAT,0,MPI_COMM_WORLD);

  (*a_ptr)= (double)(buf[0]);
  (*b_ptr)= (double)(buf[1]);
  (*n_ptr)= (double)(buf[2]);
  
} /* Read_data */



// FUNCTION Integrate: local calculation of the integral
double Integrate(double a_loc, double b_loc, int n_loc, double w){ 
  double resul_loc, x; 
  int    i; 

  // Integral calculation
  resul_loc = (f(a_loc) + f(b_loc)) / 2.0; 
  x = a_loc; 

  for (i=1; i<n_loc; i++){ 
    x = x + w; 
    resul_loc = resul_loc + f(x); 
  } 
  resul_loc = resul_loc * w; 

  return (resul_loc);
} /*  Integrate  */



// FUNCTION f: function to integrate
double f(double x){ 
  double y; 
  
  y = 1.0 / (x + 1.0) + 1.0 / (x*x + 1.0);
  return (y); 
} /* f function */