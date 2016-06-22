#include <stdio.h>
#include <values.h>

#include "defines.h"

/************************************************************************************/
void read_data (char *file_name,struct info_param *param){
  int i, j, h, w;
  float tchip;
  FILE *fdin;

  fdin = fopen (file_name, "r");

  fscanf (fdin, "%d %d %f %f %f %d %d", &param->nconf, &param->nchip, &param->t_ext, &param->tmax_chip, &param->t_delta, &param->max_iter, &param->scale);
  if (param->scale > 12) {
    printf("\n\nERROR: maximum scale factor is 12 \n\n");
    exit (-1);
  }

  param->chips = (struct info_chip **) malloc(param->nconf*sizeof(struct info_chip*));
  for (i=0; i<param->nconf; i++)
    param->chips[i] = (struct info_chip *) malloc(param->nchip * sizeof(struct info_chip));

  // chip sizes and temperatures
  for (j=0; j<param->nchip; j++)
  {
    fscanf (fdin, "%d %d %f", &h, &w, &tchip);
    for (i=0; i<param->nconf; i++)
    {
      param->chips[i][j].h = h;
      param->chips[i][j].w = w;
      param->chips[i][j].tchip = tchip;
    }
  }

  // chip positions
  for (i=0; i<param->nconf; i++)
  for (j=0; j<param->nchip; j++)
    fscanf (fdin, "%d %d", &param->chips[i][j].x, &param->chips[i][j].y);

  fclose (fdin);
}


/************************************************************************************/
void results_conf (int conf,struct info_param param, float *grid, float *grid_chips,struct temp *BT){
  int   i, j;
  float Tmax = MINFLOAT, Tmin = MAXFLOAT;
  double Tmean = 0.0;

  //Cálculo de la temperatura media de la placa
  for (i=1; i<NROW-1; i++)
  for (j=1; j<NCOL-1; j++)
    Tmean += grid[i*NCOL+j];

  Tmean = Tmean / ((NROW-2)*(NCOL-2));
	
  if (BT->Tmean > Tmean) 
  {
    BT->Tmean = Tmean;
    BT->conf = conf+1;
    for (i=1; i<NROW-1; i++)
    for (j=1; j<NCOL-1; j++) 
    {
      BT->bgrid[i*NCOL+j] = grid[i*NCOL+j];
      BT->cgrid[i*NCOL+j] = grid_chips[i*NCOL+j];
    }
  } 

  printf ("Config: %2d \t Tmean: %1.2f\n", conf+1, Tmean);
}


/************************************************************************************/
void fprint_grid (FILE *fd, float *grid,struct info_param param){
  int i, j;

  // j - i order for better visualitation
  for (j=NCOL-2; j>0; j--)
  {
    for (i=1; i<NROW-1; i++) fprintf (fd, "%1.2f ", grid[i*NCOL+j]);
    fprintf (fd, "\n");
  }	
  fprintf (fd, "\n");
}


/************************************************************************************/
void results (struct info_param param,struct  temp *BT, char *finput){
  FILE  *fd;
  char  name[100];

  printf ("\n\n >>> BEST CONFIGURATION: %2d\t Tmean: %1.2f\n\n", BT->conf, BT->Tmean); 

  sprintf (name, "%s_ser.res", finput);
  fd = fopen (name, "w");
  fprintf (fd, "Tmin_ini %1.1f  Tmax_ini %1.1f  \n", param.t_ext, param.tmax_chip);
  fprintf (fd, "%d\t  %d \n", NCOL-2, NROW-2);

  fprint_grid (fd, BT->bgrid, param);

  fprintf (fd, "\n\n >>> BEST CONFIGURATION: %d\t Tmean: %1.2f\n\n", BT->conf, BT->Tmean);
  fclose (fd);

  sprintf (name, "%s_ser.chips", finput);
  fd = fopen (name, "w");
  fprintf (fd, "Tmin_chip %1.1f  Tmax_chip %1.1f  \n", param.t_ext, param.tmax_chip);
  fprintf (fd, "%d\t  %d \n", NCOL-2, NROW-2);

  fprint_grid (fd, BT->cgrid, param);

  fclose (fd);
}

