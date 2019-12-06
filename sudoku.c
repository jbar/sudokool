#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "sudoku.h"
/*#include <curses.h>*/


#ifndef useMALLOC

#define LBASE (lBASE)
#define CBASE (cBASE)
#define SIZE (lBASE*cBASE)
#define NMAX (nMAX(lBASE,cBASE))
#define FMASK (fMASK(lBASE,cBASE))

#define TAB(i,j)  tab[(i)][(j)]
#define NBC(i,j)  nbc[(i)][(j)]

#else

static unsigned char LBASE=lBASE;
static unsigned char CBASE=cBASE;
static unsigned char SIZE=lBASE*cBASE;
static cands_t NMAX=nMAX(lBASE,cBASE);
static cands_t FMASK=fMASK(lBASE,cBASE);

#define TAB(i,j)  tab[i*SIZE+j]
#define NBC(i,j)  nbc[i*SIZE+j]

#endif /* DYNAMIC */

/*! search in the line of a case if a given number may be a candidate.
 *
 * \return 0 if number is not a candidate, or the number of cases were it is also a candidate on the line, 1 means that it may be the good candidate for the given case.
 */ 
unsigned char iscandidateinlin(t_sdk * Grid, cands_t number, unsigned char line, unsigned char col)
{
    unsigned char j,r=1;
    for(j=0;j<SIZE;j++) {
        if ( j!=col ) {
            //if (number==*(Grid->tab+line*SIZE+j)) return 0;
            if (number==Grid->TAB(line,j)) return 0;
            if (number & Grid->TAB(line,j)) r++;
        }
    }
    return r;
}

/*! search in the colon of a case if a given number may be a candidate.
 *
 * \return 0 if number is not a candidate, or the number of cases were it is also a candidate on the colon, 1 means that it may be the good candidate for the given case.
 */ 
unsigned char iscandidateincol(t_sdk * Grid, cands_t number, unsigned char line, unsigned char col)
{
    unsigned char i,r=1;
    for(i=0;i<SIZE;i++) {
        if ( i!=line ) {
            if (number==Grid->TAB(i,col)) return 0;
            if (number & Grid->TAB(i,col)) r++;
        }
    }
    return r;
}

/*! search in the region of a case if a given number may be a candidate.
 *
 * \return 0 if number is not a candidate, The number of cases were it is a candidate on the colon, 1 means that it may be the good candidate for the given case.
 */ 
unsigned char iscandidateinreg(t_sdk * Grid, cands_t number, unsigned char line, unsigned char col)
{
    unsigned char i,j,r=1,di,dj;
    di=(line/LBASE)*LBASE;
    dj=(col/CBASE)*CBASE;
    for(i=di;i<(di+LBASE);i++)
        for(j=dj;j<(dj+CBASE);j++) {
            if ( i!=line || j!=col ) {
                if (number==Grid->TAB(i,j)) return 0;
                if (number & Grid->TAB(i,j)) r++;
            }
    }
    return r;
}

void eliminateinreg_singleton(t_sdk * Grid, unsigned char di, unsigned char dj) {
    unsigned char i,j,k,l;

    for(i=di;i<(di+LBASE);i++)
        for(j=dj;j<(dj+CBASE);j++) {
            if  (Grid->NBC(i,j)==1) {
                for(k=di;k<(di+LBASE);k++)
                    for(l=dj;l<(dj+CBASE);l++) {
                        if ( i!=k || j!=l ) {
                        /*if (Grid->TAB(line,col)&Grid->TAB(line,j)){
                            Grid->TAB(line,col)&=~Grid->TAB(line,j);
                            Grid->NBC(line,col)--;}*/
                        Grid->TAB(k,l)&=~Grid->TAB(i,j);
						}
                }
            }
        }
}

void eliminateinline_singleton(t_sdk * Grid, unsigned char line) {
    unsigned char j,k;

    for(j=0;j<SIZE;j++) {
		if  (Grid->NBC(line,j)==1) {
			for (k=0;k<SIZE;k++) {
				if (k!=j) {
					/*if (Grid->TAB(line,col)&Grid->TAB(line,j)){
						Grid->TAB(line,col)&=~Grid->TAB(line,j);
						Grid->NBC(line,col)--;}*/
					Grid->TAB(line,k)&=~Grid->TAB(line,j);
				}
			}
		}
	}
}

