/********************************************************************************
*						      			       				   
* 				Susan				                                
*		Parallel 1.0 Version with Pthread
*															
* Authors:           	Yuchen Liang and Syed Muhammad Zeeshan Iqbal
* Supervisor:		Hakan Grahn	 						
* Section:          	Computer Science of BTH,Sweden
* GCC Version:		4.2.4								
* Environment: 		Kraken(Ubuntu4.2.4) with Most 8 Processors 				
* Start Date:       	15th September 2009					
* End Date:         	6th October 2009			
*		
*********************************************************************************/

/* {{{ Copyright etc. */

/**********************************************************************\

  SUSAN Version 2l by Stephen Smith
  Oxford Centre for Functional Magnetic Resonance Imaging of the Brain,
  Department of Clinical Neurology, Oxford University, Oxford, UK
  (Previously in Computer Vision and Image Processing Group - now
  Computer Vision and Electro Optics Group - DERA Chertsey, UK)
  Email:    steve@fmrib.ox.ac.uk
  WWW:      http://www.fmrib.ox.ac.uk/~steve

  (C) Crown Copyright (1995-1999), Defence Evaluation and Research Agency,
  Farnborough, Hampshire, GU14 6TD, UK
  DERA WWW site:
  http://www.dera.gov.uk/
  DERA Computer Vision and Electro Optics Group WWW site:
  http://www.dera.gov.uk/imageprocessing/dera/group_home.html
  DERA Computer Vision and Electro Optics Group point of contact:
  Dr. John Savage, jtsavage@dera.gov.uk, +44 1344 633203

  A UK patent has been granted: "Method for digitally processing
  images to determine the position of edges and/or corners therein for
  guidance of unmanned vehicle", UK Patent 2272285. Proprietor:
  Secretary of State for Defence, UK. 15 January 1997

  This code is issued for research purposes only and remains the
  property of the UK Secretary of State for Defence. This code must
  not be passed on without this header information being kept
  intact. This code must not be sold.

\**********************************************************************/

/* }}} */
/* {{{ Readme First */

/**********************************************************************\

  SUSAN Version 2l
  SUSAN = Smallest Univalue Segment Assimilating Nucleus

  Email:    steve@fmrib.ox.ac.uk
  WWW:      http://www.fmrib.ox.ac.uk/~steve

  Related paper:
  @article{Smith97,
        author = "Smith, S.M. and Brady, J.M.",
        title = "{SUSAN} - A New Approach to Low Level Image Processing",
        journal = "Int. Journal of Computer Vision",
        pages = "45--78",
        volume = "23",
        number = "1",
        month = "May",
        year = 1997}

  To be registered for automatic (bug) updates of SUSAN, send an email.

  Compile with:
  gcc -O4 -o susan susan2l.c -lm

  See following section for different machine information. Please
  report any bugs (and fixes). There are a few optional changes that
  can be made in the "defines" section which follows shortly.

  Usage: type "susan" to get usage. Only PGM format files can be input
  and output. Utilities such as the netpbm package and XV can be used
  to convert to and from other formats. Any size of image can be
  processed.

  This code is written using an emacs folding mode, making moving
  around the different sections very easy. This is why there are
  various marks within comments and why comments are indented.


  SUSAN QUICK:

  This version of the SUSAN corner finder does not do all the
  false-corner suppression and thus is faster and produced some false
  positives, particularly on strong edges. However, because there are
  less stages involving thresholds etc., the corners that are
  correctly reported are usually more stable than those reported with
  the full algorithm. Thus I recommend at least TRYING this algorithm
  for applications where stability is important, e.g., tracking.

  THRESHOLDS:

  There are two thresholds which can be set at run-time. These are the
  brightness threshold (t) and the distance threshold (d).

  SPATIAL CONTROL: d

  In SUSAN smoothing d controls the size of the Gaussian mask; its
  default is 4.0. Increasing d gives more smoothing. In edge finding,
  a fixed flat mask is used, either 37 pixels arranged in a "circle"
  (default), or a 3 by 3 mask which gives finer detail. In corner
  finding, only the larger 37 pixel mask is used; d is not
  variable. In smoothing, the flat 3 by 3 mask can be used instead of
  a larger Gaussian mask; this gives low smoothing and fast operation.

  BRIGHTNESS CONTROL: t

  In all three algorithms, t can be varied (default=20); this is the
  main threshold to be varied. It determines the maximum difference in
  greylevels between two pixels which allows them to be considered
  part of the same "region" in the image. Thus it can be reduced to
  give more edges or corners, i.e. to be more sensitive, and vice
  versa. In smoothing, reducing t gives less smoothing, and vice
  versa. Set t=10 for the test image available from the SUSAN web
  page.

  ITERATIONS:

  With SUSAN smoothing, more smoothing can also be obtained by
  iterating the algorithm several times. This has a different effect
  from varying d or t.

  FIXED MASKS:

  37 pixel mask:    ooo       3 by 3 mask:  ooo
                   ooooo                    ooo
                  ooooooo                   ooo
                  ooooooo
                  ooooooo
                   ooooo
                    ooo

  CORNER ATTRIBUTES dx, dy and I
  (Only read this if you are interested in the C implementation or in
  using corner attributes, e.g., for corner matching)

  Corners reported in the corner list have attributes associated with
  them as well as positions. This is useful, for example, when
  attempting to match corners from one image to another, as these
  attributes can often be fairly unchanged between images. The
  attributes are dx, dy and I. I is the value of image brightness at
  the position of the corner. In the case of susan_corners_quick, dx
  and dy are the first order derivatives (differentials) of the image
  brightness in the x and y directions respectively, at the position
  of the corner. In the case of normal susan corner finding, dx and dy
  are scaled versions of the position of the centre of gravity of the
  USAN with respect to the centre pixel (nucleus).

  BRIGHTNESS FUNCTION LUT IMPLEMENTATION:
  (Only read this if you are interested in the C implementation)

  The SUSAN brightness function is implemented as a LUT
  (Look-Up-Table) for speed. The resulting pointer-based code is a
  little hard to follow, so here is a brief explanation. In
  setup_brightness_lut() the LUT is setup. This mallocs enough space
  for *bp and then repositions the pointer to the centre of the
  malloced space. The SUSAN function e^-(x^6) or e^-(x^2) is
  calculated and converted to a uchar in the range 0-100, for all
  possible image brightness differences (including negative
  ones). Thus bp[23] is the output for a brightness difference of 23
  greylevels. In the SUSAN algorithms this LUT is used as follows:

  p=in + (i-3)*x_size + j - 1;
  p points to the first image pixel in the circular mask surrounding
  point (x,y).

  cp=bp + in[i*x_size+j];
  cp points to a position in the LUT corresponding to the brightness
  of the centre pixel (x,y).

  now for every pixel within the mask surrounding (x,y),
  n+=*(cp-*p++);
  the brightness difference function is found by moving the cp pointer
  down by an amount equal to the value of the pixel pointed to by p,
  thus subtracting the two brightness values and performing the
  exponential function. This value is added to n, the running USAN
  area.

  in SUSAN smoothing, the variable height mask is implemented by
  multiplying the above by the moving mask pointer, reset for each new
  centre pixel.
  tmp = *dpt++ * *(cp-brightness);

\**********************************************************************/

/* }}} */
/* {{{ Machine Information */

/**********************************************************************\

  Success has been reported with the following:

  MACHINE  OS         COMPILER

  Sun      4.1.4      bundled C, gcc

  Next

  SGI      IRIX       SGI cc

  DEC      Unix V3.2+ 

  IBM RISC AIX        gcc

  PC                  Borland 5.0

  PC       Linux      gcc-2.6.3

  PC       Win32      Visual C++ 4.0 (Console Application)

  PC       Win95      Visual C++ 5.0 (Console Application)
                      Thanks to Niu Yongsheng <niuysbit@163.net>:
                      Use the FOPENB option below

  PC       DOS        djgpp gnu C
                      Thanks to Mark Pettovello <mpettove@umdsun2.umd.umich.edu>:
                      Use the FOPENB option below

  HP       HP-UX      bundled cc
                      Thanks to Brian Dixon <briand@hpcvsgen.cv.hp.com>:
                      in ksh:
                      export CCOPTS="-Aa -D_HPUX_SOURCE | -lM"
                      cc -O3 -o susan susan2l.c

\**********************************************************************/

/* }}} */
/* {{{ History */

/**********************************************************************\

  SUSAN Version 2l, 12/2/99
  Changed GNUDOS option to FOPENB.
  (Thanks to Niu Yongsheng <niuysbit@163.net>.)
  Took out redundant "sq=sq/2;".

  SUSAN Version 2k, 19/8/98:
  In corner finding:
  Changed if(yy<sq) {...} else if(xx<sq) {...} to
          if(yy<xx) {...} else {...}
  (Thanks to adq@cim.mcgill.edu - Alain Domercq.)

  SUSAN Version 2j, 22/10/97:
  Fixed (mask_size>x_size) etc. tests in smoothing.
  Added a couple of free() calls for cgx and cgy.
  (Thanks to geoffb@ucs.ed.ac.uk - Geoff Browitt.)

  SUSAN Version 2i, 21/7/97:
  Added information about corner attributes.

  SUSAN Version 2h, 16/12/96:
  Added principle (initial enhancement) option.

  SUSAN Version 2g, 2/7/96:
  Minor superficial changes to code.

  SUSAN Version 2f, 16/1/96:
  Added GNUDOS option (now called FOPENB; see options below).

  SUSAN Version 2e, 9/1/96:
  Added -b option.
  Fixed 1 pixel horizontal offset error for drawing edges.

  SUSAN Version 2d, 27/11/95:
  Fixed loading of certain PGM files in get_image (again!)

  SUSAN Version 2c, 22/11/95:
  Fixed loading of certain PGM files in get_image.
  (Thanks to qu@San-Jose.ate.slb.com - Gongyuan Qu.)

  SUSAN Version 2b, 9/11/95:
  removed "z==" error in edges routines.

  SUSAN Version 2a, 6/11/95:
  Removed a few unnecessary variable declarations.
  Added different machine information.
  Changed "header" in get_image to char.

  SUSAN Version 2, 1/11/95: first combined version able to take any
  image sizes.

  SUSAN "Versions 1", circa 1992: the various SUSAN algorithms were
  developed during my doctorate within different programs and for
  fixed image sizes. The algorithms themselves are virtually unaltered
  between "versions 1" and the combined program, version 2.

\**********************************************************************/

/* }}} */
/* {{{ defines, includes and typedefs */

/* ********** Optional settings */

#ifndef PPC
typedef int        TOTAL_TYPE; /* this is faster for "int" but should be "float" for large d masks */
#else
typedef float      TOTAL_TYPE; /* for my PowerPC accelerator only */
#endif

/*#define FOPENB*/           /* uncomment if using djgpp gnu C for DOS or certain Win95 compilers */
#define SEVEN_SUPP           /* size for non-max corner suppression; SEVEN_SUPP or FIVE_SUPP */
#define MAX_CORNERS   150  /* max corners per frame */

/* ********** Leave the rest - but you may need to remove one or both of sys/file.h and malloc.h lines */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <sys/file.h>    /* may want to remove this line */
#include <malloc.h>      /* may want to remove this line */
#define  exit_error(IFB,IFC) { fprintf(stderr,IFB,IFC); exit(0); }
#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )
#define PROCESSORS				 2	//The amount of processor
typedef  unsigned char uchar;
typedef  struct {int x,y,info, dx, dy, I;} CORNER_LIST; //CORNER_LIST[MAX_CORNERS];	Notice:i modifiy the definition of CORNER_LIST
#include <stdint.h>
#include "input_small.h"
static uint8_t *fakeFile;

int argc = 4;
char *argv[] = {"susan", "input.pgm", "output.pgm", "-s"};
//char *argv[] = {"susan", "input.pgm", "output.pgm", "-e"};
//char *argv[] = {"susan", "input.pgm", "output.pgm", "-c"};

/* define a thread pool */
struct _THREADPOOL{
	int id;			/* thread id */
	pthread_t thread;		/* thread itself */
};
typedef struct _THREADPOOL THREADPOOL;
THREADPOOL threadPool[PROCESSORS]; 

struct _BARRIER{
	int count;
	pthread_mutex_t barrier_mutex;
	pthread_cond_t barrier_cond;
};
typedef struct _BARRIER BARRIER;
BARRIER myBarrier;

