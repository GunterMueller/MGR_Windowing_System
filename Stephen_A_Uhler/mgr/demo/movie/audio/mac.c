/* sample MGR audio control device for 4.1 */

#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sun/audioio.h>
#include "term.h"

#define CNTL		"/dev/audioctl"
#define ICON		"cntl"							/* icon name */
#define INCR		25									/* volumen increment */
#define dprintf	if(debug)fprintf

struct pos {
	int xmin;		/* min x position */
	int xmax;		/* max x position */
	int ymin;		/* min y position */
	int ymax;		/* max y position */
	};

struct pos pos[] = {
		11,26,10,26,		/* up arrow */
		30,45,10,26,		/* down arrow */
		50,70,10,26,		/* speaker */
		};

char line[100];
int debug;

main(argc,argv)
int argc;
char **argv;
	{
	int fd;		/* audio cntl fd */
	struct audio_info info;
	int clean();
	register int i;
	int x,y;		/* mouse position */
	int w,h;		/* icon size */
	int shape=1;	/* reshape window on startup */

	debug = getenv("DEBUG");

	if ((fd = open(CNTL,2)) <0) {
		fprintf(stderr,"Sorry, %s cant open %s\n",*argv,CNTL);
		exit(2);
		}

	if (ioctl(fd,AUDIO_GETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant get status for %s\n",*argv,CNTL);
		exit(3);
		}

	if (argc>1 && strcmp(argv[1],"-s")==0)
		shape=0;

	m_setup(M_FLUSH);
	signal(SIGINT,clean);
	signal(SIGTERM,clean);
	signal(SIGHUP,clean);
	m_ttyset();
	m_push(P_FLAGS|P_POSITION|P_EVENT);
	m_setmode(M_ABS);
	m_setevent(BUTTON_1,"\rB %p\r");
	m_setevent(BUTTON_2,"\rB %p\r");
	m_setevent(RESHAPE,"\rS\r");
	m_setevent(REDRAW,"\rR\r");
	m_setcursor(9);

	m_bitfile(1,ICON,&w,&h);
	dprintf(stderr,"Got icon %d x %d\n",w,h);

	if (w==0 || h==0) {
		fprintf(stderr,"Cant find icon: %s\n",ICON);
		clean(1);
		}

	get_size(&x,&y,NULL,NULL);
	dprintf(stderr,"Got window position %d,%d\n",x,y);
	if (shape)
		m_shapewindow(x,y,w+10,h+10);
	m_func(B_COPY);
	m_bitcopyto(0,0,w,h,0,0,0,1);

	while(m_gets(line)) {
		switch(*line) {
			case 'S':
				get_size(&x,&y,NULL,NULL);
				m_shapewindow(x,y,w+10,h+10);
				dprintf(stderr,"Got window position %d,%d\n",x,y);
				/* no break */
			case 'R':
				m_bitcopyto(0,0,w,h,0,0,0,1);
				break;
			case 'B':
				sscanf(line+1,"%d %d\n",&x,&y);
				dprintf(stderr,"Got %d %d\n",x,y);

				for(i=0;i<3;i++) {
					if (do_pos(i,x,y) == 0)
                  continue;
					ioctl(fd,AUDIO_GETINFO,&info);
					switch(i) {
						case 0:		/* volume up */
							x = info.play.gain;
							if (x+INCR > 255)
								x += INCR/5;
							else
								x += INCR;
							if (x>255) x = 255;
							info.play.gain = x;
							break;
						case 1:		/* volume down */
							x = info.play.gain;
							if (x-INCR <= 0)
								x -= INCR/5;
							else
								x -= INCR;
							if (x<0) x = 0;
							info.play.gain = x;
							break;
						case 2:		/* switch speaker*/
							if (info.play.port == AUDIO_SPEAKER)
								info.play.port = AUDIO_HEADPHONE;
							else
								info.play.port = AUDIO_SPEAKER;
							break;
						}
					ioctl(fd,AUDIO_SETINFO,&info);
					break;
					}
				break;
			}
		}
	clean(0);
	}

/* do mouse position stuff */

int
do_pos(i,x,y)
int i,x,y;
	{
	dprintf(stderr,"do_pos %d %d,%d\n",i,x,y);
	if (pos[i].xmin <= x && pos[i].xmax >= x &&
 			pos[i].ymin <= y && pos[i].ymax >= y) {
		m_func(B_INVERT);
		m_bitwrite(pos[i].xmin,pos[i].ymin,pos[i].xmax-pos[i].xmin,
					pos[i].ymax-pos[i].ymin);
		usleep(100000);
		m_bitwrite(pos[i].xmin,pos[i].ymin,pos[i].xmax-pos[i].xmin,
					pos[i].ymax-pos[i].ymin);
		return(1);
		}
	else
		return(0);
	}

/*
				info.play.gain = atoi(argv[++i])&255;
				info.record.gain = atoi(argv[++i])&255;
				info.monitor_gain = atoi(argv[++i])&255;
				info.play.port = AUDIO_SPEAKER;
				info.play.port = AUDIO_HEADPHONE;
	if (ioctl(fd,AUDIO_SETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant set status for %s\n",argv[0],CNTL);
		exit(3);
		}
	}
*/

int
clean(n)
int n;
	{
	m_pop();
	m_setcursor(0);
	m_ttyreset();
	exit(n);
	}
