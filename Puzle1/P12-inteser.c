/***********************************************************************

	P12-inteser.c 
	Integral of a function by sums of areas of trapezoids

************************************************************************/

#include <stdio.h>
#include <sys/time.h>

struct timeval   t0, t1;
double texec;

void   Read_data(double* a_ptr, double* b_ptr, int* n_ptr);
double Integrate(double a, double b, int n, double w);
double f(double x);


int main(int argc, char** argv) 
{
  double      a, b, w;    
  int         n;   
  double      resul;		// Result for the integral 


  Read_data(&a, &b, &n);
  w = (b-a) / n;   

// Integral calculation
  gettimeofday(&t0,0);
  resul = Integrate(a, b, n, w);

// Print results
  gettimeofday(&t1,0);
  texec = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec-t0.tv_usec) / 1e6;
  printf("\n  Integral (= ln x+1 + atan x), from %1.1f to %1.1f, %d trap. = %1.12f\n", a,b,n,resul);
  printf("  Execution time (serie) = %1.3f ms \n\n", texec*1000);

  return (0);
} /*  main  */


// FUNCION Read_data
void Read_data(double* a_ptr, double* b_ptr, int* n_ptr)
{
  float a, b;

  printf("\n  Introduce a, b (limits) and n (num. of trap.)  \n");
  scanf("%f %f %d", &a, &b, n_ptr);

  (*a_ptr)= (double)(a);
  (*b_ptr)= (double)(b);
} /* Read_data */



// FUNCTION Integrate: local calculation of the integral
double Integrate(double a, double b, int n, double w)
{ 
  double resul, x; 
  int    i; 

// Integral calculation
  resul = (f(a) + f(b)) / 2.0; 
  x = a; 

  for (i=1; i<n; i++) 
  { 
    x = x + w; 
    resul = resul + f(x); 
  }
  resul = resul * w; 

  return (resul);
} /*  Integrate  */



// FUNCTION f: Function to integrate
double f(double x)
{ 
  double y; 

  y = 1.0 / (x + 1.0) + 1.0 / (x*x + 1.0);
  return (y); 
} /* f function */