unsigned char count_cands(cands_t cand) {
    unsigned char ret=0;
	int n;
    for (n=NMAX;n;n>>=1) if ( n & cand) ret++;
    return ret;
}

unsigned char eliminateinline(char setsize, t_sdk * Grid, unsigned char line) {
    unsigned char j,k,r;
	cands_t mask;

    for(j=0;j<SIZE;j++) {
		if  (Grid->NBC(line,j)==setsize) {
			r=0; mask=0;
			for(k=0,r=0;k<SIZE;k++) {
				if (Grid->TAB(line,k)|Grid->TAB(line,j)==Grid->TAB(line,j)) {
					mask|=(1<<k);
					r++;
				}
			}
			if (r>=setsize) {
				//for (n=NMAX;n;n>>=1) if ( n & mask) {
				for (k=0;k<SIZE;k++) 
					if ( (1<<k) & ~mask)
						Grid->TAB(line,k)&=~Grid->TAB(line,j);
			}
			/*else if (r>setsize)
 				//SUDOKU INVALIDE !!
				return -1;*/
        }
    }
    return r;
}


/*! Update candidate numbers in a given case.
 *
 * \return The negative number of candidate numbers for this case :  0 if sudoku is invalid, or if we asked for an already know case, -1 if the number has been fixed.
 */
char fillpos(t_sdk * pTab, unsigned char line, unsigned char col)
{
    cands_t n;

   /* if ( pTab->NBC(line,col) >= -1 ) 
        return pTab->NBC(line,col);*/
    
    for (n=NMAX;n;n>>=1) if ( n & pTab->TAB(line,col) ) {
        if ( (iscandidateinlin(pTab,n,line,col)==0) || (iscandidateincol(pTab,n,line,col)==0) || (iscandidateinreg(pTab,n,line,col)==0) ) {
            pTab->TAB(line,col)&=~n;
            if (pTab->TAB(line,col) == 0 ) return 0;
            pTab->NBC(line,col)++;
            //printf(" %4x",(int)n);
        }
    }
    return pTab->NBC(line,col);
}


/*! Update all candidate numbers in the Grid, starting from a given position.
 *
 * \return -1 if sudoku is invalid, or the number of new cases which have only one candidate.
 */
short fillposs(t_sdk * Grid,unsigned char line,unsigned char col)
{
    unsigned char i,j,k,l;
    short r=0;

   /* for (i=0;i<(SIZE);i++) 
        for (j=0;j<(SIZE);j++) */
    for (i=line;i<(SIZE+line);i++) 
        for (j=col;j<(SIZE+col);j++) {
            k=(i>=SIZE?i-SIZE:i);
            l=(j>=SIZE?j-SIZE:j);
            if ( Grid->NBC(k,l) < -1 ) {
                //printf("\nElimination candidats de line=%d, col=%d : ",i%SIZE+1,j%SIZE+1);
                switch (fillpos(Grid,k,l)) {
                    case 0 :
                        printf("Erreur ligne=%d col=%d, sudoku invalide !\n",k+1,l+1);
                        return -1;
                    case -1 :
                        Grid->lvl+=SIZE;
                        r++;
                }
            }
        }
    return r;            
}

/*! Say if a number is unique in all candidates on his line, colon or region.
 *
 * \return  1 if the number is unique, 0 if not.
 */
unsigned char isunique(t_sdk * Tab,cands_t number, unsigned char line, unsigned char col)
{
    if ( iscandidateinlin(Tab,number,line,col)==1 ) {
        //printf("le %x possible en %d %d est unique dans sa ligne\n",(int)number,line+1,col+1);
        return 1;}
    if ( iscandidateincol(Tab,number,line,col)==1 ) {
        //printf("le %x possible en %d %d est unique dans sa colonne\n",(int)number,line+1,col+1);
        return 1;}
    if ( iscandidateinreg(Tab,number,line,col)==1 ) {
        //printf("le %x possible en %d %d est unique dans sa region\n",(int)number,line+1,col+1);
        return 1;}
    //printf("le %x possible en %d %d n'est pas unique\n",(int)number,line+1,col+1);
    return 0;                    
} 

