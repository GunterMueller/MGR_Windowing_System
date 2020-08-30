/* audio control program for vcr demo (self contained 4.1 version) */

#include <stdio.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <sun/audioio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#define INTERVAL			100000		/* microseconds */
#define DVOL				10				/* incr to change volume */
#define SUFFIX				"au"			/* suffix for sound files */
#define BLOCK(x)			(x==0 ? NULL : &poll)
#define dprintf			if (debug) fprintf
#define MIN_Q		1500		/* minimum bytes in the audio Q */

struct stat st_buff;			/* to figure out how big the file is */
char line[128];			/* control input buffer */
int name[128];				/* name of sound file */
int debug = 0;				/* debugging flag */
int speed=0;				/* set audio speed to match video stuff */
int fd;						/* fd for sound file */
int size_fd;				/* size of audio file */
int audio;					/* fd for audio */
int volume;					/* volume 0-255 */
int min_q = MIN_Q;		/* audio Q minimum */
struct timeval poll = {0,INTERVAL};	/* timeout for select */
struct audio_info info;				/* audio ioctl structure */

main(argc,argv)
int argc;
char **argv;
	{
	int mask;				/* select mask */
	int done=0;				/* true if done */
	char *input;			/* input line */
	int code;				/* select return value */

	setbuf(stdin,NULL);			/* stdio and select don't mix */
	debug = getenv("DEBUG");	/* set debugging flag */

	if ((audio = open("/dev/audio",O_WRONLY | O_NDELAY)) < 0) {
		fprintf(stderr,"Cant open audio\n");
		exit(4);
		}
	fcntl(audio,F_SETFL,fcntl(audio,F_GETFL,0)|FNDELAY);	/* non blocking */
	
	volume = get_volume(audio);

	if (argc>1 && strcmp(argv[1],"-j")==0){		/* select jack */
			play_external(audio);
			argc--, argv++;
			}

	if (argc>1 && strcmp(argv[1],"-t")==0){		/* set audio threshold */
			min_q = atoi(argv[1][2]);
			argc--, argv++;
			}

	while(!done) {
		mask = 1<<(fileno(stdin));
		code = select(32,&mask,0,0,BLOCK(speed));
		switch(code) {
			case 1:				/* got control input */
				input = gets(line);
				dprintf(stderr,"TALK Got [%s]\n",input ? input : "EOF");
				done = (process(input) == 0);
				break;
			case 0:				/* timeout, process audio */
				do_sound();
				break;
			default:				/* select error */
				break;
			}
		}
	exit(0);
	}

/* process control input */

int
process(line)
char *line;
	{
	int old_speed = speed;		/* previous speed */
	int byte;						/* set byte offset */

	if (!line)
		return(0);

	switch(*line) {
		case 'J':			/* turn on jack */
			play_external(audio);
			break;
		case 'j':			/* turn off jack */
			play_internal(audio);
			break;
		case '-':			/* volume adjust */
			break;
		case '+':			/* volume adjust */
			break;
		case '>':			/* volume volume */
			volume = volume<255-DVOL? volume + DVOL : 255;
			dprintf(stderr,"TALK volume up to %d\n",volume);
			play_volume(audio,volume);
			break;
		case '<':			/* volume down */
			volume = volume>DVOL? volume - DVOL : 0;
			dprintf(stderr,"TALK volume down to %d\n",volume);
			play_volume(audio,volume);
			break;
		case 'Q':			/* quit */
			dprintf(stderr,"TALK Quitting\n");
			return(0);
			break;
		case 's':			/* set speed */
			speed = atoi(line+2);
			dprintf(stderr,"TALK setting speed to %d\n",speed);
			if (!speed && old_speed) {		/* turn off */
				dprintf(stderr,"TALK   Stopping audio\n");
				audio_stop(audio);
				}
			break;
		case 'S':			/* set file */
			sprintf(name,"%s.%s",line+2,SUFFIX);
			if (fd>0)
				close(fd);
			fd = open(name,0);
			fstat(fd,&st_buff);
			size_fd = st_buff.st_size;
			fcntl(fd,F_SETFL,fcntl(fd,F_GETFL,0)|FNDELAY);	/* non blocking */
			dprintf(stderr,"TALK Opening file [%s] as %d (%d bytes)\n",
						name,fd,size_fd);
			break;
		case 'B':			/* set byte offset */
			if (fd>0) {
				byte = atoi(line+2)%size_fd;
				dprintf(stderr,"Talk Syncing to byte %d\n",byte);
				lseek(fd,byte,0);
				audio_stop(audio);
				}
			break;
		default:
			dprintf(stderr,"Talk Unknown command [%c]\n",*line);
			break;
		}
	do_sound();
	return(1);	
	}