struct _FUNCARGS{
	/* setup_brightness_lut_thread() */ 
	uchar **bp;				
	int thresh;
	int form;
	/* susan_smoothing_thread()*/
	int three_by_three;		
	int mask_size;	
	float temp;
	uchar* dpt;
	int increment;
	int y_size;			
	int x_size;
	uchar *out;
	uchar *bp_smoothing;
	uchar *in;
	uchar *dp;	
	/* susan_principle_small_thread() and susan_principle_thread() */		
	uchar* bp_principle;		
	int *r;
	int max_no;
	/* int_to_uchar_thread() */		
	int size;				
	int* max_r; 
	int* min_r;	
	int max_r_global;
	int min_r_global;		
	/* susan_edges_small_thread() and susan_edges_thread()*/
	uchar* bp_edges;
	uchar* mid;
	/* susan_thin_thread() */
	/* edge_draw_thread */
	uchar* midp;
	int drawing_mode;
	uchar* inp;
	/* susan_corners_quick_thread() */
	uchar* bp_corners;
	CORNER_LIST* corner_list;
	int* ns;
	/* susan_corners_thread() */
	int* cgx;
	int* cgy;
	/* corner_draw_thread() */
	int n;
	int* stop_signal;
	pthread_mutex_t pt_mutex;
	/* enlarge_thread */
	uchar **in_enlarge; 
	uchar *tmp_image; 
	int *x_size_enlarge;
	int *y_size_enlarge;
	int border;
	
};
typedef struct _FUNCARGS FUNCARGS;

struct _THREADARGS{
	int id;			/* thread id */
	FUNCARGS* funcArgs;	/* function arguments */	
};
typedef struct _THREADARGS THREADARGS;

/* Forward declaring functions*/
void* setup_brightness_lut_thread(void*);		/* invoked by setup_brightness_lut() */
void* susan_smoothing_thread(void*);		/* invoked by susan_smoothing() */
void* susan_principle_small_thread(void*);	/* invoked by susan_principle_small() */
void* susan_principle_thread(void*);		/* invoked by susan_principle() */
void* int_to_uchar_thread(void*);			/* invoked by int_to_uchar() */
void* susan_edges_small_thread(void*);		/* invoked by susan_edges_small() */
void* susan_edges_thread(void*);			/* invoked by susan_edges() */
void* susan_thin_thread(void*);			/* invoked by susan_thin() */
void* edge_draw_thread(void*);			/* invoked by edge_draw() */
void* susan_corners_quick_thread(void*);		/* invoked by susan_corners_quick() */
void* susan_corners_thread(void*);			/* invoked by susan_corners() */
void* corner_draw_thread(void*);			/* invoked by corner_draw() */
void* enlarge_thread(void*);			/* invoked by enlarge() */


/* Intilize barrier */
void startBarrier(BARRIER* myBarrier){
	pthread_mutex_init(&(myBarrier->barrier_mutex), NULL);
	pthread_cond_init(&(myBarrier->barrier_cond), NULL);
	myBarrier->count = 0;
}


/* Actuate barrier */
void actuateBarrier(BARRIER* myBarrier){
	pthread_mutex_lock(&(myBarrier->barrier_mutex));
	myBarrier->count++;
	if (myBarrier->count!=PROCESSORS) {
		pthread_cond_wait(&(myBarrier->barrier_cond), &(myBarrier->barrier_mutex));
	}
	else{
		myBarrier->count=0;
		pthread_cond_broadcast(&(myBarrier->barrier_cond));
	}
	pthread_mutex_unlock(&(myBarrier->barrier_mutex));
}


/* Initialize a thread pool and create the threads */
void startThreads(int func, FUNCARGS* funcArgs){
	int i,pc;
	for(i=0;i<PROCESSORS;i++){
		pthread_t thread;
		THREADARGS* threadArgs = (THREADARGS*)malloc(sizeof(THREADARGS));
		threadArgs->id = i;
		threadArgs->funcArgs=funcArgs;
		switch(func){
			case 0:
				pc = pthread_create(&thread,NULL,setup_brightness_lut_thread,(void*)threadArgs);
				break;
			case 1:
				pc = pthread_create(&thread,NULL,susan_smoothing_thread,(void*)threadArgs);
				break;
			case 2:
				pc = pthread_create(&thread,NULL,susan_principle_small_thread,(void*)threadArgs);
				break;
			case 3:
				pc = pthread_create(&thread,NULL,susan_principle_thread,(void*)threadArgs);
				break;
			case 4:
				pc = pthread_create(&thread,NULL,int_to_uchar_thread,(void*)threadArgs);
				break;
			case 5:				
				pc = pthread_create(&thread,NULL,susan_edges_small_thread,(void*)threadArgs);
				break;
			case 6:				
				pc = pthread_create(&thread,NULL,susan_edges_thread,(void*)threadArgs);
				break;
			case 7:
				pc = pthread_create(&thread,NULL,susan_thin_thread,(void*)threadArgs);
				break;
			case 8: 
				pc = pthread_create(&thread,NULL,edge_draw_thread,(void*)threadArgs);
				break;
			case 9:
				pc = pthread_create(&thread,NULL,susan_corners_quick_thread,(void*)threadArgs);
				break;
			case 10:
				pc = pthread_create(&thread,NULL,susan_corners_thread,(void*)threadArgs);
				break;
			case 11:
				pc = pthread_create(&thread,NULL,corner_draw_thread,(void*)threadArgs);
				break;
			case 12:				
				pc = pthread_create(&thread,NULL,enlarge_thread,(void*)threadArgs);
				break;
		}
		if(pc != 0){
			printf("Fail to create threads!");
			exit(0);
		}else{
			threadPool[i].id = i;
			threadPool[i].thread = thread;
		}
  	}
  	for(i=0;i<PROCESSORS;i++){
      	pthread_join(threadPool[i].thread,NULL);
  	}
}



/* }}} */
/* {{{ usage() */
void usage(){
  printf("Usage: susan <in.pgm> <out.pgm> [options]\n\n");
  printf("-s : Smoothing mode (default)\n");
  printf("-e : Edges mode\n");
  printf("-c : Corners mode\n\n");
  printf("See source code for more information about setting the thresholds\n");
  printf("-t <thresh> : Brightness threshold, all modes (default=20)\n");
  printf("-d <thresh> : Distance threshold, smoothing mode, (default=4) (use next option instead for flat 3x3 mask)\n");
  printf("-3 : Use flat 3x3 mask, edges or smoothing mode\n");
  printf("-n : No post-processing on the binary edge map (runs much faster); edges mode\n");
  printf("-q : Use faster (and usually stabler) corner mode; edge-like corner suppression not carried out; corners mode\n");
  printf("-b : Mark corners/edges with single black points instead of black with white border; corners or edges mode\n");
  printf("-p : Output initial enhancement image only; corners or edges mode (default is edges mode)\n");
  printf("\nSUSAN Version 2l (C) 1995-1997 Stephen Smith, DRA UK. steve@fmrib.ox.ac.uk\n");
  exit(0);
}
/* }}} */


char fgetc2() {
    char ret = *fakeFile;
    ++fakeFile;
    return ret;
}

/* {{{ int getint(fp) derived from XV */

int getint()
{
  int c, i;
  //char dummy[10000];

  c = fgetc2();
  while (1) /* find next integer */
  {
    if (c=='#')    /* if we're at a comment, read to end of line */
        while(c!= '\n') c=fgetc2();
    if (c==EOF)
      exit_error("Image %s not binary PGM.\n","is");
    if (c>='0' && c<='9')
      break;   /* found what we were looking for */
    c = fgetc2();
  }

  /* we're at the start of a number, continue until we hit a non-number */
  i = 0;
  while (1) {
    i = (i*10) + (c - '0');
    c = fgetc2();
    if (c==EOF) return (i);
    if (c<'0' || c>'9') break;
  }

  return (i);
}

void get_image(filename,in,x_size,y_size) char filename[200]; unsigned char **in; int *x_size, *y_size;{
	char header [100];
	int  tmp;
  	/* {{{ read header */
  	header[0]=fgetc2();
  	header[1]=fgetc2();
  	if(!(header[0]=='P' && header[1]=='5')){
    		exit_error("Image %s does not have binary PGM header.\n",filename);
	}
  	*x_size = getint();
  	*y_size = getint();
  	tmp = getint();
	/* }}} */
	*in = (uchar *) fakeFile;
}
/* }}} */



/* {{{ put_image(filename,in,x_size,y_size) */
void put_image(in,x_size,y_size) {
    printf("P5\n");
    printf("%d %d\n",x_size,y_size);
    printf("255\n");
}
/* }}} */



/* {{{ int_to_uchar(r,in,size) */
void int_to_uchar(r,in,size) uchar *in; int   *r, size; {
	int max_r[PROCESSORS], min_r[PROCESSORS];
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->in = in;
	funcArgs->r = r;
	funcArgs->size = size;
	funcArgs->max_r = max_r;
	funcArgs->min_r = min_r;
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(4,funcArgs);
	free(funcArgs);
	/*##### Parallelization Work #####
  	for (i=0; i<size; i++){
		.............................................		
    	}
	*/
	/*##### Parallelization Work #####
  	for (i=0; i<size; i++){		
		.............................................
	}
	*/
}
/* }}} */
void* int_to_uchar_thread(void* threadArgs){
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* in = funcArgs->in;
	int* r = funcArgs->r;
	int size = funcArgs->size;
	int i, max_r_global, min_r_global = 0;
	int* max_r =  (int*) malloc(PROCESSORS*sizeof(int*));
	max_r = funcArgs->max_r; 
	int* min_r =  (int*) malloc(PROCESSORS*sizeof(int*));
	min_r = funcArgs->min_r;
	int z = size/PROCESSORS *myID;	 
	max_r[myID]=r[z], min_r[myID]=r[z];
  	/* Process for (i=0; i<size; i++), and residue process is on the last site */
	for (i=size/PROCESSORS*myID; i<size/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && size%PROCESSORS!=0 ? size%PROCESSORS : 0); i++){		
      	if ( r[i] > max_r[myID] ){
        		max_r[myID]=r[i];
		}
      	if ( r[i] < min_r[myID] ){
        		min_r[myID]=r[i];
		}
    	}
	actuateBarrier(&myBarrier);				/* Actuate a barrier to start calculating max_r and min_r */
	/* calculate maximum and minmum on the 1st site */
	if(myID == 0){
		max_r_global = max_r[0];
		min_r_global = min_r[0];
		for(i=0; i<PROCESSORS; i++){
			if(max_r[i] > max_r_global){
				max_r_global = max_r[i];
			}
			if(min_r[i] < min_r_global){
				min_r_global = min_r[i];
			}
		}
		max_r_global-=min_r_global;
		funcArgs->max_r_global = max_r_global;
		funcArgs->min_r_global = min_r_global;
	}
	actuateBarrier(&myBarrier);				/* Actuate a barrier to ending max_r and min_r */
	max_r_global = funcArgs->max_r_global;		// Broadcast global max_r
	min_r_global = funcArgs->min_r_global;		// Broadcast global min_r
	/* Process for (i=0; i<size; i++), and residue process is on the last site */
	for (i=size/PROCESSORS*myID; i<size/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && size%PROCESSORS!=0 ? size%PROCESSORS : 0); i++){	
		in[i] = (uchar)((int)((int)(r[i]-min_r_global)*255)/max_r_global);
	}
	return NULL;		
}



/* {{{ setup_brightness_lut(bp,thresh,form) */
void setup_brightness_lut(bp,thresh,form) uchar **bp; int thresh, form; {
	*bp=(unsigned char *)malloc(516);
	*bp=*bp+258;
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->bp = bp;
	funcArgs->thresh = thresh;
	funcArgs->form = form;
	startThreads(0,funcArgs);
	free(funcArgs);
	/*##### Parallelization Work #####
  	for(k=-256;k<257;k++){
    		//..............................................
	}
	*/
}
/* }}} */
void* setup_brightness_lut_thread(void* threadArgs){	
	int k, i;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar **bp = funcArgs->bp;
	int thresh =  funcArgs->thresh;
	int form = funcArgs->form;
	float temp;	
	/* process k from ~256 to 256*/								
	for(k=(-256)+512/PROCESSORS*myID; k<(-256)+512/PROCESSORS*myID+512/PROCESSORS+1; k++){
		temp=((float)k)/((float)thresh);
    		temp=temp*temp;
    		if (form==6){
     			temp=temp*temp*temp;
		}
    		temp=100.0*exp(-temp);int i, timestart, timeend, iter;
    		*(*bp+k)= (uchar)temp;
	}
	if(512%PROCESSORS!=0 && myID<512%PROCESSORS){	//Process residue
		k = 256 - 512%PROCESSORS + myID +1;
		temp=((float)k)/((float)thresh);
    		temp=temp*temp;
    		if (form==6){
     			temp=temp*temp*temp;
		}
    		temp=100.0*exp(-temp);
    		*(*bp+k)= (uchar)temp;
	}	
	return NULL;	    		
}




