/* watch for changes in the audio device */

#include <sys/ioctl.h>
#include <sys/signal.h>
#include <stropts.h>
#include <sun/audioio.h>
#include <stdio.h>

#define CNTL		"/dev/audioctl"

struct audio_info old,new;
int fd;		/* audiuo cntl fd */

main(argc,argv)
int argc;
char **argv;
	{
	register int i;
	int poll();

	if ((fd = open(CNTL,2)) <0) {
		fprintf(stderr,"Sorry, %s cant open %s\n",*argv,CNTL);
		exit(2);
		}
		
	if (ioctl(fd,AUDIO_GETINFO,&old) <0) {
		fprintf(stderr,"Sorry, %s cant get status for %s\n",*argv,CNTL);
		exit(3);
		}

	ioctl(fd,I_SETSIG,S_MSG);		/* send signals on changes */

	signal(SIGPOLL,poll);

	while(1)
		sleep(1);
	}

/* see what changed, print it out */

#define DIFF(x)	(old.x != new.x)
#define P(x)		old.x,new.x

int
poll(n)
int n;
	{
	ioctl(fd,AUDIO_GETINFO,&new);

	if (DIFF(play.gain))
		printf("Volume %d -> %d\n",P(play.gain));
	if (DIFF(record.gain))
		printf("Record volume %d -> %d\n",P(record.gain));
	if (DIFF(play.port))
		printf("Output port %d -> %d\n",P(play.port));
	if (DIFF(play.active))
		printf("Output Active %d -> %d\n",P(play.active));
	if (DIFF(record.active))
		printf("Input Active %d -> %d\n",P(record.active));
	if (DIFF(play.pause))
		printf("Play Paused %d -> %d\n",P(play.pause));
	if (DIFF(record.pause))
		printf("Input Paused %d -> %d\n",P(record.pause));
	if (DIFF(play.waiting))
		printf("Someone waiting to play %d -> %d\n",P(play.waiting));
	if (DIFF(record.waiting))
		printf("Someone waiting record %d -> %d\n",P(record.waiting));
	if (DIFF(record.error))
		printf("playing error %d -> %d\n",P(record.error));
	if (DIFF(record.error))
		printf("recording error %d -> %d\n",P(record.error));
	if (DIFF(play.samples) && new.play.samples>old.play.samples)
		printf("played %d bytes\n",new.play.samples-old.play.samples);
	if (DIFF(record.samples) && new.record.samples>old.record.samples)
		printf("recorded %d bytes\n",new.record.samples-old.record.samples);
	old=new;
	return(0);
	}
