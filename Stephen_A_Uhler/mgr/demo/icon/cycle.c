/*                        Copyright (c) 1988 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/demo/icon/cycle.c,v 1.1 92/09/11 08:25:02 sau Exp Locker: sau $
	$Source: /home/sau/mgr/demo/icon/cycle.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/demo/icon/cycle.c,v $$Revision: 1.1 $";

#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include "term.h"

/*
 * cycle -- a program to do simple flip-book animation
 * Steve Hawley
 */

/* sleep for 100th of seconds */

#define fsleep(x) \
   { \
   struct timeval time; \
   time.tv_sec = (x)/100; \
   time.tv_usec = ((x)%100)*10000; \
   select(0,0,0,0,&time); \
   }

#define DEF_SPEED 10
#define MAXBUF 512

static char	*cmd;
static int	offset;	/* where icons end (offset from argc) */
static int	bitcount;	/* number of bitmaps created */
static char	cwd[MAXBUF];
static int	w, h;

static
cleanup()
{
	/* be a nice program and clean up */
	int i;

	m_ttyreset();			/* reset echo */
	for (i = 1; i <= bitcount; i++)	/* free up bitmaps */
		m_bitdestroy(i);
	m_pop();			/* pop environment */
	exit(0);
}

main(argc,argv)
char **argv;
{
	char	*getenv();
	int	speed, i;
	int	reverse = 0;
	int	stop = 0;
	FILE	*popen(), *fp = popen("/bin/pwd", "r");

	cmd = *argv;
	argc--; argv++;

	if (!fp) {
		fprintf(stderr,"%s: can't get current directory\n",cmd);
		exit(2);
	}
	fgets(cwd,sizeof(cwd),fp);
	*(cwd + strlen(cwd) - 1) = '\0';  /* blah */
	pclose(fp);

	if (argc < 1)
		usage();

	ckmgrterm();

	m_setup(M_FLUSH);	/* flush output stream */
	m_push(P_BITMAP|P_EVENT|P_FLAGS);
	m_setmode(M_ABS);

	signal(SIGINT,cleanup);		/* program loops forever */
	signal(SIGTERM,cleanup);	/* this gives a mechanism */
	signal(SIGQUIT,cleanup);	/* for cleaning up */

	m_func(B_COPY);	/* bit copy, so we don't have to erase */
	m_clear();	/* clear the screen */
	m_ttyset()	;/* no keybaord echo */

	speed = DEF_SPEED;
	offset = 0;

	while( argv[0][0] == '-' ) {
		switch( argv[0][1] ) {
		case 's':
			speed = atoi(&(argv[0][2]));
			break;
		case 'p':
			stop = atoi(&(argv[0][2]));
			if (stop <=0) stop = 100; 		/* default to 1 second */
			break;
		case 'r':
			reverse = 1;
			break;
		default:
			usage();
		}
		argv++; argc--;
	}

	if (argc < 1)
		usage();

	for ( ; argc;  argv++, argc-- ) {
		bitcount++;
		loadmap( bitcount, *argv );
	}
	while(1) {
		for (i = 1; i <= bitcount; i++) {
			m_bitcopyto(0, 0, w, h, 0, 0, 0, i);
			fsleep(speed);
			/* delay a bit, so we can see animation */
		}
		if( reverse )
			for (i--;  i > 1;  i--) {
				m_bitcopyto(0, 0, w, h, 0, 0, 0, i);
				fsleep(speed);
			}
		if ( stop )
			fsleep(stop);
	}
}


static
loadmap( i, file )
int	i;
char	*file;
{
	char	buf[MAXBUF];

	if (*file == '/')
		m_bitfromfile(i, file);
	else if (strncmp(file, "../", 3) == 0) {
		sprintf(buf, "%s/%s", cwd, file);
		m_bitfromfile(i, buf);
	}
	else if (strncmp(file, "./", 2) == 0) {
		sprintf(buf, "%s%s", cwd, file+1);
		m_bitfromfile(i, buf);
	}
	else {
		m_bitfromfile(i, file);
	}
	m_gets(buf);
	sscanf(buf, "%d %d", &w, &h); /* load in icons. */
	if (w == 0 || h == 0) {
		fprintf(stderr, "%s: %s is not a bitmap.\n", cmd, file);
		cleanup();
	}
}


static
usage()
{
	fprintf(stderr, "Usage: %s [-sspeed] [-r] [-p] icon1 [icon2 ...iconn]\n",
		cmd);
	fputs("\
-sspeed	delay `speed' seconds/100 between frames\n\
-r	after running forward through the frames, reverse and run backwards\n\
-pspeed	after running forward through the frames, pause, then repeat\n",
		stderr);
	exit(1);
}
