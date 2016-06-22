/* File: defines.h */ 

// minimal card and maximun size
#define RSIZE 200
#define CSIZE 100
#define MAX_GRID_POINTS 3000000

#define NROW (RSIZE*param.scale + 2)
#define NCOL (CSIZE*param.scale + 2)

struct info_chip {
  int  	 x, y, h, w;
  float  tchip;
};

struct info_param {
  int    nconf, nchip;
  float  t_ext, tmax_chip, t_delta;
  int    max_iter, scale;
  struct info_chip **chips;
};

 struct temp {
  double Tmean;
  int    conf;
  float  bgrid[MAX_GRID_POINTS];
  float  cgrid[MAX_GRID_POINTS];
};