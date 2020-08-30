/* sample MGR audio control device for 4.1 */

#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sun/audioio.h>
#include "term.h"

#define CNTL		"/dev/audioctl"
#define ICON		"select"							/* icon name */
#define dprintf	if(debug)fprintf

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

	debug = getenv("DEBUG");

	if ((fd = open(CNTL,2)) <0) {
		fprintf(stderr,"Sorry, %s cant open %s\n",*argv,CNTL);
		exit(2);
		}

	if (ioctl(fd,AUDIO_GETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant get status for %s\n",*argv,CNTL);
		exit(3);
		}

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
	m_shapewindow(x,y,w+10,h+10);
	m_func(B_COPY);
	m_bitcopyto(0,0,w,h,0,0,0,1);

	while(m_gets(line)) {
		switch(*line) {
			case 'S':
				get_size(x,y,NULL,NULL);
				m_shapewindow(&x,&y,w+10,h+10);
				dprintf(stderr,"Got window position %d,%d\n",x,y);
				/* no break */
			case 'R':
				m_bitcopyto(0,0,w,h,0,0,0,1);
				break;
			case 'B':
				sscanf(line+1,"%d %d\n",&x,&y);
				dprintf(stderr,"Got %d %d\n",x,y);
				if (x<0 || y < 0 || x > w || y > h)
					break;

				AUDIO_INITINFO(&info);
				if (y < h/2)
					info.play.port = AUDIO_HEADPHONE;
				else
					info.play.port = AUDIO_SPEAKER;
				ioctl(fd,AUDIO_SETINFO,&info);
				break;
			}
		}
	clean(0);
	}

int
clean(n)
int n;
	{
	m_pop();
	m_setcursor(0);
	m_ttyreset();
	exit(n);
	}
