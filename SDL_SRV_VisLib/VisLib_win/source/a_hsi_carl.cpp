#include "vislib.h"
#include "a_hsi_carl.h"


int save_sample_hsi_carl_params(vlHSI_carl_tol_t *p)
{

 FILE *pFp;

  if (NULL == (pFp = fopen ("para.txt", "w+"))) {
    perror ("HSI carl parameters: unable to create file");
    return (-1);		/* failure */
  }

  fprintf (pFp, "%hu %hu %hu %hu %hu %hu \n", p->h, p->s, p->i, p->h_tol,p->s_tol, p->i_tol);
    
  if (0 > fclose (pFp)) {
    perror ("file close error");
    return (-1);		/* failure */
  }    
return(0);
}



int load_sample_hsi_carl_params(vlHSI_carl_tol_t *p)
{

  FILE *pFp;

  if (NULL == (pFp = fopen ("para.txt", "r"))) {
    perror ("HSI carl parameters: unable to create file");
    return (-1);		/* failure */
  }

    fscanf (pFp, "%hu %hu %hu %hu %hu %hu ", &(p->h), &(p->s), &(p->i),
		  &(p->h_tol), &(p->s_tol), &(p->i_tol));
    
  if (0 > fclose (pFp)) {
    perror ("file close error");
    return (-1);		/* failure */
  }    
return (0);
}

/*
下面给出堆栈的实现——为了改进递归寻找所有的blobs
*/

int initstack(sqstack *s)
{
	s->base=(int *)malloc(init_size *sizeof(int));
    if(!s->base) 
		exit(0);
    s->top=0;
    s->stacksize= init_size;
    return 1;
}

int destroystack(sqstack *s)
{
	if(s->base)
		free(s->base);
	return 0;
}

int push(sqstack *s,int e)
{
    if(s->top >= s->stacksize)
	{
		s->base=(int *)realloc(s->base,(s->stacksize+incrment)*sizeof(int));
		if(!s->base) 
		{
			printf("stack is reallocaed error.\n.");
			exit(0);
		}
		s->top=s->stacksize;
		s->stacksize+=incrment;
    }

	s->base[s->top++]= e;
    return 1;
}

int pop(sqstack *s,int *e)
{
	if(s->top==0) 
		return 0;
	--s->top;
	*e=s->base[s->top];
	return 1;
}