/*! advance elimination of candidates in a given region
 *
 */
unsigned char areunique(t_sdk * Tab,cands_t number, unsigned char line, unsigned char col)
{
    unsigned char i,j,ri=0,rj=0,r=0,di,dj;
    di=(line/LBASE)*LBASE;
    dj=(col/CBASE)*CBASE;
    for(i=di;i<(di+LBASE);i++) {
        if (number & Tab->TAB(i,j)) ri++;
    }
    if ( iscandidateinlin(Tab,number,line,col)==ri ) {
        //printf("les %x possibles en %d %d sont uniques/obligatoires (%d) dans leur ligne\n",(int)number,line+1,col+1,ri);
        if (ri==1) return 1;
        else
            r=ri;
    }
    // TO FINISH (...)
    return r;
    

}


/*! Search all numbers which are unique in all candidates on his line, colon or region in all the sudoku.
 *
 * \return  <1 if no unique found (-number of hyp needed), 0 if the sudoku is completed, or the number of uniques found
 */
short getuniques(t_sdk * pTab, unsigned char * line, unsigned char * col)
{
    unsigned char i,j;
    cands_t n;
    short nbcands=-512,nbu=0;
    
    for (i=0;i<SIZE;i++) 
        for (j=0;j<SIZE;j++) 
            if ( pTab->NBC(i,j) < -1 )
                for (n=NMAX;n;n>>=1) if ( n & pTab->TAB(i,j) ) {
                             if ( isunique(pTab,n,i,j) ) { 
                            pTab->lvl+=2*(SIZE+pTab->NBC(i,j));
                            pTab->TAB(i,j)=n;
                            pTab->NBC(i,j)=-1;
                            *line=i;
                            *col=j;
                            nbu++;
                        }
                        if ( pTab->NBC(i,j) > nbcands ) {
                            nbcands=pTab->NBC(i,j);
                            *line=i;
                            *col=j;
                        }
                    }
    return (nbu>0?nbu:(nbcands==-512?0:nbcands));
}

struct inrecursion {
    short r;
    int ibuff;
    int k;
    unsigned char i;
    unsigned char j;
    cands_t n;
};

static int wbuff=0;

/*! The solve function without any parameters or variables inside for a fastest recursivity
 * \return -1 if reach memory limit , the lvl of the sudoku or the negative numbers of solutions of the sudoku.
 */
/*int solve(void)
{

}*/



/*! solve a given sudoku
 * \return -1 if reach memory limit , the lvl of the sudoku or the negative numbers of solutions of the sudoku.
 */
int solve(t_sdk * buff, int * nbgen, int buffsize)
{
    short r;
    //static unsigned int s=1;
    int ibuff,k;
    unsigned char i=0,j=0;
    cands_t n;

    ibuff=wbuff;
    do {
        if ( fillposs(&buff[wbuff],i,j) == -1 ) return 0;  
//        print_grid(&buff[wbuff],-1,FIXbyPERM);
        r=getuniques(&buff[wbuff],&i,&j);
        if (r<0) {
            print_grid(&buff[wbuff],-1,FIXbyPERM);
///        for (k=0;k<=wbuff;k++) printf("*");
///        printf("\n");
//if ( VerboseLvl > 0 )
            
                //sleep(1);
            
            for (n=NMAX;n;n>>=1) if ( n & buff[ibuff].TAB(i,j) ) {
                    wbuff++;
                    printf("hyp %3X sur %d (wb=%3d) in line=%d, col=%d\n",n,-r,wbuff,i+1,j+1);
                    if (wbuff >= buffsize) {
                        buff[ibuff].lvl=-1;
                        wbuff--;
                        return -1;
                    }
                    memcpy(&buff[wbuff],&buff[ibuff],sizeof(t_sdk));
                    buff[wbuff].TAB(i,j)=n;
                    buff[wbuff].NBC(i,j)=FIXbyHYP;
                    k=solve(buff,nbgen,buffsize);
                    if (k == 0) wbuff--;
///                    else if (k > 0) 
///                        printf("hyp %3X sur %d (wb=%3d) in line=%d, col=%d --> score=%d\n",n,-r,wbuff,i+1,j+1,k);
                   /*{
                        printf("                    +++++++++++++++++++++\n                           ++++++++++++++++\n");
//                        break;
                    }*/

        /*                case 0:
                            wbuff--;
                            break;
                        case -1:
                            n=1;
                            break;*/
            }
        if ( buff[ibuff].lvl != -1) switch (wbuff-ibuff) {
            case 0 :
                buff[ibuff].TAB(i,j)=0;
                buff[ibuff].NBC(i,j)=0;
                return 0;
            case 1 :
                memcpy(&buff[ibuff],&buff[wbuff],sizeof(t_sdk));
                wbuff--;
                buff[ibuff].NBC(i,j)=(1-r)/2;
                buff[ibuff].lvl+=SIZE*SIZE*(-r);
                break;
            default :
                buff[ibuff].lvl=0;
                for (k=ibuff+1;k<=wbuff;k++)
                    if (buff[k].lvl > 0) buff[ibuff].lvl--;
            }
        }
    } while (r>0);
    *nbgen=wbuff+1;
    //print_grid(&buff[wbuff]);
    //printf("  lvl = %d\n",buff[wbuff].lvl);
    /*if (s < maxgrids) 
        memcpy(&grids[s],&buff[ibuff],sizeof(t_sdk));*/
    return (buff[ibuff].lvl);
}

