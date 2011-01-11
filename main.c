#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"


int main (int argc, char **argv)
{
    t_sdk * grids, * grids2;
    int r;
    int nbgrids,buffsize=300,bsize2=50;

    grids=malloc(buffsize*sizeof(t_sdk));
    grids2=malloc(bsize2*sizeof(t_sdk));

    readfile(&grids[0],argv[1]);
    /*if (! initnewgrid(&grids[0],atoi(argv[1]))) {
	printf("\n Error generating a random grid, u may have ask for too many numbers ! \n");
	return -1;
    }*/
    print_grid(&grids[0],FIXbyHYP,FIXbyPERM);

/*    do {
	printf(" quitter (q) ? ");
    } while (getchar()!='q');*/
    grids[0].lvl=0;
    r=solve(grids,&nbgrids,buffsize);
    /*print_grid(&grids[0],FIXbyHYP,FIXbyPERM);
    print_grid(&grids[0],-1,FIXbyPLAYER);*/
    printf("lvl= %d \n",grids[0].lvl);
    printf("\n nbgrids = %d   nbsol = %d\n",nbgrids,r);
    //print_grid(&grids[0],-10,FIXbyPLAYER);
    for (r=0;r<(nbgrids<buffsize?nbgrids:buffsize);r++) {
    	printf("lvl= %4d  %3d\n",grids[r].lvl,r);
	if ( grids[r].lvl> lBASE*cBASE*4 ) {
	    //print_grid(&grids[r],FIXbyHYP,FIXbyPERM);
	    print_grid(&grids[r],-10,FIXbyPLAYER);
	    //printf(" ==>\n ");
	    memcpy(&grids2[0],&grids[r],sizeof(t_sdk));
	    minimalize(grids2,bsize2);
	    print_grid(grids2,FIXbyHYP,FIXbyPERM);
	    printf("#### lvl= %d ####\n\n",grids2->lvl);
	    
	}
    }
//    printf("sizeof(t_sdk)=%d",sizeof(t_sdk));
    	
    //for (n=NMAX;n;n>>=1) printf("    %x\n",(int)n);

    //return (grids[0].lvl);
    return 0;
}
