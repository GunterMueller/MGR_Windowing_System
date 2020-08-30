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

main(argc,argv)
int argc;
char **argv;
	{
	int x=0;				/* x coord */
	int y1,y2;				/* y coords */
	int chunk = 80;		/* bytes/chunk */
	int w,h;					/* window coords */
	register int i=0;		/* # of total samples */
	int n;
	int pause = getenv("PAUSE");

	signal(SIGINT,clean);
	m_setup(0);
	m_setmode(M_ABS);
	m_setcursor(9);
	m_func(B_SET);
	get_size(0,0,&w,&h);
	h /=2;

	if (argc>1)
		chunk = atoi(argv[1]);
	
	m_func(B_SET);
	m_clear();
	m_bitwrite(0,h,1024,1);
	while ((i+=n=get_limit(0,&y1,&y2,chunk))>0 && n>0) {
		if (i>8000) {
			m_bitwrite(x,0,1,2*h);
			i-=8000;
			}
		else
			m_bitwrite(x,h+y1*h/4096,1,(y2-y1)*h/4096 );
		if (++x == w) {
			x = 0;
			if (pause) {
				m_bell();
				m_flush();
				m_getchar();
				}
			m_clear();
			get_size(0,0,&w,&h);
			h /=2;
			m_bitwrite(0,h,1024,1);
			m_flush();
			}
		else if (x&15==0)
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