int f_2xminfixednumber(unsigned char lbase, unsigned char cbase)
{
    int r=0,i;
    for (i=lbase;i<lbase*cbase;i+=lbase) 
        r+=i;
    r*=lbase;
    if (lbase==cbase)
        r*=2;
    else
        for (i-=cbase;i>0;i-=cbase)
            r+=i*cbase;
    return (lbase*cbase*lbase*cbase-r);
}

/*! Clean a solved sudoku (to solve it again for example).
 * \return the number of case cleaned.
 */
int remove_solutions(t_sdk * grid)
{
    unsigned char i,j;
    cands_t def=FMASK;
    int r=0;

    /*for (int c=0;c<SIZE;c++)
        def|=0x01<<c;*/

    for (i=0;i<SIZE;i++) 
        for (j=0;j<SIZE;j++) 
            if (grid->NBC(i,j)<FIXbyHYP) {
                grid->NBC(i,j)=-SIZE;
                grid->TAB(i,j)=FMASK;
                r++;
            }
    grid->lvl=0;

    return r;
}


int minimalize(t_sdk * grid,int buffsize)
{
    unsigned char i,j;
    short lvl;
    char snbc;
    cands_t scand;
    int r=0;


    for (i=0;i<SIZE;i++) 
        for (j=0;j<SIZE;j++) 
            if (grid->NBC(i,j)>=FIXbyHYP) {
                snbc=grid->NBC(i,j);
                scand=grid->TAB(i,j);
                lvl=grid->lvl;
                grid->NBC(i,j)=-SIZE;
                remove_solutions(grid);
                wbuff=0;
                if (solve(grid,&r,buffsize)<0) {
                    grid->NBC(i,j)=snbc;
                    grid->TAB(i,j)=scand;
                    grid->lvl=lvl;
                }
                //else r++;
            }
    return r;
}

/*! Initialize a random sudoku grid
 * \param Grid The grid to init.
 * \param nbtofix Number of cases to fix in the grid. 
 * \return 1 if it successfully placed MAX(nbtofix,SIZE*SIZE-1). 0 If the grid is already wrong.
 */
