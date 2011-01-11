/*
  sudoku.h : header file for sudoku librairy and applications.
*/

#define FIXbyHYP    65
#define FIXatSTART  66
#define FIXbyPERM   67
#define FIXbyPLAYER 127

#ifndef lBASE
#define lBASE 3
#endif
#ifndef cBASE
#define cBASE 3
#endif

#ifndef useMALLOC /* static size */

 #if ( lBASE*cBASE < 2 )
  #error Sudoku is too small
 #elif ( lBASE*cBASE <= 8 )
typedef unsigned char cands_t;
 #elif ( lBASE*cBASE <= 16 )
typedef unsigned short cands_t;
 #elif ( lBASE*cBASE <= 32 )
  #ifdef _XBS5_LP64_OFF64
typedef unsigned int cands_t;
  #else
typedef unsigned long cands_t;
  #endif /* _XBS5_LP64_OFF64 */
 #elif ( lBASE*cBASE <= 64 )
  #ifdef _XBS5_LP64_OFF64
typedef unsigned long cands_t;
  #else
   #error Sudoku is too big
  #endif /* _XBS5_LP64_OFF64 */
 #else 
  #error Sudoku is too big
 #endif /* lBASE*cBASE */

typedef struct t_sdk {
    cands_t tab[lBASE*cBASE][lBASE*cBASE];
    char nbc[lBASE*cBASE][lBASE*cBASE];
    short lvl;
} t_sdk ;

#else /* dynamic size */

typedef unsigned long cands_t;

typedef struct t_sdk {
    cands_t * tab;
    char * nbc;
    short lvl;
} t_sdk ;

#endif /* static or dynamic size */

/*! Correspond to the highest number in the sudoku */
#define nMAX(lbase,cbase) ( 0x01 << ((lbase)*(cbase)-1) )

/*! Correspond to the fullmask of a cand_t nimber in the sudoku */
#define fMASK(lbase,cbase) ( (0x01 << (lbase)*(cbase))-1)

/*! Give twice a minimal number of cases to fix to start generating a sudoku */
#define twice_MIN_NUM(lbase,cbase) ((lbase)*(cbase)*((lbase)+(cbase))/2)

unsigned char sdku_getsize(unsigned char * lbase,unsigned char * cbase);
unsigned char sdku_setsize(unsigned char lbase,unsigned char cbase);

char fillpos(t_sdk * pTab, unsigned char line, unsigned char col);
short fillposs(t_sdk * Grid,unsigned char line,unsigned char col);
unsigned char isunique(t_sdk * Tab,cands_t number, unsigned char line, unsigned char col);
short getuniques(t_sdk * pTab, unsigned char * line, unsigned char * col);
//unsigned int solve(t_sdk * buff, unsigned short * nbgen, unsigned short buffsize);
int solve(t_sdk * buff, int * nbgen, int buffsize);

int remove_solutions(t_sdk * grid);
int minimalize(t_sdk * grid,int buffsize);
int initnewgrid(t_sdk * newgrid,unsigned short nbtofix);

int readfile (t_sdk * pTab, char *sdfile);
void print_grid(t_sdk * Grid, char nbcmin, char nbcmax);