/* {{{ susan principle */
/* {{{ susan_principle(in,r,bp,max_no,x_size,y_size) */
void susan_principle(in,r,bp,max_no,x_size,y_size) uchar *in, *bp; int *r, max_no, x_size, y_size;{
  	memset (r,0,x_size * y_size * sizeof(int));
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->in = in;
	funcArgs->bp_principle = bp;
	funcArgs->r = r;
	funcArgs->max_no = max_no;
	funcArgs->x_size = x_size;
	funcArgs->y_size = y_size;
	startThreads(3,funcArgs);
	free(funcArgs);
	/*##### Parallelization Work #####
  	for (i=3;i<y_size-3;i++){	
    		for (j=3;j<x_size-3;j++){
      			........................................
    		}
	}
	*/
}
/* }}} */
void* susan_principle_thread(void* threadArgs){
	int   i, j, n;
	uchar *p,*cp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* in = funcArgs->in;
	uchar* bp = funcArgs->bp_principle;
	int* r = funcArgs->r;
	int max_no = funcArgs->max_no;
	int x_size = funcArgs->x_size;
	int y_size = funcArgs->y_size;	
	/* Process for (i=3;i<y_size-3;i++), and residue process is on the last site*/
	for (i=3+(y_size-3-3)/PROCESSORS*myID; i<3+(y_size-3-3)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-3-3)%PROCESSORS!=0 ? (y_size-3-3)%PROCESSORS : 0); i++){	
    		for (j=3;j<x_size-3;j++){
      		n=100;
      		p=in + (i-3)*x_size + j - 1;
      		cp=bp + in[i*x_size+j];
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-3; 
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-5;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-6;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=2;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-6;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-5;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-3;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
     			n+=*(cp-*p);
      		if (n<=max_no){
        			r[i*x_size+j] = max_no - n;
			}
    		}
	}
}



/* {{{ susan_principle_small(in,r,bp,max_no,x_size,y_size) */
void susan_principle_small(in,r,bp,max_no,x_size,y_size) uchar *in, *bp; int *r, max_no, x_size, y_size; {
	//uchar *p,*cp;
  	memset (r,0,x_size * y_size * sizeof(int));
  	max_no = 730;	// ho hum ;) 
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->in = in;
	funcArgs->bp_principle = bp;
	funcArgs->r = r;
	funcArgs->max_no = max_no;	
	startThreads(2,funcArgs);
	free(funcArgs);
	/*##### Parallelization Work #####
  	for (i=1;i<y_size-1;i++){	
    		for (j=1;j<x_size-1;j++){
			.............................................
    		}
	}
	*/
}
/* }}} */
/* }}} */
void* susan_principle_small_thread(void* threadArgs){
	int   i, j, n;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar *p,*cp;
	int y_size = funcArgs->y_size;
	int x_size = funcArgs->x_size;
	uchar* in = funcArgs->in;
	uchar* bp = funcArgs->bp_principle;
	int *r = funcArgs->r;
	int max_no = funcArgs->max_no;
	/* Process for (i=1;i<y_size-1;i++), and residue process is on the last site*/
	for (i=1+(y_size-1-1)/PROCESSORS*myID; i<1+(y_size-1-1)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-1-1)%PROCESSORS!=0 ? (y_size-1-1)%PROCESSORS : 0); i++){
    		for (j=1;j<x_size-1;j++){
      		n=100;
      		p=in + (i-1)*x_size + j - 1;
      		cp=bp + in[i*x_size+j];
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-2; 
      		n+=*(cp-*p);
      		p+=2;
      		n+=*(cp-*p);
      		p+=x_size-2;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		if (n<=max_no){
        			r[i*x_size+j] = max_no - n;
			}
    		}
	}
	return NULL;
}



/* {{{ smoothing */
/* {{{ median(in,i,j,x_size) */
uchar median(in,i,j,x_size) uchar *in; int i, j, x_size; {
	int p[8],k,l,tmp;
  	p[0]=in[(i-1)*x_size+j-1];
  	p[1]=in[(i-1)*x_size+j  ];
  	p[2]=in[(i-1)*x_size+j+1];
  	p[3]=in[(i  )*x_size+j-1];
  	p[4]=in[(i  )*x_size+j+1];
  	p[5]=in[(i+1)*x_size+j-1];
  	p[6]=in[(i+1)*x_size+j  ];
  	p[7]=in[(i+1)*x_size+j+1];
  	for(k=0; k<7; k++){
    		for(l=0; l<(7-k); l++){
      		if (p[l]>p[l+1]){
        			tmp=p[l]; p[l]=p[l+1]; p[l+1]=tmp;
     			 }
		}
	}
  	return( (p[3]+p[4]) / 2 );
}
/* }}} */



/* {{{ enlarge(in,tmp_image,x_size,y_size,border) */
/* this enlarges "in" so that borders can be dealt with easily */
void enlarge(in,tmp_image,x_size,y_size,border) uchar **in; uchar *tmp_image; int *x_size, *y_size, border; {
	/*##### Parallelization Work #####
  	for(i=0; i<*y_size; i++){	
    		....................................................
	}
  	for(i=0; i<border; i++){	
    		...................................................
  	}
	for(i=0; i<border; i++){	
		...................................................
	}    
	*/
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->in_enlarge=in;
	funcArgs->tmp_image=tmp_image;
	funcArgs->x_size_enlarge=x_size;
	funcArgs->y_size_enlarge=y_size;
	funcArgs->border=border;
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(12,funcArgs);
	free(funcArgs);
}
/* }}} */
void* enlarge_thread(void* threadArgs){
	int   i, j;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar **in = funcArgs->in_enlarge; 
	uchar *tmp_image = funcArgs->tmp_image; 
	int *x_size = funcArgs->x_size_enlarge; 
	int *y_size = funcArgs->y_size_enlarge; 
	int border = funcArgs-> border;
	/* copy *in into tmp_image */	
	/* Process 	for(i=0; i<*y_size; i++), and residue process is on the last site*/
	for (i=(*y_size)/PROCESSORS*myID; i<(*y_size)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (*y_size)%PROCESSORS!=0 ? (*y_size)%PROCESSORS : 0); i++){
		memcpy(tmp_image+(i+border)*(*x_size+2*border)+border, *in+i* *x_size, *x_size);
	}
	actuateBarrier(&myBarrier);				/* Actuate a barrier for "tmp_image" value*/	
	/* copy top and bottom rows; invert as many as necessary */
	/* Process 	for(i=0; i<border; i++), and residue process is on the last site*/
	for (i=border/PROCESSORS*myID; i<border/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && border%PROCESSORS!=0 ? border%PROCESSORS : 0); i++){ 	
    		memcpy(tmp_image+(border-1-i)*(*x_size+2*border)+border,*in+i* *x_size,*x_size);
    		memcpy(tmp_image+(*y_size+border+i)*(*x_size+2*border)+border,*in+(*y_size-i-1)* *x_size,*x_size);
  	}
	actuateBarrier(&myBarrier);				/* Actuate a barrier for "tmp_image" value*/	
	/* copy left and right columns */					
	/* Process 	for(i=0; i<border; i++), and residue process is on the last site*/
	for (i=border/PROCESSORS*myID; i<border/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && border%PROCESSORS!=0 ? border%PROCESSORS : 0); i++){ 	
		for(j=0; j<*y_size+2*border; j++){			
      		tmp_image[j*(*x_size+2*border)+border-1-i]=tmp_image[j*(*x_size+2*border)+border+i];
      		tmp_image[j*(*x_size+2*border)+ *x_size+border+i]=tmp_image[j*(*x_size+2*border)+ *x_size+border-1-i];
    		}
	}
	actuateBarrier(&myBarrier);				/* Actuate a barrier for "tmp_image" value*/	
	if(myID==0){
  		*x_size+=2*border;  /* alter image size */
  		*y_size+=2*border;
  		*in=tmp_image;      /* repoint in */
	}
	pthread_exit(NULL);
}



/* {{{ void susan_smoothing(three_by_three,in,dt,x_size,y_size,bp) */
void susan_smoothing(three_by_three,in,dt,x_size,y_size,bp) int three_by_three, x_size, y_size; uchar *in, *bp; float dt; {
	/* {{{ vars */
	float temp;
	int n_max, increment, mask_size, i, j, x, y, area, brightness, tmp, centre;
	uchar *ip, *dp, *dpt, *cp, *out=in, *tmp_image;
	TOTAL_TYPE total;
	/* }}} */
  	/* {{{ setup larger image and border sizes */
  	if (three_by_three==0){
    		mask_size = ((int)(1.5 * dt)) + 1;
  	}else{
		mask_size = 1;
	}
	total=0.1; /* test for total's type */
  	if ( (dt>15) && (total==0) ){
		printf("Distance_thresh (%f) too big for integer arithmetic.\n",dt);
    		printf("Either reduce it to <=15 or recompile with variable \"total\"\n");
    		printf("as a float: see top \"defines\" section.\n");
    		exit(0);
  	}
  	if ( (2*mask_size+1>x_size) || (2*mask_size+1>y_size) ){
    		printf("Mask size (1.5*distance_thresh+1=%d) too big for image (%dx%d).\n",mask_size,x_size,y_size);
    		exit(0);
 	}
  	tmp_image = (uchar *) malloc( (x_size+mask_size*2) * (y_size+mask_size*2) );
  	enlarge(&in,tmp_image,&x_size,&y_size,mask_size);
	/* }}} */
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->three_by_three = three_by_three;  
	/* large Gaussian masks */
    	/* {{{ setup distance lut */
  	n_max = (mask_size*2) + 1;
  	increment = x_size - n_max;
  	dp = (unsigned char *)malloc(n_max*n_max);
  	dpt = dp;
  	temp = -(dt*dt);
	funcArgs->mask_size = mask_size;
	funcArgs->temp = temp;
	funcArgs->dpt = dpt;
	funcArgs->dp = dp;
    	/* {{{ main section */
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->bp_smoothing = bp;
	funcArgs->increment = increment;
	funcArgs->out = out;
	funcArgs->in = in;
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(1,funcArgs);
	free(funcArgs);
}
/* }}} */
/* }}} */
void* susan_smoothing_thread(void* threadArgs){
	int i,j,x,y,z;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	int three_by_three = funcArgs->three_by_three;
	int y_size = funcArgs->y_size;
	int x_size = funcArgs->x_size;
	uchar *in = funcArgs->in;
	float temp = funcArgs->temp;
	uchar* bp_smoothing = funcArgs->bp_smoothing;
	int mask_size = funcArgs->mask_size;
	uchar *out = funcArgs->out;
	int area, centre, brightness, tmp;
	uchar *ip, *cp;
	TOTAL_TYPE total;
	if(three_by_three == 0){
		uchar* dpt = funcArgs->dpt;
		uchar *dp = funcArgs->dp;
		/* Process for(i=-mask_size; i<=mask_size; i++) */
		int p = (mask_size+mask_size) / PROCESSORS * (mask_size+mask_size+1)  * myID;
		for(i=(-mask_size)+(mask_size*2)/PROCESSORS*myID; i<(-mask_size)+(mask_size*2)/PROCESSORS*myID+(mask_size*2)/PROCESSORS; i++){
			for(j=-mask_size; j<=mask_size; j++){
      			x = (int) (100.0 * exp( ((float)((i*i)+(j*j))) / temp ));
      			//*dpt++ = (unsigned char)x;
				*(dpt+p) = (unsigned char)x;
				p ++;
    			}
		}
		if((mask_size+mask_size+1)%PROCESSORS!=0 && myID==PROCESSORS-1){	/* Process residue on last site*/
			for(j=-mask_size; j<=mask_size; j++){
      			x = (int) (100.0 * exp( ((float)((i*i)+(j*j))) / temp ));
      			//*dpt++ = (unsigned char)x;
				*(dpt+p) = (unsigned char)x;
				p ++;
    			}
			*dpt = *(dpt+p);
		}	
		actuateBarrier(&myBarrier);				/* Actuate a barrier for "dpt" value*/			
		/* Process for (i=mask_size;i<y_size-mask_size;i++) */
		z = (y_size-mask_size-mask_size)/PROCESSORS * (x_size-mask_size-mask_size) *myID;	
		/* Notice the position of ouput pointer that should be assigned according to ID */
		out = out+z;
		int increment = funcArgs->increment;
		/* Process for(i=mask_size;i<y_size-mask_size;i++), and residue process is on the last site */
		for (i=mask_size+(y_size-mask_size-mask_size)/PROCESSORS*myID;i<mask_size+(y_size-mask_size-mask_size)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-mask_size-mask_size)%PROCESSORS!=0 ? (y_size-mask_size-mask_size)%PROCESSORS : 0); i++){	
    			for (j=mask_size;j<x_size-mask_size;j++){
      			area = 0;
      			total = 0;
      			dpt = dp;
      			ip = in + ((i-mask_size)*x_size) + j - mask_size;
      			centre = in[i*x_size+j];
      			cp = bp_smoothing + centre;			
      			for(y=-mask_size; y<=mask_size; y++){
        				for(x=-mask_size; x<=mask_size; x++){
          					brightness = *ip++;
          					tmp = *dpt++ * *(cp-brightness);
          					area += tmp;
          					total += tmp * brightness;
        				}
        				ip += increment;
     				}
      			tmp = area-10000;
      			if (tmp==0){
					*out++=median(in,i,j,x_size);
      			}else{
					*out++=((total-(centre*10000))/tmp);
				}		
    			}
  		}
	}else{
		/* Process for (i=1;i<y_size-1;i++), and residue process is on the last site*/
		z = (y_size-1-1)/PROCESSORS * (x_size-1-1) *myID;	
		/* Notice the position of ouput pointer that should be assigned according to ID */
		out = out+z;
		for (i=1+(y_size-1-1)/PROCESSORS*myID; i<1+(y_size-1-1)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-1-1)%PROCESSORS!=0 ? (y_size-1-1)%PROCESSORS : 0); i++){
    			for (j=1;j<x_size-1;j++){
      			area = 0;
      			total = 0;
      			ip = in + ((i-1)*x_size) + j - 1;
      			centre = in[i*x_size+j];
      			cp = bp_smoothing + centre;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip; 	tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			ip += x_size-2;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip; 	tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			ip += x_size-2;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip++; tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			brightness=*ip; 	tmp=*(cp-brightness); 	area += tmp; 	total += tmp * brightness;
      			tmp = area-100;
      			if (tmp==0){
        				*out++=median(in,i,j,x_size);
      			}else{
        				*out++=(total-(centre*100))/tmp;
    				}
			}
  		}
	}
	pthread_exit(NULL);
}



