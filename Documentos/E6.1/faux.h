/* File: faux.h */ 

extern void read_data (char *, struct info_param *);
extern void results_conf (int, struct info_param, float *, float *, struct temp *);
extern void fprint_grid (FILE *, float *, struct info_param);
extern void results (struct info_param, struct temp *, char *);
