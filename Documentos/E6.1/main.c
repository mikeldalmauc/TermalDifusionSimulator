/* File: heats.c */ 

#include <stdio.h>
#include <values.h>
#include <sys/time.h>
#include <mpi.h>

#include "defines.h"
#include "faux.h"
#include "difussion.h"

// global variables
float grid_chips[MAX_GRID_POINTS], grid[MAX_GRID_POINTS], grid_aux[MAX_GRID_POINTS];
struct temp BT;

/************************************************************************************/
void init_grid_chips (int conf, struct info_param param, float *grid_chips)
{
  int i, j, n;

  for (i=0; i<NROW; i++)
  for (j=0; j<NCOL; j++)  
    grid_chips[i*NCOL+j] = param.t_ext;

  for (n=0; n<param.nchip; n++)
  for (i=param.chips[conf][n].x*param.scale; i<(param.chips[conf][n].x+param.chips[conf][n].h)*param.scale; i++)
  for (j=param.chips[conf][n].y*param.scale; j<(param.chips[conf][n].y+param.chips[conf][n].w)*param.scale; j++) 
    grid_chips[(i+1)*NCOL+(j+1)] = param.chips[conf][n].tchip;
}

/************************************************************************************/
void init_grids (struct info_param param, float *grid, float *grid_aux)
{
  int i, j;

  for (i=0; i<NROW; i++)
  for (j=0; j<NCOL; j++) 
    grid[i*NCOL+j] = grid_aux[i*NCOL+j] = param.t_ext;
}

/************************************************************************************/
void calculate_chop_points (struct info_param param,  int *displacement, int *size, int npr){
  //Compute the division and remainder of rows
  int reparto = NROW/npr;
  int remainder = (NROW % npr);
  int i;
  
  //Initializations of size and displacement
  displacement[0] = 0;
  if(remainder>0){
    size[0] = (reparto+2)*NCOL;
    remainder--;
  }else if(npr != 1){
    size[0] = (reparto+1)*NCOL;
  }else{
	size[0] = NROW*NCOL;
  }
  for(i=1; i<npr; i++){
    if(remainder>0){
	  size[i] = (reparto+1+2)*NCOL;
	  //Las últimas dos filas y primeras dos filas se comparten con los nodos vecinos
	  displacement[i] = displacement[i-1]+size[i-1]-2*NCOL;
	  remainder -= 1;
	}else{
	  if(i == npr-1){
		size[i] = (reparto+1)*NCOL;
	  }else{
		size[i] = (reparto+2)*NCOL;
	  }
	  displacement[i] = displacement[i-1]+size[i-1]-2*NCOL;
	}
  }
}

/************************************************************************************/
int main (int argc, char *argv[]){
  int    conf, nconf, pid, npr, i, j;
  struct info_param param;
  struct timeval t0, t1;
  double *tej, tsim = 0.0;
  
  // MPI Initializations
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &pid);
  MPI_Comm_size(MPI_COMM_WORLD, &npr);
  
  if( pid == 0){
	  
	 // reading initial data file
	 if (argc != 2) {
	   printf ("\n\nERROR: needs a card description file \n\n");
	   exit (-1);
	 } 

	 read_data (argv[1], &param);

	 printf ("\n  ===================================================================");
	 printf ("\n    Thermal difussion - Paralel version ");
	 printf ("\n    %d x %d points, %d chips", RSIZE*param.scale, CSIZE*param.scale, param.nchip);
	 printf ("\n    T_ext = %1.1f, Tmax_chip = %1.1f, T_delta: %1.3f, Max_iter: %d", param.t_ext, param.tmax_chip, param.t_delta, param.max_iter);
	 printf ("\n  ===================================================================\n\n");
	  
	 BT.Tmean = MAXDOUBLE;
	 tej = (double *) malloc(param.nconf * sizeof(double));
  }
  //Sending necessary parameters
  MPI_Bcast(&param.scale, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&param.nconf, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&param.t_ext, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&param.t_delta, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&param.max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  //Declaration of size and displacement of individual grids
  int displacement[npr];
  int size[npr];
  
  calculate_chop_points(param, &displacement[0], &size[0], npr);

  if(size[npr-1]+displacement[npr-1] != NROW*NCOL){
	    printf (" Error wrong distribution of chip : dist %1.3d real %1.3d \n\n",size[npr-1]+displacement[npr-1] , NROW*NCOL);
		return(2);
  }
  
  //Declaracion of individual parts of the grid
  float *grid_part;
  float *grid_aux_part;
  float *grid_chips_part;
  grid_part = malloc (sizeof(float) * size[pid]);
  grid_aux_part = malloc (sizeof(float) * size[pid]);
  grid_chips_part = malloc (sizeof(float) * size[pid]);
  
  // loop to process chip configurations
  for (conf=0; conf<param.nconf; conf++){
	if(pid == 0){
	  gettimeofday (&t0, 0);
	  init_grid_chips (conf, param, grid_chips);
    }
	init_grids (param, grid, grid_aux);

	// Scattering of grid chips among the processes
	MPI_Scatterv(&grid_chips[0], &size[0], &displacement[0],  MPI_FLOAT,
                 &grid_chips_part[0], size[pid], MPI_FLOAT, 0, MPI_COMM_WORLD);

	 //Create grid individual parts
    for (i=0; i<size[pid]; i++){
		grid_part[i] =  grid[i + displacement[pid]];
		grid_aux_part[i] = grid_part[i]; 
	}
	
    // main loop: thermal injection/disipation until convergence (t_delta or max_iter)
    diffusion (param, &grid_part[0], &grid_chips_part[0], &grid_aux_part[0], (size[pid]/NCOL), npr, pid);

	// Gathering of grid
	MPI_Gatherv(&grid_part[0], size[pid],MPI_FLOAT, &grid[0], 
			&size[0], &displacement[0], MPI_FLOAT, 0, MPI_COMM_WORLD);
				 
	if(pid == 0){
		// processing configuration results 
		gettimeofday (&t1, 0);
		tej[conf] = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec)/1e6;
		results_conf (conf, param, grid, grid_chips, &BT);
	}
  }
  if(pid == 0){
	  // writing best configuration results
	  results (param, &BT, argv[1]);
	  for (conf=0; conf<param.nconf; conf++) tsim += tej[conf];
	  printf ("   > Time (Paralel %d) : %1.3f s \n\n",npr, tsim);
  }	  
  //MPI Finalisation
  MPI_Finalize();
  return(0);
}