/* {{{ edges */
/* {{{ edge_draw(in,corner_list,drawing_mode) */
void edge_draw(in,mid,x_size,y_size,drawing_mode) uchar *in, *mid; int x_size, y_size, drawing_mode;{
	uchar *inp, *midp;
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->mid = mid;
	funcArgs->in = in;
	funcArgs->drawing_mode = drawing_mode;
	funcArgs->midp = midp;
	funcArgs->inp = inp;
	startBarrier(&myBarrier);	/* Start barrier 	*/	
	startThreads(8,funcArgs);
	free(funcArgs);
	/*
  	if (drawing_mode==0){
		//##### Parallelization Work #####
    		for (i=0; i<x_size*y_size; i++){	
   			............................................
    		}
  	}
	//##### Parallelization Work #####
  	for (i=0; i<x_size*y_size; i++){		
    		..............................................................
  	}
	*/
}
/* }}} */
void* edge_draw_thread(void* threadArgs){
	int   i;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* in = funcArgs->in;
	uchar* mid = funcArgs->mid;
	uchar* midp = funcArgs->midp;
	uchar* inp = funcArgs->inp;
	int* r = funcArgs->r;
	int x_size = funcArgs->x_size;
	int y_size = funcArgs->y_size;
	int drawing_mode = funcArgs->drawing_mode;
	int z = x_size*y_size/PROCESSORS *myID;
	if (drawing_mode==0){
		midp=mid + z;
		/* Process 	for (i=0; i<x_size*y_size; i++), and residue process is on the last site*/
		for (i=x_size*y_size/PROCESSORS*myID; i<x_size*y_size/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && x_size*y_size%PROCESSORS!=0 ?x_size*y_size%PROCESSORS : 0); i++){
      		if (*midp<8) {
				inp = in + (midp - mid) - x_size - 1;
        			*inp++=255; *inp++=255; *inp=255; inp+=x_size-2;
        			*inp++=255; *inp++;     *inp=255; inp+=x_size-2;
        			*inp++=255; *inp++=255; *inp=255;
      			}
      		midp++;
    		}
	}
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "r[]" value*/	
	midp=mid +z;
	/* Process 	for (i=0; i<x_size*y_size; i++), and residue process is on the last site*/
  	for (i=x_size*y_size/PROCESSORS*myID; i<x_size*y_size/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && x_size*y_size%PROCESSORS!=0 ?x_size*y_size%PROCESSORS : 0); i++){		
    		if (*midp<8){ 
      		*(in + (midp - mid)) = 0;
		}
    		midp++;
  	}
}



/* {{{ susan_thin(r,mid,x_size,y_size) */
/* only one pass is needed as i,j are decremented if necessary to go back and do bits again */
void susan_thin(r,mid,x_size,y_size) uchar *mid; int   *r, x_size, y_size; {
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->mid = mid;
	funcArgs->r = r;	
	startThreads(7,funcArgs);
	free(funcArgs);
	/*##### Parallelization Work #####
  	for (i=4;i<y_size-4;i++){	
    		...................................
	}	
	*/
}
/* }}} */
void* susan_thin_thread(void* threadArgs){
	int   l[9], centre, nlinks, npieces;
      int	b01, b12, b21, b10;
      int	p1, p2, p3, p4;
      int	b00, b02, b20, b22;
      int	m, n, a, b, x, y, i, j;
	uchar *mp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* mid = funcArgs->mid;
	int* r = funcArgs->r;
	int x_size = funcArgs->x_size;
	int y_size = funcArgs->y_size;
	/* Process 	for (i=4;i<y_size-4;i++), and residue process is on the last site*/
	for (i=4+(y_size-4-4)/PROCESSORS*myID; i<4+(y_size-4-4)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-4-4)%PROCESSORS!=0 ? (y_size-4-4)%PROCESSORS : 0); i++){
		for (j=4;j<x_size-4;j++){
			if (mid[i*x_size+j]<8){
        			centre = r[i*x_size+j];
       			/* {{{ count number of neighbours */
        			mp=mid + (i-1)*x_size + j-1;
				n = (*mp<8) +
				    (*(mp+1)<8) +
            		    (*(mp+2)<8) +
            		    (*(mp+x_size)<8) +
            		    (*(mp+x_size+2)<8) +
            		    (*(mp+x_size+x_size)<8) +
            		    (*(mp+x_size+x_size+1)<8) +
            		    (*(mp+x_size+x_size+2)<8);

				/* }}} */
        			/* {{{ n==0 no neighbours - remove point */
        			if (n==0){
          				mid[i*x_size+j]=100;
				}
				/* }}} */
        			/* {{{ n==1 - extend line if I can */
        			/* extension is only allowed a few times - the value of mid is used to control this */
        			if ( (n==1) && (mid[i*x_size+j]<6) ){
          				/* find maximum neighbour weighted in direction opposite the neighbour already present. e.g.
             			have: O O O  weight r by 0 2 3
                   			X X O              0 0 4
                  			O O O              0 2 3     */
         				l[0]=r[(i-1)*x_size+j-1]; l[1]=r[(i-1)*x_size+j]; l[2]=r[(i-1)*x_size+j+1];
					l[3]=r[(i  )*x_size+j-1]; l[4]=0;                 l[5]=r[(i  )*x_size+j+1];
          				l[6]=r[(i+1)*x_size+j-1]; l[7]=r[(i+1)*x_size+j]; l[8]=r[(i+1)*x_size+j+1];
          				if (mid[(i-1)*x_size+j-1]<8)        { l[0]=0; l[1]=0; l[3]=0; l[2]*=2; l[6]*=2; l[5]*=3; l[7]*=3; l[8]*=4; }
          				else { if (mid[(i-1)*x_size+j]<8)   { l[1]=0; l[0]=0; l[2]=0; l[3]*=2; l[5]*=2; l[6]*=3; l[8]*=3; l[7]*=4; }
          				else { if (mid[(i-1)*x_size+j+1]<8) { l[2]=0; l[1]=0; l[5]=0; l[0]*=2; l[8]*=2; l[3]*=3; l[7]*=3; l[6]*=4; }
          				else { if (mid[(i)*x_size+j-1]<8)   { l[3]=0; l[0]=0; l[6]=0; l[1]*=2; l[7]*=2; l[2]*=3; l[8]*=3; l[5]*=4; }
          				else { if (mid[(i)*x_size+j+1]<8)   { l[5]=0; l[2]=0; l[8]=0; l[1]*=2; l[7]*=2; l[0]*=3; l[6]*=3; l[3]*=4; }
          				else { if (mid[(i+1)*x_size+j-1]<8) { l[6]=0; l[3]=0; l[7]=0; l[0]*=2; l[8]*=2; l[1]*=3; l[5]*=3; l[2]*=4; }
          				else { if (mid[(i+1)*x_size+j]<8)   { l[7]=0; l[6]=0; l[8]=0; l[3]*=2; l[5]*=2; l[0]*=3; l[2]*=3; l[1]*=4; }
          				else { if (mid[(i+1)*x_size+j+1]<8) { l[8]=0; l[5]=0; l[7]=0; l[6]*=2; l[2]*=2; l[1]*=3; l[3]*=3; l[0]*=4; } }}}}}}}
          				m=0;     /* find the highest point */
          				for(y=0; y<3; y++){
            				for(x=0; x<3; x++){
              					if (l[y+y+y+x]>m){ 
								m=l[y+y+y+x]; a=y; b=x; 
							}
						}
					}
          				if (m>0){
            				if (mid[i*x_size+j]<4){
              					mid[(i+a-1)*x_size+j+b-1] = 4;
            				}else{
              					mid[(i+a-1)*x_size+j+b-1] = mid[i*x_size+j]+1;
						}
            				if ( (a+a+b) < 3 ){ /* need to jump back in image */
              					i+=a-1;
              					j+=b-2;
              					if (i<4) i=4;
              					if (j<4) j=4;
	    					}
	  				}
        			}
				/* }}} */
      			/* {{{ n==2 */
				if (n==2){
					/* put in a bit here to straighten edges */
					b00 = mid[(i-1)*x_size+j-1]<8; /* corners of 3x3 */
					b02 = mid[(i-1)*x_size+j+1]<8;
					b20 = mid[(i+1)*x_size+j-1]<8;
          				b22 = mid[(i+1)*x_size+j+1]<8;
          				if ( ((b00+b02+b20+b22)==2) && ((b00|b22)&(b02|b20))){
						/* case: move a point back into line.
						e.g. X O X  CAN  become X X X
			     			     O X O              O O O
                       			     O O O              O O O    */
            				if (b00){ 
              					if (b02) { x=0; y=-1; }
              					else     { x=-1; y=0; }
	    					}else{
              					if (b02) { x=1; y=0; }
              					else     { x=0; y=1; }
						}
						if (((float)r[(i+y)*x_size+j+x]/(float)centre) > 0.7){
							if ( ( (x==0) && (mid[(i+(2*y))*x_size+j]>7) && (mid[(i+(2*y))*x_size+j-1]>7) && (mid[(i+(2*y))*x_size+j+1]>7) ) ||
			   	     				( (y==0) && (mid[(i)*x_size+j+(2*x)]>7) && (mid[(i+1)*x_size+j+(2*x)]>7) && (mid[(i-1)*x_size+j+(2*x)]>7) ) ){
                						mid[(i)*x_size+j]=100;
                						mid[(i+y)*x_size+j+x]=3;  /* no jumping needed */
	      						}
	    					}
					}else{
            				b01 = mid[(i-1)*x_size+j  ]<8;
            				b12 = mid[(i  )*x_size+j+1]<8;
            				b21 = mid[(i+1)*x_size+j  ]<8;
            				b10 = mid[(i  )*x_size+j-1]<8;
            				/* {{{ right angle ends - not currently used */
						#ifdef IGNORETHIS
						if ( (b00&b01)|(b00&b10)|(b02&b01)|(b02&b12)|(b20&b10)|(b20&b21)|(b22&b21)|(b22&b12) ){
							/* case; right angle ends. clean up.
							e.g.; X X O  CAN  become X X O
                        				O X O              O O O
                        				O O O              O O O        */
							if ( ((b01)&(mid[(i-2)*x_size+j-1]>7)&(mid[(i-2)*x_size+j]>7)&(mid[(i-2)*x_size+j+1]>7)&
                                    			((b00&((2*r[(i-1)*x_size+j+1])>centre))|(b02&((2*r[(i-1)*x_size+j-1])>centre)))) |
								((b10)&(mid[(i-1)*x_size+j-2]>7)&(mid[(i)*x_size+j-2]>7)&(mid[(i+1)*x_size+j-2]>7)&
                                    			((b00&((2*r[(i+1)*x_size+j-1])>centre))|(b20&((2*r[(i-1)*x_size+j-1])>centre)))) |
								((b12)&(mid[(i-1)*x_size+j+2]>7)&(mid[(i)*x_size+j+2]>7)&(mid[(i+1)*x_size+j+2]>7)&
                                    			((b02&((2*r[(i+1)*x_size+j+1])>centre))|(b22&((2*r[(i-1)*x_size+j+1])>centre)))) |
								((b21)&(mid[(i+2)*x_size+j-1]>7)&(mid[(i+2)*x_size+j]>7)&(mid[(i+2)*x_size+j+1]>7)&
                                    			((b20&((2*r[(i+1)*x_size+j+1])>centre))|(b22&((2*r[(i+1)*x_size+j-1])>centre)))) ){
								mid[(i)*x_size+j]=100;
                						if (b10&b20){j-=2;}
                						if (b00|b01|b02) { i--; j-=2; }
							}
						}
						#endif
						/* }}} */
						if ( ((b01+b12+b21+b10)==2) && ((b10|b12)&(b01|b21)) &&
                 					((b01&((mid[(i-2)*x_size+j-1]<8)|(mid[(i-2)*x_size+j+1]<8)))|(b10&((mid[(i-1)*x_size+j-2]<8)|(mid[(i+1)*x_size+j-2]<8)))|
                 					(b12&((mid[(i-1)*x_size+j+2]<8)|(mid[(i+1)*x_size+j+2]<8)))|(b21&((mid[(i+2)*x_size+j-1]<8)|(mid[(i+2)*x_size+j+1]<8)))) ){
							/* case; clears odd right angles.
                 					e.g.; O O O  becomes O O O
                       					X X O          X O O
                       					O X O          O X O     */
							mid[(i)*x_size+j]=100;
              					i--;               	/* jump back */
              					j-=2;
              					if (i<4) i=4;
              					if (j<4) j=4;
	    					}
					}
				}
				/* }}} */
				/* {{{ n>2 the thinning is done here without breaking connectivity */
				if (n>2){
					b01 = mid[(i-1)*x_size+j  ]<8;
					b12 = mid[(i  )*x_size+j+1]<8;
					b21 = mid[(i+1)*x_size+j  ]<8;
					b10 = mid[(i  )*x_size+j-1]<8;
					if((b01+b12+b21+b10)>1){
						b00 = mid[(i-1)*x_size+j-1]<8;
            				b02 = mid[(i-1)*x_size+j+1]<8;
	    					b20 = mid[(i+1)*x_size+j-1]<8;
	    					b22 = mid[(i+1)*x_size+j+1]<8;
            				p1 = b00 | b01;
            				p2 = b02 | b12;
            				p3 = b22 | b21;
            				p4 = b20 | b10;
						if( ((p1 + p2 + p3 + p4) - ((b01 & p2)+(b12 & p3)+(b21 & p4)+(b10 & p1))) < 2){
              					mid[(i)*x_size+j]=100;
              					i--;
              					j-=2;
              					if (i<4) i=4;
              					if (j<4) j=4;
            					}
					}
        			}

/* }}} */
			}	/* if(<8) */
		}	/* 2nd for */
	}
}



