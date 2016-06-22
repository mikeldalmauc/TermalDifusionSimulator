/* File: difussion.c */ 

#include "defines.h"
#include <mpi.h>

/************************************************************************************/
void thermal_update (struct info_param param, float *grid, float *grid_chips, int nrows){
  int i, j;
  
  // heat injection at chip positions
  for (i=1; i<nrows-1; i++)
  for (j=1; j<NCOL-1; j++) 
    if (grid_chips[i*NCOL+j] > grid[i*NCOL+j])
      grid[i*NCOL+j] += 0.05 * (grid_chips[i*NCOL+j] - grid[i*NCOL+j]);

  // air cooling at the middle of the card
  int a = 0.45*(NCOL-2)+1;
  int b = 0.55*(NCOL-2)+1;

  for (i=1; i<nrows-1; i++)
  for (j=a; j<b; j++)
      grid[i*NCOL+j] -= 0.01 * (grid[i*NCOL+j] - param.t_ext);
}


/************************************************************************************/
void diffusion (struct info_param param, float *grid, float *grid_chips, float *grid_aux, int nrows, int npr, int pid){
  int    i, j, end, niter;
  float  T;
  double Tmean, Tmean0 = param.t_ext;
  float frontier[2*NCOL];
  MPI_Status	info;
  
  end = 0; niter = 0;

  while (end == 0){
	niter++;
	Tmean = 0.0;
	
    // heat injection and air cooling 
    thermal_update (param, grid, grid_chips, nrows);
	
	//***********   FRONTERA   ****************************************	
	//Send up frontier
	if(pid > 0){
		MPI_Send(&grid[NCOL], NCOL, MPI_FLOAT, pid-1 , 0, MPI_COMM_WORLD);
	}
	//Send bot frontier
	if(pid < npr-1){
		MPI_Send(&grid[NCOL*(nrows-2)], NCOL, MPI_FLOAT, pid+1 , 0, MPI_COMM_WORLD);
	}
	//Receive up frontier
	if(pid < npr-1){
		MPI_Recv(&grid[NCOL*(nrows-1)], NCOL, MPI_FLOAT, pid+1, 0, MPI_COMM_WORLD, &info );
	}
	//Receive bot frontier
	if(pid > 0){
		MPI_Recv(&grid[0], NCOL, MPI_FLOAT, pid-1, 0, MPI_COMM_WORLD, &info);
	}
	//***********   FRONTERA   ****************************************
	
    // thermal difussion
    for (i=1; i<nrows-1; i++)
    for (j=1; j<NCOL-1; j++){
      T = grid[i*NCOL+j] + 
          0.10 * (grid[(i+1)*NCOL+j] + grid[(i-1)*NCOL+j] + grid[i*NCOL+(j+1)] + grid[i*NCOL+(j-1)] + 
                 grid[(i+1)*NCOL+j+1] + grid[(i-1)*NCOL+j+1] + grid[(i+1)*NCOL+(j-1)] + grid[(i-1)*NCOL+(j-1)] 
                 - 8*grid[i*NCOL+j]);

      grid_aux[i*NCOL+j] = T;
      Tmean += T;
    }
	
    //Update values of the grid
    for (i=1; i<nrows-1; i++)
    for (j=1; j<NCOL-1; j++)
      grid[i*NCOL+j] = grid_aux[i*NCOL+j]; 

	// convergence every 10 iterations
	if (niter % 10 == 0){
		
      //************* SUMA DE TODAS LAS TEMPERATURAS PARA COMVERGENCIA **************************
	  
	  double tmean;
      MPI_Allreduce(&Tmean, &tmean, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	  Tmean = tmean;
	   //************* SUMA DE TODAS LAS TEMPERATURAS PARA COMVERGENCIA **************************
	   
	  Tmean = Tmean / ((NCOL-2)*(NROW-2));
	  if ((fabs(Tmean - Tmean0) < param.t_delta) || (niter > param.max_iter))
		   end = 1;
	  else Tmean0 = Tmean;
	}
  }
}