/* play some sound */

#define HUNK		300		/* hunk to process at a time */

unsigned char buff[2*HUNK];	/* sound buffer */

int
do_sound()
	{
	int bytes;					/* bytes read */
	int q;

	if (fd>0) {
		while(speed>0 && write_q(audio)<min_q) { 
			bytes = get_sound(fd,buff,HUNK,speed);
			if (bytes<=0) {
				dprintf(stderr,"Talk End of file, rewinding\n");
				/* speed=0; */
				lseek(fd,0,0);
				}
			else
				a_write(audio,buff,bytes);
			}
		}
	}

/* read in sound, adjust for speed (preliminary version) */

int
get_sound(fd,buff,size,s)
int fd;			/* file to get sound data from */
char *buff;		/* where to read sound */
int size;		/* # of bytes to get */
int s;		/* speed %of normal */
	{
	int got;		/* got this many */

	switch(s) {
		case 100:		/* normal speed */
			got = read(fd,buff,size);
			break;
		case 200:		/* fast forward */
			got = read(fd,buff,size*2)/2;
			break;
		case 50:			/* 1/ speed */
			got = read(fd,buff,size/2);
			if (got>0) bcopy(buff,buff+got,got);
			got *= 2;
			break;
		default:
			dprintf(stderr,"Talk cant play at speed %d (using 100)\n",s);
			speed = 100;
		}
	return(got);
	}

/*-------------------------------------- audio stuff ---------------------- */

static unsigned int wrote = 0;		/* total bytes written */

/* get the current volume */

int
get_volume(fd)
int fd;
	{
	ioctl(fd,AUDIO_GETINFO,&info);
	return(info.play.gain);
	}

/* put the current volume */

int
play_volume(fd,volume)
int fd;
	{
	AUDIO_INITINFO(&info);
	info.play.gain = volume;
	ioctl(fd,AUDIO_SETINFO,&info);
	}

/* set the external jack */

int
play_external(fd)
int fd;
	{
	AUDIO_INITINFO(&info);
	info.play.port = AUDIO_HEADPHONE;
	ioctl(fd,AUDIO_SETINFO,&info);
	}

/* set the internal speaker */

int
play_internal(fd)
int fd;
	{
	AUDIO_INITINFO(&info);
	info.play.port = AUDIO_SPEAKER;
	ioctl(fd,AUDIO_SETINFO,&info);
	}

/* flush the audio output (thus stopping it) */

audio_stop(fd)
int fd;
	{
	int arg = FLUSHW;
	wrote = 0;
	ioctl(fd,I_FLUSH,arg);
	AUDIO_INITINFO(&info);
	info.play.samples = 0;
	ioctl(fd,AUDIO_SETINFO,&info);
	return(0);
	}

/* write audio */

int
a_write(audio,buff,bytes)
int audio;		/* audio fd */
char *buff;		/* what to write */
int bytes;	
	{
	int count;

	count = write(audio,buff,bytes);
	if (count > 0)
		wrote += count;
	return(count);
	}

/* how many bytes are in the audio Q */

int
write_q(fd)
int fd;
	{
	ioctl(fd,AUDIO_GETINFO,&info);
	return(wrote - info.play.samples);
	}