/* {{{ susan_edges(in,r,sf,max_no,out) */
void susan_edges(in,r,mid,bp,max_no,x_size,y_size) uchar *in, *bp, *mid; int *r, max_no, x_size, y_size; {
	float z;
	int   do_symmetry, i, j, m, n, a, b, x, y, w;
	uchar c,*p,*cp;
  	memset (r,0,x_size * y_size * sizeof(int));
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->x_size = x_size;
	funcArgs->y_size = y_size;
	funcArgs->in = in;
	funcArgs->bp_edges = bp;
	funcArgs->mid = mid;
	funcArgs->r = r;
	funcArgs->max_no = max_no;
	/*##### Parallelization Work #####
  	for (i=3;i<y_size-3;i++){	
    		.................................	
	}	
	*/
	/*##### Parallelization Work #####
  	for (i=4;i<y_size-4;i++){	
		................................
	}
	*/
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(6,funcArgs);
	free(funcArgs);
}
/* }}} */
void* susan_edges_thread(void* threadArgs){
	float z;
	int   do_symmetry, i, j, m, n, a, b, x, y, w;
	uchar c,*p,*cp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* in = funcArgs->in;
	uchar* bp = funcArgs->bp_edges;
	uchar* mid = funcArgs->mid;
	int* r = funcArgs->r;
	int max_no = funcArgs->max_no;
	int x_size = funcArgs->x_size;
	int y_size = funcArgs->y_size;
	/* Process 	for (i=3;i<y_size-3;i++), and residue process is on the last site*/
	for (i=3+(y_size-3-3)/PROCESSORS*myID; i<3+(y_size-3-3)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-3-3)%PROCESSORS!=0 ? (y_size-3-3)%PROCESSORS : 0); i++){
		for (j=3;j<x_size-3;j++){
      		n=100;
      		p=in + (i-3)*x_size + j - 1;
      		cp=bp + in[i*x_size+j];
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-3; 
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-5;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-6;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=2;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-6;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-5;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-3;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		if (n<=max_no){
        			r[i*x_size+j] = max_no - n;
			}
    		}	/* 2nd for */
	}	/* 1st for */
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "r[]" value*/
	/* Process 	for (i=4;i<y_size-4;i++), and residue process is on the last site*/
	for (i=4+(y_size-4-4)/PROCESSORS*myID; i<4+(y_size-4-4)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-4-4)%PROCESSORS!=0 ? (y_size-4-4)%PROCESSORS : 0); i++){
		for (j=4;j<x_size-4;j++){
      		if (r[i*x_size+j]>0){
        			m=r[i*x_size+j];
        			n=max_no - m;
        			cp=bp + in[i*x_size+j];
        			if (n>600){
          				p=in + (i-3)*x_size + j - 1;
          				x=0;y=0;
          				c=*(cp-*p++);x-=c;y-=3*c;
          				c=*(cp-*p++);y-=3*c;
          				c=*(cp-*p);x+=c;y-=3*c;
          				p+=x_size-3;     
          				c=*(cp-*p++);x-=2*c;y-=2*c;
          				c=*(cp-*p++);x-=c;y-=2*c;
          				c=*(cp-*p++);y-=2*c;
          				c=*(cp-*p++);x+=c;y-=2*c;
          				c=*(cp-*p);x+=2*c;y-=2*c;
          				p+=x_size-5;   
          				c=*(cp-*p++);x-=3*c;y-=c;
          				c=*(cp-*p++);x-=2*c;y-=c;
          				c=*(cp-*p++);x-=c;y-=c;
          				c=*(cp-*p++);y-=c;
          				c=*(cp-*p++);x+=c;y-=c;
          				c=*(cp-*p++);x+=2*c;y-=c;
          				c=*(cp-*p);x+=3*c;y-=c;
          				p+=x_size-6;
          				c=*(cp-*p++);x-=3*c;
          				c=*(cp-*p++);x-=2*c;
          				c=*(cp-*p);x-=c;
          				p+=2;
          				c=*(cp-*p++);x+=c;
          				c=*(cp-*p++);x+=2*c;
          				c=*(cp-*p);x+=3*c;
          				p+=x_size-6;
          				c=*(cp-*p++);x-=3*c;y+=c;
          				c=*(cp-*p++);x-=2*c;y+=c;
          				c=*(cp-*p++);x-=c;y+=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p++);x+=c;y+=c;
          				c=*(cp-*p++);x+=2*c;y+=c;
          				c=*(cp-*p);x+=3*c;y+=c;
          				p+=x_size-5;
          				c=*(cp-*p++);x-=2*c;y+=2*c;
          				c=*(cp-*p++);x-=c;y+=2*c;
          				c=*(cp-*p++);y+=2*c;
          				c=*(cp-*p++);x+=c;y+=2*c;
          				c=*(cp-*p);x+=2*c;y+=2*c;
          				p+=x_size-3;
          				c=*(cp-*p++);x-=c;y+=3*c;
          				c=*(cp-*p++);y+=3*c;
          				c=*(cp-*p);x+=c;y+=3*c;
          				z = sqrt((float)((x*x) + (y*y)));
          				if (z > (0.9*(float)n)){	/* 0.5 */
            				do_symmetry=0;
            				if (x==0)
              					z=1000000.0;
            				else
              					z=((float)y) / ((float)x);
            				if (z < 0) { z=-z; w=-1; }
            				else w=1;
            				if (z < 0.5) { /* vert_edge */ a=0; b=1; }
            				else { if (z > 2.0) { /* hor_edge */ a=1; b=0; }
            				else { /* diag_edge */ if (w>0) { a=1; b=1; }
                                   	else { a=-1; b=1; }}}
            				if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) &&
                 					(m > r[(i+(2*a))*x_size+j+(2*b)]) && (m >= r[(i-(2*a))*x_size+j-(2*b)]) )
              					mid[i*x_size+j] = 1;
          				} else{
            				do_symmetry=1;
					}
        			} else{ 
          				do_symmetry=1;
				}
        			if (do_symmetry==1){ 
          				p=in + (i-3)*x_size + j - 1;
          				x=0; y=0; w=0;
          				/*   |      \
               			y  -x-  w
               				|        \   */
          				c=*(cp-*p++);x+=c;y+=9*c;w+=3*c;
          				c=*(cp-*p++);y+=9*c;
          				c=*(cp-*p);x+=c;y+=9*c;w-=3*c;
          				p+=x_size-3; 
          				c=*(cp-*p++);x+=4*c;y+=4*c;w+=4*c;
          				c=*(cp-*p++);x+=c;y+=4*c;w+=2*c;
          				c=*(cp-*p++);y+=4*c;
          				c=*(cp-*p++);x+=c;y+=4*c;w-=2*c;
          				c=*(cp-*p);x+=4*c;y+=4*c;w-=4*c;
          				p+=x_size-5;   
          				c=*(cp-*p++);x+=9*c;y+=c;w+=3*c;
          				c=*(cp-*p++);x+=4*c;y+=c;w+=2*c;
          				c=*(cp-*p++);x+=c;y+=c;w+=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p++);x+=c;y+=c;w-=c;
          				c=*(cp-*p++);x+=4*c;y+=c;w-=2*c;
          				c=*(cp-*p);x+=9*c;y+=c;w-=3*c;
          				p+=x_size-6;
          				c=*(cp-*p++);x+=9*c;
          				c=*(cp-*p++);x+=4*c;
          				c=*(cp-*p);x+=c;
          				p+=2;
          				c=*(cp-*p++);x+=c;
          				c=*(cp-*p++);x+=4*c;
          				c=*(cp-*p);x+=9*c;
          				p+=x_size-6;    
          				c=*(cp-*p++);x+=9*c;y+=c;w-=3*c;
          				c=*(cp-*p++);x+=4*c;y+=c;w-=2*c;
          				c=*(cp-*p++);x+=c;y+=c;w-=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p++);x+=c;y+=c;w+=c;
          				c=*(cp-*p++);x+=4*c;y+=c;w+=2*c;
          				c=*(cp-*p);x+=9*c;y+=c;w+=3*c;
          				p+=x_size-5;
          				c=*(cp-*p++);x+=4*c;y+=4*c;w-=4*c;
          				c=*(cp-*p++);x+=c;y+=4*c;w-=2*c;
          				c=*(cp-*p++);y+=4*c;
          				c=*(cp-*p++);x+=c;y+=4*c;w+=2*c;
          				c=*(cp-*p);x+=4*c;y+=4*c;w+=4*c;
          				p+=x_size-3;
          				c=*(cp-*p++);x+=c;y+=9*c;w-=3*c;
          				c=*(cp-*p++);y+=9*c;
          				c=*(cp-*p);x+=c;y+=9*c;w+=3*c;
          				if (y==0)
            				z = 1000000.0;
          				else
            				z = ((float)x) / ((float)y);
          				if (z < 0.5) { /* vertical */ a=0; b=1; }
          				else { if (z > 2.0) { /* horizontal */ a=1; b=0; }
          				else { /* diagonal */ if (w>0) { a=-1; b=1; }
                              else { a=1; b=1; }}}
          				if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) &&
               				(m > r[(i+(2*a))*x_size+j+(2*b)]) && (m >= r[(i-(2*a))*x_size+j-(2*b)]) )
            				mid[i*x_size+j] = 2;	
        			}
      			}
    		}	/* 2nd for */	
	}	/* 1st for */
	return NULL;
}



