/* both record and save video  - 4.1 version */

#include <sys/time.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sun/audioio.h>

#define NAME		"/usr/tmp/record"		/* default file name template */
#define COMPRESS	"compress"				/* name of compression program */
#define HUNK		4096						/* arbitrary */
#define GAIN		255						/* input gain (%) */

char line[128];
char buff[HUNK];	/* input buffer */
FILE *a_fd;			/* audio output */
FILE *v_fd;			/* video output */
int fd;							/* dev audio fd */

struct timeval time = {0, 200000};		/* 2/10 seconds */

int clean();

main(argc,argv)
int argc;
char **argv;
	{	
	char *name;
	int pid = getpid();			/* unique identifier */
	int done=0;							
	int mask;						/* select mask */
	int count=0;					/* read count */
	struct audio_info info;		/* audio ioctl structure */
	char *getenv();
	int arg;

	if (argc>1)
		name = argv[1];
	else
		name = NAME;

	/* get video file */

	sprintf(line,"%s > %s.%d.Z",COMPRESS,name,pid);
	if ((v_fd = popen(line,"w")) == NULL) {
		fprintf(stderr,"Cant open %s\n",line);
		exit(1);
		}

	/* get audio file */

	sprintf(line,"%s.%d.au",name,pid);
	if ((a_fd = fopen(line,"w")) == NULL) {
		fprintf(stderr,"Cant open %s\n",line);
		exit(1);
		}
		
	/* open dev audio */

	if ((fd = open("/dev/audio",O_RDONLY|O_NDELAY)) <0) {
		fprintf(stderr,"Sorry, %s cant open audio\n",*argv);
		exit(2);
		}

	fcntl(fd,F_SETFL,fcntl(fd,F_GETFL,0)|FNDELAY);
	if (ioctl(fd,AUDIO_GETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant get status for audio\n",*argv);
		exit(3);
		}
	if (getenv("GAIN") && (arg=atoi(getenv("GAIN")))>0) {
		info.play.gain = arg;
		ioctl(fd,AUDIO_SETINFO,&info);
		}

	/* catch signals */

	signal(SIGHUP,clean);
	signal(SIGTERM,clean);
	signal(SIGINT,clean);
	
	/* start processing */

	while (!done) {
		mask = 1<<(0);		/* video input (stdin) */
		switch(select(32,&mask,0,0,&time)) {
			case 1:						/* can read input */
				count = read(0,buff,HUNK);
				if (count > 0)
					fwrite(buff,1,count,v_fd);
				else {
					done++;
					break;
					}
				/* no break */
			case 0:						/* time out */
				count = read(fd,buff,HUNK);
				if (count>0)
					fwrite(buff,1,count,a_fd);
				break;
			}
		}

	clean(0);
	}

/* clean up and exit */

int clean(n)
int n;
	{
	pclose(v_fd);
	fclose(a_fd);
	close(fd);
	exit(n);
	}
