
#ifndef A_HSI_CARL_H
#define A_HSI_CARL_H

#include "vislib.h"

#define init_size 10000
#define incrment 1000


typedef struct
{  
	int *base;
    int top;
    int stacksize;
}sqstack;


int initstack(sqstack *s);
int push(sqstack *s,int e);
int pop(sqstack *s,int *e);
int destroystack(sqstack *s);


int save_sample_hsi_carl_params(vlHSI_carl_tol_t *p);
int load_sample_hsi_carl_params(vlHSI_carl_tol_t *p);


#endif

