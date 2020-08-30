/* sample audio control device for 4.1 */

#include <sys/ioctl.h>
#include <sun/audioio.h>
#include <stdio.h>

#define CNTL		"/dev/audioctl"

struct cmd {
	char *name;		/* command name */
	int arg;			/* # of args */
	};

struct cmd cmd[] = {
   "status", 0,
   "volume", 1,
	"record", 1,
	"monitor", 1,
	"internal", 0,
	"external", 0,
	NULL,0
	};

   

main(argc,argv)
int argc;
char **argv;
	{
	int fd;		/* audiuo cntl fd */
	struct audio_info info;
	register int i;

	if (argc <2) {
		fprintf(stderr,"usage: %s <stuff>\n",*argv);
		for(i=0;cmd[i].name;i++)
			printf("	%s%s\n",cmd[i].name,cmd[i].arg ? " <value>":"");
		exit(1);
		}

	if ((fd = open(CNTL,2)) <0) {
		fprintf(stderr,"Sorry, %s cant open %s\n",*argv,CNTL);
		exit(2);
		}

		
	if (ioctl(fd,AUDIO_GETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant get status for %s\n",*argv,CNTL);
		exit(3);
		}

	for(i=1;i<argc;i++) {
		switch(lookup(argv[i])) {
			case 0:		/* status */
				printf("record gain %d/%d\n",info.record.gain,AUDIO_MAX_GAIN);
				printf("playback gain %d/%d\n",info.play.gain,AUDIO_MAX_GAIN);
				printf("monitor gain %d/%d\n",info.monitor_gain,AUDIO_MAX_GAIN);
				printf("play port: %s\n",info.play.port==AUDIO_SPEAKER ?
                   "internal":"external");
				continue;		/* no set */
				break;
			case 1:		/* volume */
				info.play.gain = atoi(argv[++i])&255;
				break;
			case 2:		/* record volume */
				info.record.gain = atoi(argv[++i])&255;
				break;
			case 3:		/* monitor volume */
				info.monitor_gain = atoi(argv[++i])&255;
				break;
			case 4:		/* internal speaker */
				info.play.port = AUDIO_SPEAKER;
				break;
			case 5:		/* external speaker */
				info.play.port = AUDIO_HEADPHONE;
				break;
			default:
				fprintf(stderr,"Invalid command: %s\n",argv[i]);
				continue;
			}
		}
	if (ioctl(fd,AUDIO_SETINFO,&info) <0) {
		fprintf(stderr,"Sorry, %s cant set status for %s\n",argv[0],CNTL);
		exit(3);
		}
	}

int
lookup(s)
char *s;
	{
	register int i;

	for(i=0;cmd[i].name;i++) {
		if (strcmp(s,cmd[i].name)==0)
			return(i);
		}
	return(-1);
	}
