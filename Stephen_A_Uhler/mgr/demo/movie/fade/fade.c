/* fade an image to black (fade1.c) */

#include <sys/time.h>
#include <stdio.h>
#include "bitmap.h"

#define BASE	6
#define SCREEN "/dev/bwtwo0"
#define GET_OPT(i)   \
   strlen(argv[i])>2 ? argv[i]+2 : argv[++i]

/* sleep 100ths of seconds */

#define fsleep(s) \
   { \
   struct timeval time; \
   time.tv_sec = (s)/100; \
   time.tv_usec = ((s)%100)*10000; \
   select(0,0,0,0,&time); \
   }

#define dprintf		if(debug)	fprintf

/* screen size */

#define WIDE	1152
#define HIGH	900

struct rect {
	short rand;			/* random value used to sort by */
	short wait;			/* 100th seconds to wait */
	short x,y;			/* initial coordinates */
	};

struct rect *rects;
int cmp();
int mode=0;						/* drawing mode - used by cmp */
int midx = WIDE/2;
int midy = HIGH/2;
	
extern int no_initial;		/* flag to (share) to not-save initial display */

main(argc,argv)
int argc;
char **argv;
	{
	register int i,j;
	char *getenv(),*malloc();
	char *sname = getenv("SCREEN") ? getenv("SCREEN") : SCREEN;
	int debug = (int) getenv("DEBUG");
	char *index();
	BITMAP *screen;
	BITMAP *map;
	BITMAP *bitmapread();
	FILE *in;						/* for bitmap to fade to */
	register struct rect *r = rects;
	int op = BIT_SRC;				/* bitblit function */
	int begin=0;					/* wait at begginning */
	int end = 0;					/* addional wait at end */
	int nx=10;						/* # rectangles in x direction */
	int ny=10;						/* # rectangles in y direction */
	int delay=3;					/* 100th seconds delay between rects */
	int skip=0;						/* # of rects to skip between delays */
	int count;						/* # of rects */
	int wide,high;					/* size of each rect */
	int rnd=0;						/* randomize time */
	int blend=0;					/* expreimental */
	int final = -1;				/* final entire screen blit */
	int share=0;

	/* get arguments */

	for(i=1;i<argc;i++) {
		if (*argv[i] == '-') {
			switch(argv[i][1]) {
				case 'b':		/* set blend mode, get blend-to bitmap  */
					blend++;
					in = fopen(GET_OPT(i),"r");
					dprintf(stderr,"blending %s\n",in?"OK":"BAD");
					break;
				case 'm':		/* set a drawing mode */
					mode = atoi(GET_OPT(i));
					dprintf(stderr,"Drawing mode %d\n",mode);
					break;
				case 'f':		/* use this blit function */
					op = 0xf&atoi(GET_OPT(i));
					dprintf(stderr,"Drawing function %d\n",op);
					break;
				case 'F':		/* use this as final blit function */
					final = 0xf&atoi(GET_OPT(i));
					break;
				case 'r':		/* randomize delay added between rects in 100ths */
					rnd = atoi(GET_OPT(i));
					break;
				case 'd':		/* delay between rects in 100ths */
					delay = atoi(GET_OPT(i));
					skip = index(argv[i],'/') ? atoi(1+index(argv[i],'/')) : 0;
					dprintf(stderr,"delay is %d 100ths every %d\n",delay,skip);
					break;
				case 'x':		/* # rects in x direction */
					nx = atoi(GET_OPT(i));
					if (nx<1) nx=1;
					break;
				case 'y':		/* # rects in y direction */
					ny = atoi(GET_OPT(i));
					if (ny<1) nx=1;
					break;
				case 't':		/* additional wait at end */
					begin = atoi(GET_OPT(i));
					end = index(argv[i],'/') ? atoi(1+index(argv[i],'/')) : 0;
					dprintf(stderr,"wait is %d secs, then  %d secs\n",begin,end);
					break;
				case 'z':		/* make a log */
					share++;
					dprintf(stderr,"setting capture mode\n");
					break;
				default:
					fprintf(stderr,"Bad flag, try again\n");
					break;
				}
			}
		}

	if (!(screen=bit_open(sname))) {
		fprintf(stderr, "couldn't open %s\n",sname);
		exit(1);
		}

	if (blend && !in) {
		fprintf(stderr, "couldn't open image file\n");
		exit(2);
		}

	if (blend && (map = bitmapread(in))==NULL) {
		fprintf(stderr, "Invalid bitmap format\n");
		exit(3);
		}

	/* generate random rectangles */

	count = nx*ny;
	wide = (WIDE+nx-1)/nx;
	high = (HIGH+ny-1)/ny;
	midx -= wide/2;
	midy -= high/2;

	dprintf(stderr,"%d rects, %d x %d each %d x %d\n", count,nx,ny,wide,high);

	if (count > 10000) {
		fprintf(stderr,"Too many rectangles, sorry\n");
		exit(2);
		}

	rects = (struct rect *) malloc(sizeof(struct rect) * count);

	if (!rects) {
		fprintf(stderr,"Cant malloc %d rects\n",count);
		exit(1);
		}

	srandom(getpid());
	for(r=rects,i=0;i<WIDE;i+=wide)
		for(j=0;j<HIGH;j+=high) {
			r->rand = random();
			r->wait = delay + (rnd>0 ? random()%rnd : 0);
			r->x = i;
			r->y = j;
			r++;
			}

	/* sort into random order */

	qsort(rects,count,sizeof(struct rect),cmp);

	if (share) {
		no_initial = 1;
		start_log(stdout); 
		SEND_TIME();
		if (begin>0) {
			dprintf(stderr,"sleeping %d\n",begin);
			sleep(begin);
			SEND_TIME();
			}
		}

	for(r=rects,i=0;i<count;i++) {
		if (blend)
			bit_blit(screen,r->x,r->y,wide,high,op,map,r->x,r->y);
		else
			bit_blit(screen,r->x,r->y,wide,high,op,NULL,0,0);
		if (r->wait>0 && (skip==0 || i%skip==0)) {
			SEND_TIME();
			fsleep(r->wait);
			}
		r++;
		}

	if (final >=0)
		bit_blit(screen,0,0,WIDE,HIGH,final,blend?map:0,0,0);
	SEND_TIME();
	if (end>0) {
		dprintf(stderr,"sleeping %d\n",end);
		sleep(end);
		SEND_TIME();
		}
   end_log(); 
	}

