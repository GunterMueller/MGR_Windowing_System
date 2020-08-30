/* both record and save video  - NO SOUND */

#include <sys/time.h>
#include <sys/signal.h>
#include <stdio.h>

#define NAME		"/usr/tmp/record"		/* default file name template */
#define HUNK		1024
#define COMPRESS	"compress"				/* name of compression program */

char line[128];
char buff[HUNK];
FILE *v_fd;			/* video output */

int clean();

main(argc,argv)
int argc;
char **argv;
	{	
	char *name;
	int pid = getpid();			/* unique identifier */
	int count=0;					/* read count */
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

	/* catch signals */

	signal(SIGHUP,clean);
	signal(SIGTERM,clean);
	signal(SIGINT,clean);
	
	/* start processing */

	while ((count = read(0,buff,HUNK)) > 0) 
		fwrite(buff,1,count,v_fd);

	clean(0);
	}

/* clean up and exit */

int clean(n)
int n;
	{
	pclose(v_fd);
	exit(n);
	}