int initnewgrid(t_sdk * newgrid,unsigned short nbtofix)
{
    unsigned char i,j,n;
    unsigned short c;
    cands_t def=0,def2=0;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    srand(tv.tv_usec+tv.tv_sec);

    /*for (c=0;c<SIZE;c++)
        def|=0x01<<c;*/
    for (i=0;i<SIZE;i++) 
        for (j=0;j<SIZE;j++) {
            newgrid->TAB(i,j)=FMASK;
            newgrid->NBC(i,j)=-SIZE;
        }
    def=0;

    if ( nbtofix >= SIZE*SIZE ) nbtofix=SIZE*SIZE-1;

    for (c=0;c<nbtofix;c++) {
        i=(rand()%SIZE);
        j=(rand()%SIZE);
        if (c < SIZE-1) {
            while ((0x01<<i) & def) i=(i+1)%SIZE ;
            while ((0x01<<j) & def2) j=(j+1)%SIZE ;
            n=c;
        } else
            n=(rand()%SIZE);

        def|=(0x01<<i);
        def2|=(0x01<<j);

        switch (fillpos(newgrid,i,j)) {
            case 1:
                c--;
                break;
            case 0:
                return 0;
            default:
                while ((0x01<<n) & ~newgrid->TAB(i,j)) n=(n+1)%SIZE ;
                newgrid->TAB(i,j)=(0x01<<n);
                newgrid->NBC(i,j)=FIXatSTART;
        }
    }
    return 1;
}

/*! Read a sudoku file and fill a t_sdk structure */
int readfile (t_sdk * Tab,char *sdfile)
{
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
    FILE *fp ;
    unsigned char i,j;
    cands_t def=FMASK;
    int c;

    if((fp = fopen(sdfile,"r"))==NULL) return -1;

    /*for (c=0;c<SIZE;c++)
        def|=0x01<<c;*/
    for (i=0;i<SIZE;i++) {
        for (j=0;j<SIZE;j++) {
            /*do {
                c=fgetc(fp);
                if (c==EOF) c=' ';
            } while  ( (c < '0' || c > '0'+SIZE ) && c !=' ' );*/ 
            while (1) {
                c=fgetc(fp);
                if ( c > '0' && c <= MIN('0'+SIZE,'9') ) {
                    Tab->TAB(i,j)=0x01<<(c-'1');
                    Tab->NBC(i,j)=FIXatSTART;
                    break;
                } else if ( c >= 'a' && c < 'a'+SIZE-9 ) {
                    Tab->TAB(i,j)=0x01<<(c+9-'a');
                    Tab->NBC(i,j)=FIXatSTART;
                    break;
                } else if ( c==' ' || c=='0' || c==EOF ) {
                    Tab->TAB(i,j)=FMASK;
                    Tab->NBC(i,j)=-SIZE;
                    break;
                }
            }
        }
    }
    fclose(fp);
    Tab->lvl=0;
    return 1;
}

/*! Print a sudoku grid
 * \param Grid The grid to print
 * \param Max number of candidates to print the case : 1 print the solved sudoku, 0 print the sudoku to resolve. 
 * \return nothing.
 */
void print_grid(t_sdk * Grid, char nbcmin, char nbcmax)
{
    unsigned char i,j,n;

/*    for (i=0;i<SIZE;i++) {
        printf("\n");
        if (i%LBASE)
            for (n=0;n<SIZE*4+LBASE;n++) printf("-");
        else
            for (n=0;n<SIZE*4+LBASE;n++) printf("=");
        printf("\n");
        for (j=0;j<SIZE;j++) {
            //printf(" %d",Grid->NBC(i,j));
            if (!(j%CBASE)) printf("|");
            if (Grid->TAB(i,j) == 0 )
                printf(" X |");
            else if (Grid->NBC(i,j) >= nbcmin && Grid->NBC(i,j) <= nbcmax ) {
                for (n=0;(Grid->TAB(i,j) != (0x01<<n) && (n<SIZE));n++) ;
                printf("%2d |",n+1);
            }
            else printf("   |");
        }
    }*/

    for (i=0;i<SIZE;i++) {
        if (!(i%LBASE)) {
            for (n=0;n<SIZE+LBASE;n++) printf("-");
            printf("\n");
        }
        for (j=0;j<SIZE;j++) {
            if (!(j%CBASE)) printf("|");
            if ( Grid->TAB(i,j) == 0 )
                printf("X");
            else if ((Grid->NBC(i,j) >= nbcmin) && (Grid->NBC(i,j) <= nbcmax) ) {
                for (n=0;(Grid->TAB(i,j) != (0x01<<n) && (n<SIZE));n++) ;
                printf("%c",(n<9?n+'1':n-9+'a'));
            }
            else printf(" ");
        }
        printf("\n");
    }
    for (n=0;n<SIZE+LBASE;n++) printf("-");
    printf("\n");
}


