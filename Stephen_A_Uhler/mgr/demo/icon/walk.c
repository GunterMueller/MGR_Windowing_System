/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/demo/icon/walk.c,v 1.1 91/03/11 13:41:37 sau Exp Locker: sau $
	$Source: /home/sau/mgr/demo/icon/walk.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/demo/icon/walk.c,v $$Revision: 1.1 $";

#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include "term.h"

#define ICONPATH	"eye"
#define EYES    24
#define CYC	5
#define EYERT   1
#define EYEDN	7
#define EYELF   13
#define EYEUP	19
#define fsleep(x) \
   { \
   struct timeval time; \
   time.tv_sec = 0; \
   time.tv_usec = (x) * 1000; \
   select(0,0,0,0,&time); \
   }
static char *_quit = "\034";

int x, y, w, h, i, j, k;
int hsize, vsize;

cleanup()
{
	m_pop();
	m_clear();
	exit(0);
}

clearit()
{
	get_size(&x, &y, &hsize, &vsize);
	m_clear();
}
main(argc,argv)
char **argv;
{
        register int s = 0;
	int speed = 1;
	int delay = 90;
	char buf[101];

	ckmgrterm( *argv );

	m_setup(M_FLUSH);
	m_push(P_BITMAP|P_EVENT|P_FLAGS|P_CURSOR);
	m_setcursor(CS_INVIS);
	m_setmode(M_ABS);
	m_linecolor(3,4);

	if (argc>1 && strcmp(argv[1],"-d")==0)
		delay = atoi(argv[1]+1);

	if (argc>1 && strcmp(argv[1],"-s")==0)
		speed=0;

	signal(SIGINT,cleanup);
	signal(SIGTERM,cleanup);
	signal(SIGQUIT,clearit);

	m_setevent(RESHAPE,_quit);
	m_setevent(REDRAW,_quit);
	
	m_func(B_SRC); /* bit copy, so we don't have to erase */
	m_clear(); /* clear the screen */
	m_ttyset();/* no echo */

	for (i = 1; i <EYES+1; i++) {
		sprintf(buf, "%s/eye%d", ICONPATH,i);
		if( !m_bitfile(i, buf, &w, &h) ) {
			fprintf( stderr, "cannot download %s.  quit\n", buf );
			exit( 1 );
		}
	}
	m_ttyreset();/* reset echo */
	get_size(&x, &y, &hsize, &vsize);
	while(1)
	{
		m_flush();
		j = EYERT;
		for (i = 2; i < hsize - w; i += speed) {
			m_linecolor(j,27);
			m_bitcopyto(i, 2, w, h, 0, 0, 0, j);
			++j; /* cycle bitmap number */
			if (j > EYERT+CYC) j = EYERT;
			fsleep(delay); /* delay a bit, so we can see animation */
		 }
		j = EYEDN;
		for (i = 2; i < vsize - w - 4; i += speed) {
			m_bitcopyto(hsize - w, i, w, h, 0, 0, 0, j);
			++j;
			if (j > EYEDN+CYC) j = EYEDN;
			fsleep(delay);
		}
		j = EYELF;
		for (i = hsize - w; i > 2; i -= speed) {
			m_bitcopyto(i, vsize - w - 4, w, h, 0, 0, 0, j);
			++j; /* cycle the other way */
			if (j > EYELF+CYC) j = EYELF;
			fsleep(delay);
		}
		j = EYEUP;
		for (i = vsize - w - 4; i > 2; i -= speed) {
			m_bitcopyto(2, i, w, h, 0, 0, 0, j);
			++j;
			if (j > EYEUP+CYC) j = EYEUP;
			fsleep(delay);
		}
	}
}