/* {{{ susan_edges_small(in,r,sf,max_no,out) */
void susan_edges_small(in,r,mid,bp,max_no,x_size,y_size) uchar *in, *bp, *mid; int *r, max_no, x_size, y_size; {
	//float z;
	//int   do_symmetry, i, j, m, n, a, b, x, y, w;
	//uchar c,*p,*cp;
  	memset (r,0,x_size * y_size * sizeof(int));
 	max_no = 730; /* ho hum ;) */
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->in = in;
	funcArgs->bp_edges = bp;
	funcArgs->mid = mid;
	funcArgs->r = r;
	funcArgs->max_no = max_no;
	funcArgs->x_size = x_size;
	funcArgs->y_size = y_size;
	/*##### Parallelization Work #####
  	for (i=1;i<y_size-1;i++){	
    		.....................................
	}
	*/
	/*##### Parallelization Work #####
  	for (i=2;i<y_size-2;i++){	
    		....................................
	}
	*/
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(5,funcArgs);
	free(funcArgs);	
}
/* }}} */
/* }}} */
void* susan_edges_small_thread(void* threadArgs){
	float z;
	int   do_symmetry, i, j, m, n, a, b, x, y, w;
	uchar c,*p,*cp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	uchar* in = funcArgs->in;
	uchar* bp = funcArgs->bp_edges;
	uchar* mid = funcArgs->mid;
	int* r = funcArgs->r;
	int max_no = funcArgs->max_no;
	int x_size = funcArgs->x_size;
	int y_size = funcArgs->y_size;
	/* Process 	for (i=1;i<y_size-1;i++), and residue process is on the last site*/
	for (i=1+(y_size-1-1)/PROCESSORS*myID; i<1+(y_size-1-1)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-1-1)%PROCESSORS!=0 ? (y_size-1-1)%PROCESSORS : 0); i++){	
    		for (j=1;j<x_size-1;j++){
      		n=100;
      		p=in + (i-1)*x_size + j - 1;
      		cp=bp + in[i*x_size+j];
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		p+=x_size-2; 
      		n+=*(cp-*p);
      		p+=2;
      		n+=*(cp-*p);
      		p+=x_size-2;
      		n+=*(cp-*p++);
      		n+=*(cp-*p++);
      		n+=*(cp-*p);
      		if (n<=max_no)
        			r[i*x_size+j] = max_no - n;
    		}
	}
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "r[]" value*/		
	/* Process 	for (i=2;i<y_size-2;i++), and residue process is on the last site*/
	for (i=2+(y_size-2-2)/PROCESSORS*myID; i<2+(y_size-2-2)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-2-2)%PROCESSORS!=0 ? (y_size-2-2)%PROCESSORS : 0); i++){
		for (j=2;j<x_size-2;j++){
      		if (r[i*x_size+j]>0){
        			m=r[i*x_size+j];
        			n=max_no - m;
        			cp=bp + in[i*x_size+j];
        			if (n>250){
          				p=in + (i-1)*x_size + j - 1;
          				x=0;y=0;
          				c=*(cp-*p++);x-=c;y-=c;
          				c=*(cp-*p++);y-=c;
          				c=*(cp-*p);x+=c;y-=c;
          				p+=x_size-2; 
          				c=*(cp-*p);x-=c;
          				p+=2;
          				c=*(cp-*p);x+=c;
          				p+=x_size-2;
          				c=*(cp-*p++);x-=c;y+=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p);x+=c;y+=c;
          				z = sqrt((float)((x*x) + (y*y)));
          				if (z > (0.4*(float)n)){ 
            				do_symmetry=0;
            				if (x==0)
	      					z=1000000.0;
	    					else
	      					z=((float)y) / ((float)x);
	    					if (z < 0) { z=-z; w=-1; }
            				else w=1;
            				if (z < 0.5) {a=0; b=1; }
            				else { if (z > 2.0) {a=1; b=0; }
            				else { if (w>0) { a=1; b=1; }
                                   	else { a=-1; b=1; }}}
            				if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) )
              					mid[i*x_size+j] = 1;
         				} else
            				do_symmetry=1;
        			} else{
          				do_symmetry=1;
				}
        			if (do_symmetry==1){ 
          				p=in + (i-1)*x_size + j - 1;
          				x=0; y=0; w=0;
          				c=*(cp-*p++);x+=c;y+=c;w+=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p);x+=c;y+=c;w-=c;
          				p+=x_size-2; 
          				c=*(cp-*p);x+=c;
          				p+=2;
          				c=*(cp-*p);x+=c;
          				p+=x_size-2;
          				c=*(cp-*p++);x+=c;y+=c;w-=c;
          				c=*(cp-*p++);y+=c;
          				c=*(cp-*p);x+=c;y+=c;w+=c;
          				if (y==0)
            				z = 1000000.0;
          				else
            				z = ((float)x) / ((float)y);
          				if (z < 0.5) {  a=0; b=1;}
          				else { if (z > 2.0) { a=1; b=0;}
          				else { if (w>0) { a=-1; b=1;}
                              else { a=1; b=1; }}}
          				if ( (m > r[(i+a)*x_size+j+b]) && (m >= r[(i-a)*x_size+j-b]) ){
            				mid[i*x_size+j] = 2;
					}	
        			}
			}
    		}
	}	
	return NULL;
}



/* {{{ corners */
/* {{{ corner_draw(in,corner_list,drawing_mode) */
void corner_draw(in,corner_list,x_size,drawing_mode) uchar *in; CORNER_LIST* corner_list; int x_size, drawing_mode; {
	int   n=0;
	/*##### Parallelization Work #####
  	while(corner_list[n].info != 7){	
    		............................................
  	}
	*/
	int* stop_signal = (int*) malloc(sizeof(int*));
	(*stop_signal) = MAX_CORNERS;		
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->in = in;
	funcArgs->corner_list = corner_list;
	funcArgs->x_size = x_size;
	funcArgs->drawing_mode = drawing_mode;
	funcArgs->stop_signal = stop_signal;
	pthread_mutex_t pt_mutex;
	pthread_mutex_init(&pt_mutex, NULL);
	funcArgs->pt_mutex = pt_mutex; 
	startThreads(11,funcArgs);
	free(funcArgs);
}
/* }}} */
void* corner_draw_thread(void* threadArgs){
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	int x_size = funcArgs->x_size;
	int n;
	CORNER_LIST *corner_list = funcArgs->corner_list;
	int drawing_mode = funcArgs->drawing_mode;
	uchar* in = funcArgs->in;
	uchar* p;
	int* stop_signal = funcArgs->stop_signal;
	pthread_mutex_t pt_mutex = funcArgs->pt_mutex;
	//int z = MAX_CORNERS/PROCESSORS*myID;
	//n = n + z;
	while(corner_list[n].info != 7 && n<(*stop_signal)){
		if(myID == n%PROCESSORS){	//process in ture on different processor
			if (drawing_mode==0){
      			p = in + (corner_list[n].y-1)*x_size + corner_list[n].x - 1;
      			*p++=255; *p++=255; *p=255; p+=x_size-2;
      			*p++=255; *p++=0;   *p=255; p+=x_size-2;
      			*p++=255; *p++=255; *p=255;
    			}else{
      			p = in + corner_list[n].y*x_size + corner_list[n].x;
      			*p=0;
    			}
		}
		n++;
	}
	if(corner_list[n].info == 7){
		pthread_mutex_lock(&pt_mutex);
		*stop_signal = n;
		pthread_mutex_unlock(&pt_mutex);
		pthread_mutex_destroy(&pt_mutex);
	}
	
	return NULL;
}



