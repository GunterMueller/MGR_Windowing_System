/* show strength of audio signals */

#include <fcntl.h>
#include <sys/signal.h>
#include <stdio.h>
#include "term.h"

#define dprintf if (debug)fprintf
#define MIN(x,y)	((x)<(y)?(x):(y))
#define MAX(x,y)	((x)<(y)?(y):(x))

int debug = 0;

/*		conversion tables: */

extern short I2sb[];				/*	 mu-law to signed 13 bit binary */

int clean();
int fds[10];						/* fd's to display */

main(argc,argv)
int argc;
char **argv;
	{
	int x=0;				/* x coord */
	int y1,y2;				/* y coords */
	int chunk = 80;		/* bytes/chunk */
	int w,h;					/* window coords */
	int done=0;
	register int i=0;		/* # of total samples */

	debug = getenv("DEBUG");
	signal(SIGINT,clean);
	m_setup(0);
	m_setmode(M_ABS);
	m_setcursor(9);
	m_func(B_SET);
	get_size(0,0,&w,&h);

	if (argc<2 || argc >10) {
		fprintf(stderr,"usage: %s [-c#] file...\n",*argv);
		exit(1);
		}
	if (strncmp(argv[1],"-c",2)==0) {
		chunk = atoi(argv[1]+2);
		argc--, argv++;
		}
	argc--, argv++;
	
	for(i=0;i<argc;i++) {
		if (strcmp(argv[i],"-")==0)
			fds[i] = 0;
		else
			fds[i] = open(argv[i],0);
		dprintf(stderr,"File %d from %s = %d\n",i,argv[i],fds[i]);
		}

	m_func(B_SET);
	if (!debug) m_clear();
	while(!done) {
		done=1;
		for(i=0;i<argc;i++) {
			if (fds[i] < 0)
				continue;
			done=0;
			if (get_limit(fds[i],&y1,&y2,chunk)<=0)  {
				close(fds[i]);
				fds[i] = -1;
				dprintf(stderr,"%s is done\n",argv[i]);
				}
			else
				draw(x,y1,y2,h,i,argc);
			}
		if (++x == w) {
			x = 0;
			m_clear();
			/* get_size(0,0,&w,&h); */
			}
		m_flush();
		}
	clean(0);
	}

clean(n)
int n;
	{
	m_pop();
	m_setcursor(0);
	exit(n);
	}

unsigned char buff[8000];

int
get_limit(fd,min,max,count)
int fd;
int *min, *max;
int count;
	{
	register int c;
	int cnt = read(fd,buff,count);
	register unsigned char *start = buff, *end = buff+cnt;
	register int low = 9999, high = -9999;

	while(start<end) {
		c =  I2sb[*start++];
		if (c < low) low = c;
		if (c > high) high = c;
		}

	*min = low;
	*max = high;
	return(cnt);
	}

/* draw a line */

int
draw(x,y1,y2,h,i,max)
int x;		/* x coord */
int y1,y2;	/* y coords */
int h;		/* total window height */
int i;		/* which graph */
int max;	/* # of graphs */
	{
	int a = (y1+4096)*h/(8192*max);
	int b = (y2+4096)*h/(8192*max);
	m_line(x,a+i*h/max,x,b+i*h/max);
	}
