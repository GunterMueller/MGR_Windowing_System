/* extract ranges from a sound file (no error checking) */

#define SIZE			4096		/* hunk size */
#define Min(x,y)		((x)<(y)?(x):(y))

#include <stdio.h>

double atof();
char buff[SIZE];

main(argc,argv)
int argc;
char **argv;
	{
	char *name;
	float start;
	float end;
	int a;
	int b;
	int fd;
	register int count;
	register int got;

	if (argc<4) {
		fprintf(stderr,"usage: %s <file> <start_seconds> <end_seconds>\n",*argv);
		exit(1);
		}

	name = argv[1];
	start = atof(argv[2]);
	end = atof(argv[3]);

	if (start>end) {
		fprintf(stderr,"Invalid times\n");
		exit(1);
		}
	a = start*8000;
	b = end*8000;

	fd = open(name,0);
	if (fd<0) {
		fprintf(stderr,"cant open file\n");
		exit(1);
		}

	count = b-a;
	fprintf(stderr,"%d bytes\n",count);

	lseek(fd,a,0);
	while(count>0 && (got = read(fd,buff,Min(count,SIZE))) > 0) {
		write(1,buff,got);
		count -= got;
		}
	exit(0);
   }