/* {{{ susan(in,r,sf,max_no,corner_list) */
void susan_corners(in,r,bp,max_no,corner_list,x_size,y_size) uchar *in, *bp; int *r, max_no, x_size, y_size; CORNER_LIST* corner_list; {
  	memset (r,0,x_size * y_size * sizeof(int));
  	int* cgx=(int *)malloc(x_size*y_size*sizeof(int));
  	int* cgy=(int *)malloc(x_size*y_size*sizeof(int));
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->r = r;
	funcArgs->bp_corners = bp;
	funcArgs->max_no = max_no;
	funcArgs->corner_list = corner_list;
	funcArgs->in = in;
	funcArgs->cgx = cgx;
	funcArgs->cgy = cgy;
	int ns[PROCESSORS];
	funcArgs->ns = ns;	
	/*##### Parallelization Work #####
  	for (i=5;i<y_size-5;i++){	
    		..........................................
	}
	*/
	/*##### Parallelization Work #####
  	for (i=5;i<y_size-5;i++){	
    		..........................................
	}
	*/
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(10,funcArgs);
	free(funcArgs);
	free(cgx);
	free(cgy);
}
/* }}} */
void* susan_corners_thread(void* threadArgs){
	int   i,j,n,x,y,sq,xx,yy;
	float divide;
	uchar c,*p,*cp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	int y_size = funcArgs->y_size;
	int x_size = funcArgs->x_size;
	int *r = funcArgs->r;
	uchar* bp = funcArgs->bp_corners;
	int max_no = funcArgs->max_no;
	//CORNER_LIST* corner_list = funcArgs->corner_list;
	uchar* in = funcArgs->in;
	int* cgx = funcArgs->cgx;
	int* cgy = funcArgs->cgy;
	int* ns = funcArgs->ns;
	CORNER_LIST corner_list[MAX_CORNERS];
	memcpy(corner_list,funcArgs->corner_list,sizeof(CORNER_LIST)*MAX_CORNERS);
	/* Process 	for (i=5;i<y_size-5;i++), and residue process is on the last site*/
	for (i=5+(y_size-5-5)/PROCESSORS*myID; i<5+(y_size-5-5)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-5-5)%PROCESSORS!=0 ? (y_size-5-5)%PROCESSORS : 0); i++){
		for (j=5;j<x_size-5;j++) {
        		n=100;
        		p=in + (i-3)*x_size + j - 1;
        		cp=bp + in[i*x_size+j];
        		n+=*(cp-*p++);
       		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-3; 
		      n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-5;
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-6;
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
      		if (n<max_no){    /* do this test early and often ONLY to save wasted computation */
        			p+=2;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
       			p+=x_size-6;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
        			p+=x_size-5;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
        			p+=x_size-3;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
        		if (n<max_no){
            		x=0;y=0;
            		p=in + (i-3)*x_size + j - 1;
            		c=*(cp-*p++);x-=c;y-=3*c;
            		c=*(cp-*p++);y-=3*c;
            		c=*(cp-*p);x+=c;y-=3*c;
            		p+=x_size-3;   
            		c=*(cp-*p++);x-=2*c;y-=2*c;
            		c=*(cp-*p++);x-=c;y-=2*c;
           			c=*(cp-*p++);y-=2*c;
            		c=*(cp-*p++);x+=c;y-=2*c;
            		c=*(cp-*p);x+=2*c;y-=2*c;
            		p+=x_size-5;   
            		c=*(cp-*p++);x-=3*c;y-=c;
            		c=*(cp-*p++);x-=2*c;y-=c;
            		c=*(cp-*p++);x-=c;y-=c;
           			c=*(cp-*p++);y-=c;
            		c=*(cp-*p++);x+=c;y-=c;
            		c=*(cp-*p++);x+=2*c;y-=c;
            		c=*(cp-*p);x+=3*c;y-=c;
            		p+=x_size-6;
            		c=*(cp-*p++);x-=3*c;
            		c=*(cp-*p++);x-=2*c;
            		c=*(cp-*p);x-=c;
            		p+=2;
            		c=*(cp-*p++);x+=c;
            		c=*(cp-*p++);x+=2*c;
            		c=*(cp-*p);x+=3*c;
            		p+=x_size-6;   
            		c=*(cp-*p++);x-=3*c;y+=c;
            		c=*(cp-*p++);x-=2*c;y+=c;
            		c=*(cp-*p++);x-=c;y+=c;
            		c=*(cp-*p++);y+=c;
            		c=*(cp-*p++);x+=c;y+=c;
            		c=*(cp-*p++);x+=2*c;y+=c;
            		c=*(cp-*p);x+=3*c;y+=c;
            		p+=x_size-5;
            		c=*(cp-*p++);x-=2*c;y+=2*c;
            		c=*(cp-*p++);x-=c;y+=2*c;
           			c=*(cp-*p++);y+=2*c;
            		c=*(cp-*p++);x+=c;y+=2*c;
            		c=*(cp-*p);x+=2*c;y+=2*c;
            		p+=x_size-3;
            		c=*(cp-*p++);x-=c;y+=3*c;
            		c=*(cp-*p++);y+=3*c;
            		c=*(cp-*p);x+=c;y+=3*c;
            		xx=x*x;
            		yy=y*y;
            		sq=xx+yy;
            		if ( sq > ((n*n)/2) ){
              			if(yy<xx) {
                				divide=(float)y/(float)abs(x);
                				sq=abs(x)/x;
                				sq=*(cp-in[(i+FTOI(divide))*x_size+j+sq]) + *(cp-in[(i+FTOI(2*divide))*x_size+j+2*sq]) + *(cp-in[(i+FTOI(3*divide))*x_size+j+3*sq]);
              			} else {
                				divide=(float)x/(float)abs(y);
                				sq=abs(y)/y;
                				sq=*(cp-in[(i+sq)*x_size+j+FTOI(divide)]) + *(cp-in[(i+2*sq)*x_size+j+FTOI(2*divide)]) + *(cp-in[(i+3*sq)*x_size+j+FTOI(3*divide)]);}

              				if(sq>290){
                					r[i*x_size+j] = max_no-n;
                					cgx[i*x_size+j] = (51*x)/n;
                					cgy[i*x_size+j] = (51*y)/n;}
            					}
					}
			}}}}}}}}}}}}}}}}}}
		}	/* 2nd for */	
	}	/* 1st for */
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "r[]" value*/
	//int z = (y_size-5-5)/PROCESSORS * (x_size-5-5) *myID;
	n=0;
	//n=z;
	/* Process for (i=5;i<y_size-5;i++), and residue process is on the last site*/
	for (i=5+(y_size-5-5)/PROCESSORS*myID; i<5+(y_size-5-5)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-5-5)%PROCESSORS!=0 ? (y_size-5-5)%PROCESSORS : 0); i++){
		for (j=5;j<x_size-5;j++) {
			x = r[i*x_size+j];
			if (x>0)  {
          			/* 5x5 mask */
				#ifdef FIVE_SUPP
          			if (	(x>r[(i-1)*x_size+j+2]) &&
              			(x>r[(i  )*x_size+j+1]) &&
              			(x>r[(i  )*x_size+j+2]) &&
              			(x>r[(i+1)*x_size+j-1]) &&
              			(x>r[(i+1)*x_size+j  ]) &&
              			(x>r[(i+1)*x_size+j+1]) &&
              			(x>r[(i+1)*x_size+j+2]) &&
              			(x>r[(i+2)*x_size+j-2]) &&
              			(x>r[(i+2)*x_size+j-1]) &&
              			(x>r[(i+2)*x_size+j  ]) &&
              			(x>r[(i+2)*x_size+j+1]) &&
              			(x>r[(i+2)*x_size+j+2]) &&
              			(x>=r[(i-2)*x_size+j-2]) &&
              			(x>=r[(i-2)*x_size+j-1]) &&
              			(x>=r[(i-2)*x_size+j  ]) &&
              			(x>=r[(i-2)*x_size+j+1]) &&
              			(x>=r[(i-2)*x_size+j+2]) &&
              			(x>=r[(i-1)*x_size+j-2]) &&
              			(x>=r[(i-1)*x_size+j-1]) &&
	      			(x>=r[(i-1)*x_size+j  ]) &&
	      			(x>=r[(i-1)*x_size+j+1]) &&
	      			(x>=r[(i  )*x_size+j-2]) &&
	      			(x>=r[(i  )*x_size+j-1]) &&
	      			(x>=r[(i+1)*x_size+j-2]) )
				#endif
				#ifdef SEVEN_SUPP
          				if (	(x>r[(i-3)*x_size+j-3]) &&
                				(x>r[(i-3)*x_size+j-2]) &&
                				(x>r[(i-3)*x_size+j-1]) &&
                				(x>r[(i-3)*x_size+j  ]) &&
                				(x>r[(i-3)*x_size+j+1]) &&
                				(x>r[(i-3)*x_size+j+2]) &&
                				(x>r[(i-3)*x_size+j+3]) &&
                				(x>r[(i-2)*x_size+j-3]) &&
                				(x>r[(i-2)*x_size+j-2]) &&
                				(x>r[(i-2)*x_size+j-1]) &&
                				(x>r[(i-2)*x_size+j  ]) &&
                				(x>r[(i-2)*x_size+j+1]) &&
                				(x>r[(i-2)*x_size+j+2]) &&
                				(x>r[(i-2)*x_size+j+3]) &&
                				(x>r[(i-1)*x_size+j-3]) &&
                				(x>r[(i-1)*x_size+j-2]) &&
                				(x>r[(i-1)*x_size+j-1]) &&
                				(x>r[(i-1)*x_size+j  ]) &&
                				(x>r[(i-1)*x_size+j+1]) &&
                				(x>r[(i-1)*x_size+j+2]) &&
                				(x>r[(i-1)*x_size+j+3]) &&
                				(x>r[(i)*x_size+j-3]) &&
                				(x>r[(i)*x_size+j-2]) &&
                				(x>r[(i)*x_size+j-1]) &&
                				(x>=r[(i)*x_size+j+1]) &&
                				(x>=r[(i)*x_size+j+2]) &&
                				(x>=r[(i)*x_size+j+3]) &&
                				(x>=r[(i+1)*x_size+j-3]) &&
                				(x>=r[(i+1)*x_size+j-2]) &&
                				(x>=r[(i+1)*x_size+j-1]) &&
                				(x>=r[(i+1)*x_size+j  ]) &&
                				(x>=r[(i+1)*x_size+j+1]) &&
                				(x>=r[(i+1)*x_size+j+2]) &&
                				(x>=r[(i+1)*x_size+j+3]) &&
                				(x>=r[(i+2)*x_size+j-3]) &&
                				(x>=r[(i+2)*x_size+j-2]) &&
                				(x>=r[(i+2)*x_size+j-1]) &&
                				(x>=r[(i+2)*x_size+j  ]) &&
                				(x>=r[(i+2)*x_size+j+1]) &&
                				(x>=r[(i+2)*x_size+j+2]) &&
                				(x>=r[(i+2)*x_size+j+3]) &&
                				(x>=r[(i+3)*x_size+j-3]) &&
                				(x>=r[(i+3)*x_size+j-2]) &&
                				(x>=r[(i+3)*x_size+j-1]) &&
                				(x>=r[(i+3)*x_size+j  ]) &&
                				(x>=r[(i+3)*x_size+j+1]) &&
                				(x>=r[(i+3)*x_size+j+2]) &&
                				(x>=r[(i+3)*x_size+j+3]) )
				#endif
					{
						corner_list[n].info=0;
						corner_list[n].x=j;
						corner_list[n].y=i;
						corner_list[n].dx=cgx[i*x_size+j];
						corner_list[n].dy=cgy[i*x_size+j];
						corner_list[n].I=in[i*x_size+j];
						n++;
						if(n==MAX_CORNERS){
      						fprintf(stderr,"Too many corners.\n");
      						exit(1);
         					}
					}
			}	/* if(x>0) */
		}	/* 2nd for */	
	}
	*(ns+myID) = n;
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "n" value*/
	int previous_n = 0;
	for(i=0;i<myID;i++){
		previous_n += *(ns+i);
	}
	memcpy(funcArgs->corner_list+previous_n,corner_list,sizeof(CORNER_LIST)*n);
	if(myID+1==PROCESSORS){	//the last n should be processed on the last site
		//corner_list[n].info=7;
		(*(funcArgs->corner_list+previous_n+n)).info = 7; //the last n is i of the last site
	}
	return NULL;		
}



