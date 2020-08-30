/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: bounce.c,v 4.4 88/06/30 15:16:09 sau Exp $
	$Source: /tmp/mgrsrc/demo/misc/RCS/bounce.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/misc/RCS/bounce.c,v $$Revision: 4.4 $";

#include <sys/time.h>
#include <stdio.h>
#include "term.h"
#include "restart.h"

#define MAXX	999
#define MAXY	999
#define MAXV	60
#define MINV	20
#define LCT	10
#define SLOW	60000		/* usec to sleep between lines */

#define fsleep(x) \
   { \
   struct timeval time; \
   time.tv_sec = 0; \
   time.tv_usec = x; \
   select(0,0,0,0,&time); \
   }

int vx1, vy1, vx2, vy2;
int x1, y1, x2, y2;
int thex1[LCT];
int they1[LCT];
int thex2[LCT];
int they2[LCT];
int ptr;
int lcolor,bcolor;
long random();

main(argc,argv)
char **argv;
{
        register int s = 0;
	int sleep = 0;

	ckmgrterm( *argv );

	m_setup(0);
	m_push(P_EVENT|P_FLAGS|P_CURSOR);
	m_setcursor(CS_INVIS);
	srand(getpid());
	vx1 = 50;
	vy1 = 50;
	x1 = 500;
	y1 = 1;
	vx2 = -50;
	vy2 = -50;
	x2 = 500;
	y2 = MAXY;

	if (argc>1 && strcmp(argv[1],"-s")==0)
		sleep++;

        Restart();
	m_setevent(UNCOVERED,_quit);
	m_clearmode(M_BACKGROUND);
	for (ptr=0;ptr<LCT;ptr++)
	{
		thex1[ptr] = they1[ptr] = thex2[ptr] = they2[ptr] = -1;
	}

	
	bcolor = rand()%24;
	while((lcolor=rand()%24) == bcolor);
	m_bcolor(bcolor);
	m_fcolor(lcolor);
	m_linecolor(B_SRC^B_DST,lcolor);
	m_clear();
	for(;;)
	{
		ptr = (ptr+1) % LCT;
		if (thex1[ptr] >= 0)
			m_line(thex1[ptr],they1[ptr],thex2[ptr],they2[ptr]);

		mvpoint(&x1,&y1,&vx1,&vy1);
		mvpoint(&x2,&y2,&vx2,&vy2);
		thex1[ptr] = x1;
		they1[ptr] = y1;
		thex2[ptr] = x2;
		they2[ptr] = y2;
			
		if (thex1[ptr] >= 0)
			m_line(thex1[ptr],they1[ptr],thex2[ptr],they2[ptr]);
		m_flush();
		if (sleep)
		   fsleep(90000);
	}
}

mvpoint(tx,ty,v_x,v_y)
int *tx,*ty,*v_x,*v_y;
{

		*tx += *v_x;	/* move the point */
		*ty += *v_y;

		if ( *tx >= MAXX) 	/* bounce */
		{
			*v_x = (*v_x > 0) ? -(*v_x) : *v_x;
			diddle(v_x);
		}
		if ( *ty >= MAXY)
		{
			*v_y = (*v_y > 0) ? -(*v_y) : *v_y;
			diddle(v_y);
		}

		if ( *tx <= 0)
		{
			*v_x = (*v_x < 0) ? -(*v_x) : *v_x;
			diddle(v_x);
		}
		if ( *ty <= 0)
		{
			*v_y = (*v_y < 0) ? -(*v_y) : *v_y;
			diddle(v_y);
		}
}

diddle(ptr)
int *ptr;
{
	int tmp;
	/*
	**	pick a number between MAXV and MINV
	*/
	tmp = (rand()% (MAXV-MINV)) + MINV;
	/*
	**	 and get the sign right
	*/
	if (*ptr < 0)
		*ptr = -tmp;
	else
		*ptr = tmp;
}