int smap[] = {3,1,5};

int cmp(x,y)
register struct rect *x, *y;
	{
	int dx,dy,dr;		/* differences in fields */
	int result;
	int scale;
	
	dx = x->x - y->x;
	dy = x->y - y->y;
	dr = x->rand - y-> rand;

	if (mode/BASE == 1) dx = -dx;
	if (mode/BASE == 2) dy = -dy;
	if (mode/BASE == 3) dy = -dy; dx = -dx;


	switch(mode%BASE) {
		case 0:		/* random */
			result = dr;
			break;
		case 1:		/* by x */
			result = dx ? dx : dy;
			break;
		case 2:		/* by y */
			result = dy ? dy : dx;
			break;
		case 3:		/* by x  - semi random */
			result = dx ? dx : dr;
			break;
		case 4:		/* by y  - semi random */
			result = dy ? dy : dr;
			break;
		case 5:		/* by x,y, center out */
			scale =smap[(mode/BASE)%3];
			result = (scale*(x->x-midx)*(x->x-midx) + 3*(x->y-midy)*(x->y-midy)) -
						(scale*(y->x-midx)*(y->x-midx) + 3*(y->y-midy)*(y->y-midy));
			if (mode/BASE==3)
				result = -result;
			break;
		}

	return (result);
	}