/* {{{ susan_quick(in,r,sf,max_no,corner_list) */
void susan_corners_quick(in,r,bp,max_no,corner_list,x_size,y_size) uchar *in, *bp; int *r, max_no, x_size, y_size; CORNER_LIST* corner_list; {
  	memset (r,0,x_size * y_size * sizeof(int));
	FUNCARGS* funcArgs = (FUNCARGS*)malloc(sizeof(FUNCARGS));
	funcArgs->y_size = y_size;
	funcArgs->x_size = x_size;
	funcArgs->r = r;
	funcArgs->bp_corners = bp;
	funcArgs->max_no = max_no;
	funcArgs->corner_list = corner_list;
	funcArgs->in = in;
	int ns[PROCESSORS];
	funcArgs->ns = ns;
	/*###### Parallelization Work #####
  	for (i=7;i<y_size-7;i++){		
		.....................................    		
	}
	*/
	/*###### Parallelization Work, notice n++ #####
  	for (i=7;i<y_size-7;i++){		
    		................................................
	}
	*/
	startBarrier(&myBarrier);	/* Start barrier 	*/
	startThreads(9,funcArgs);
	free(funcArgs);
}
/* }}} */
/* }}} */
void* susan_corners_quick_thread(void* threadArgs){
	int   n,x,y,i,j;
	uchar *p,*cp;
	THREADARGS* args = (THREADARGS*)threadArgs;
	int myID = args->id;
	FUNCARGS* funcArgs = args->funcArgs;
	int* ns = funcArgs->ns;
	int y_size = funcArgs->y_size;
	int x_size = funcArgs->x_size;
	int *r = funcArgs->r;
	uchar* bp = funcArgs->bp_corners;
	int max_no = funcArgs->max_no;
	CORNER_LIST corner_list[MAX_CORNERS];
	memcpy(corner_list,funcArgs->corner_list,sizeof(CORNER_LIST)*MAX_CORNERS);
	uchar* in = funcArgs->in;		
/* Process 	for (i=7;i<y_size-7;i++), and residue process is on the last site*/
	for (i=7+(y_size-7-7)/PROCESSORS*myID; i<7+(y_size-7-7)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-7-7)%PROCESSORS!=0 ? (y_size-7-7)%PROCESSORS : 0); i++){
		for (j=7;j<x_size-7;j++) {
        		n=100;
        		p=in + (i-3)*x_size + j - 1;
        		cp=bp + in[i*x_size+j];
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-3;
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-5;
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
        		p+=x_size-6;
        		n+=*(cp-*p++);
        		n+=*(cp-*p++);
        		n+=*(cp-*p);
      		if (n<max_no){
        			p+=2;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
        			p+=x_size-6;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
        			p+=x_size-5;
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
      		if (n<max_no){
        			p+=x_size-3;
       			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p++);
      		if (n<max_no){
        			n+=*(cp-*p);
        		if (n<max_no)
          			r[i*x_size+j] = max_no-n;
			}}}}}}}}}}}}}}}}}}
		}			
	}
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "r[]" value*/
	n=0;
	/* Process 	for (i=7;i<y_size-7;i++), and residue process is on the last site*/
	for (i=7+(y_size-7-7)/PROCESSORS*myID; i<7+(y_size-7-7)/PROCESSORS*(myID+1) + (myID+1==PROCESSORS && (y_size-7-7)%PROCESSORS!=0 ? (y_size-7-7)%PROCESSORS : 0); i++){
		for (j=7;j<x_size-7;j++) {
      		x = r[i*x_size+j];
      		if (x>0)  {
      			/* 5x5 mask */
				#ifdef FIVE_SUPP
      			if (	(x>r[(i-1)*x_size+j+2]) &&
              			(x>r[(i  )*x_size+j+1]) &&
              			(x>r[(i  )*x_size+j+2]) &&
              			(x>r[(i+1)*x_size+j-1]) &&
              			(x>r[(i+1)*x_size+j  ]) &&
              			(x>r[(i+1)*x_size+j+1]) &&
              			(x>r[(i+1)*x_size+j+2]) &&
              			(x>r[(i+2)*x_size+j-2]) &&
              			(x>r[(i+2)*x_size+j-1]) &&
              			(x>r[(i+2)*x_size+j  ]) &&
              			(x>r[(i+2)*x_size+j+1]) &&
              			(x>r[(i+2)*x_size+j+2]) &&
              			(x>=r[(i-2)*x_size+j-2]) &&
              			(x>=r[(i-2)*x_size+j-1]) &&
              			(x>=r[(i-2)*x_size+j  ]) &&
              			(x>=r[(i-2)*x_size+j+1]) &&
              			(x>=r[(i-2)*x_size+j+2]) &&
              			(x>=r[(i-1)*x_size+j-2]) &&
              			(x>=r[(i-1)*x_size+j-1]) &&
	      			(x>=r[(i-1)*x_size+j  ]) &&
	      			(x>=r[(i-1)*x_size+j+1]) &&
	      			(x>=r[(i  )*x_size+j-2]) &&
	      			(x>=r[(i  )*x_size+j-1]) &&
	      			(x>=r[(i+1)*x_size+j-2]) )
					#endif
					#ifdef SEVEN_SUPP
          				if (	(x>r[(i-3)*x_size+j-3]) &&
                				(x>r[(i-3)*x_size+j-2]) &&
                				(x>r[(i-3)*x_size+j-1]) &&
                				(x>r[(i-3)*x_size+j  ]) &&
                				(x>r[(i-3)*x_size+j+1]) &&
                				(x>r[(i-3)*x_size+j+2]) &&
                				(x>r[(i-3)*x_size+j+3]) &&
                				(x>r[(i-2)*x_size+j-3]) &&
                				(x>r[(i-2)*x_size+j-2]) &&
                				(x>r[(i-2)*x_size+j-1]) &&
                				(x>r[(i-2)*x_size+j  ]) &&
                				(x>r[(i-2)*x_size+j+1]) &&
                				(x>r[(i-2)*x_size+j+2]) &&
                				(x>r[(i-2)*x_size+j+3]) &&
                				(x>r[(i-1)*x_size+j-3]) &&
                				(x>r[(i-1)*x_size+j-2]) &&
                				(x>r[(i-1)*x_size+j-1]) &&
                				(x>r[(i-1)*x_size+j  ]) &&
                				(x>r[(i-1)*x_size+j+1]) &&
                				(x>r[(i-1)*x_size+j+2]) &&
                				(x>r[(i-1)*x_size+j+3]) &&
                				(x>r[(i)*x_size+j-3]) &&
                				(x>r[(i)*x_size+j-2]) &&
                				(x>r[(i)*x_size+j-1]) &&
                				(x>=r[(i)*x_size+j+1]) &&
                				(x>=r[(i)*x_size+j+2]) &&
                				(x>=r[(i)*x_size+j+3]) &&
                				(x>=r[(i+1)*x_size+j-3]) &&
                				(x>=r[(i+1)*x_size+j-2]) &&
                				(x>=r[(i+1)*x_size+j-1]) &&
                				(x>=r[(i+1)*x_size+j  ]) &&
                				(x>=r[(i+1)*x_size+j+1]) &&
                				(x>=r[(i+1)*x_size+j+2]) &&
                				(x>=r[(i+1)*x_size+j+3]) &&
                				(x>=r[(i+2)*x_size+j-3]) &&
                				(x>=r[(i+2)*x_size+j-2]) &&
                				(x>=r[(i+2)*x_size+j-1]) &&
                				(x>=r[(i+2)*x_size+j  ]) &&
                				(x>=r[(i+2)*x_size+j+1]) &&
                				(x>=r[(i+2)*x_size+j+2]) &&
                				(x>=r[(i+2)*x_size+j+3]) &&
                				(x>=r[(i+3)*x_size+j-3]) &&
                				(x>=r[(i+3)*x_size+j-2]) &&
                				(x>=r[(i+3)*x_size+j-1]) &&
                				(x>=r[(i+3)*x_size+j  ]) &&
                				(x>=r[(i+3)*x_size+j+1]) &&
                				(x>=r[(i+3)*x_size+j+2]) &&
                				(x>=r[(i+3)*x_size+j+3]) )
					#endif
					{
						corner_list[n].info=0;
						corner_list[n].x=j;
						corner_list[n].y=i;
						x = in[(i-2)*x_size+j-2] + in[(i-2)*x_size+j-1] + in[(i-2)*x_size+j] + in[(i-2)*x_size+j+1] + in[(i-2)*x_size+j+2] +
    						in[(i-1)*x_size+j-2] + in[(i-1)*x_size+j-1] + in[(i-1)*x_size+j] + in[(i-1)*x_size+j+1] + in[(i-1)*x_size+j+2] +
   						in[(i  )*x_size+j-2] + in[(i  )*x_size+j-1] + in[(i  )*x_size+j] + in[(i  )*x_size+j+1] + in[(i  )*x_size+j+2] +
    						in[(i+1)*x_size+j-2] + in[(i+1)*x_size+j-1] + in[(i+1)*x_size+j] + in[(i+1)*x_size+j+1] + in[(i+1)*x_size+j+2] +
    						in[(i+2)*x_size+j-2] + in[(i+2)*x_size+j-1] + in[(i+2)*x_size+j] + in[(i+2)*x_size+j+1] + in[(i+2)*x_size+j+2];
						corner_list[n].I=x/25;
						/*corner_list[n].I=in[i*x_size+j];*/
						x = in[(i-2)*x_size+j+2] + in[(i-1)*x_size+j+2] + in[(i)*x_size+j+2] + in[(i+1)*x_size+j+2] + in[(i+2)*x_size+j+2] - (in[(i-2)*x_size+j-2] + in[(i-1)*x_size+j-2] + in[(i)*x_size+j-2] + in[(i+1)*x_size+j-2] + in[(i+2)*x_size+j-2]);
						x += x + in[(i-2)*x_size+j+1] + in[(i-1)*x_size+j+1] + in[(i)*x_size+j+1] + in[(i+1)*x_size+j+1] + in[(i+2)*x_size+j+1] - (in[(i-2)*x_size+j-1] + in[(i-1)*x_size+j-1] + in[(i)*x_size+j-1] + in[(i+1)*x_size+j-1] + in[(i+2)*x_size+j-1]);
						y = in[(i+2)*x_size+j-2] + in[(i+2)*x_size+j-1] + in[(i+2)*x_size+j] + in[(i+2)*x_size+j+1] + in[(i+2)*x_size+j+2] - (in[(i-2)*x_size+j-2] + in[(i-2)*x_size+j-1] + in[(i-2)*x_size+j] + in[(i-2)*x_size+j+1] + in[(i-2)*x_size+j+2]);
						y += y + in[(i+1)*x_size+j-2] + in[(i+1)*x_size+j-1] + in[(i+1)*x_size+j] + in[(i+1)*x_size+j+1] + in[(i+1)*x_size+j+2] - (in[(i-1)*x_size+j-2] + in[(i-1)*x_size+j-1] + in[(i-1)*x_size+j] + in[(i-1)*x_size+j+1] + in[(i-1)*x_size+j+2]);
						corner_list[n].dx=x/15;
						corner_list[n].dy=y/15;
						n++;
						if(n==MAX_CORNERS){
      						fprintf(stderr,"Too many corners.\n");
      						exit(1);
         					}
					}
			}	/* if(x>0) */
		}	/* 2nd for */	
	}	/* 1st for */
	*(ns+myID) = n;
	actuateBarrier(&myBarrier);	/* Actuate a barrier for "n" value*/
	int previous_n = 0;
	for(i=0;i<myID;i++){
		previous_n += *(ns+i);
	}
	//j=0;
	//for(i=previous_n;i<previous_n+*(ns+myID);i++){
		//*(funcArgs->corner_list+i) = corner_list[j];
		//j++;
	//}
	memcpy(funcArgs->corner_list+previous_n,corner_list,sizeof(CORNER_LIST)*n);
	if(myID+1==PROCESSORS){	//the last n should be processed on the last site
		//corner_list[n].info=7;
		(*(funcArgs->corner_list+previous_n+n)).info = 7; //the last n is i of the last site
	}
	return NULL;
}



/* {{{ main(argc, argv) */
int main() {
	/* {{{ vars */
	char filename [80], *tcp;
	uchar *in, *bp, *mid;
	float dt=4.0;
	int *r, argindex=3, bt=20, principle=0, thin_post_proc=1, three_by_three=0, drawing_mode=0, susan_quick=0;
	int max_no_corners=1850, max_no_edges=2650, mode = 0;
      int i, x_size, y_size;
	CORNER_LIST corner_list[MAX_CORNERS]; //CORNER_LIST corner_list;
	/* }}} */
	if (argc<3){
		usage();
	}
    fakeFile = test_data;
	/* Read image data */
	get_image(argv[1],&in,&x_size,&y_size);
	/* {{{ look at options */
	while (argindex < argc){
		tcp = argv[argindex];
		if (*tcp == '-'){
			switch (*++tcp){
				case 's': /* smoothing */
         				mode=0;
					break;
				case 'e': /* edges */
					mode=1;
					break;
				case 'c': /* corners */
					mode=2;
					break;
				case 'p': /* principle */
					principle=1;
					break;
				case 'n': /* thinning post processing */
					thin_post_proc=0;
					break;
				case 'b': /* simple drawing mode */
					drawing_mode=1;
					break;
				case '3': /* 3x3 flat mask */
					three_by_three=1;
					break;
				case 'q': /* quick susan mask */
					susan_quick=1;
					break;
				case 'd': /* distance threshold */
					if (++argindex >= argc){
						printf ("No argument following -d\n");
	    					exit(0);
					}
	  				dt=atof(argv[argindex]);
          				if (dt<0){
						three_by_three=1;
					}
	  				break;
				case 't': /* brightness threshold */
					if (++argindex >= argc){
						printf ("No argument following -t\n");
	    					exit(0);
					}
	  				bt=atoi(argv[argindex]);
	  				break;
      			}	    
      	}else{
        		usage();
		}
    		argindex++;
  	}
  	if ( (principle==1) && (mode==0) ){
    		mode=1;
	}
	/* }}} */
  	/* {{{ main processing */

	switch (mode){
		case 0: /* {{{ smoothing */
			setup_brightness_lut(&bp,bt,2);									/*##### Parallelization ##### has done!! */
      		susan_smoothing(three_by_three,in,dt,x_size,y_size,bp);				/*##### Parallelization ##### & enlarge() has done!! */
      		break;
			/* }}} */
    		case 1: /* {{{ edges */
      		r = (int *) malloc(x_size * y_size * sizeof(int));
      		setup_brightness_lut(&bp,bt,6);									
			if (principle){
        			if (three_by_three){
					susan_principle_small(in,r,bp,max_no_edges,x_size,y_size); 			/*##### Parallelization ##### has done!! */
				}else{
					susan_principle(in,r,bp,max_no_edges,x_size,y_size);				/*##### Parallelization ##### has done!! */
				}
				int_to_uchar(r,in,x_size*y_size);								/*##### Parallelization ##### has done!! */
      		}else{
				mid = (uchar *)malloc(x_size*y_size);
				memset (mid,100,x_size * y_size); /* note not set to zero */
				if (three_by_three){
					susan_edges_small(in,r,mid,bp,max_no_edges,x_size,y_size);			/*##### Parallelization ##### has done!! */
				}else{
					susan_edges(in,r,mid,bp,max_no_edges,x_size,y_size);				/*##### Parallelization ##### has done!! */
				}
        			if(thin_post_proc){
          				susan_thin(r,mid,x_size,y_size);							/*##### Parallelization ##### has done!! */
				}
        			edge_draw(in,mid,x_size,y_size,drawing_mode);						/*##### Parallelization ##### has done!! */
      			}
      		break;
			/* }}} */
		case 2: /* {{{ corners */
			r   = (int *) malloc(x_size * y_size * sizeof(int));
      		setup_brightness_lut(&bp,bt,6);									
			if (principle){
				susan_principle(in,r,bp,max_no_corners,x_size,y_size);				
        			int_to_uchar(r,in,x_size*y_size);								
      		}else{
        			if(susan_quick){
          				susan_corners_quick(in,r,bp,max_no_corners,corner_list,x_size,y_size);	/*##### Parallelization ##### has done!! */
        			}else{
          				susan_corners(in,r,bp,max_no_corners,corner_list,x_size,y_size);		/*##### Parallelization ##### has done!! */
				}
        			corner_draw(in,corner_list,x_size,drawing_mode);					/*##### Parallelization ##### has done!! */
      			}
      		break;
			/* }}} */
	}    
	/* }}} */
  	put_image(in,x_size,y_size);
}
/* }}} */
