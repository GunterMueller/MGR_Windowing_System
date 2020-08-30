/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: clock2.c,v 4.2 88/06/22 14:37:27 bianchi Exp $
	$Source: /tmp/mgrsrc/demo/misc/RCS/clock2.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/misc/RCS/clock2.c,v $$Revision: 4.2 $";

/* get today's date  (analog clock version) */

#include <errno.h>
#include <time.h>
#include <stdio.h>
#include "term.h"
#include "restart.h"

#define OFFSET		(500<<10)		/* shift points to 1st quad */

/* sin, cos tables: 0-360 deg. in 6 deg. increments (scaled to +/- 1024) */

int sin[] = {
	 0, 107, 212, 316, 416, 512,
	 601, 685, 760, 828, 886, 935,
	 973, 1001, 1018, 1023, 1018, 1001,
	 973, 935, 886, 828, 760, 685,
	 601, 511, 416, 316, 212, 107,
	 0, -107, -212, -316, -416, -512,
	 -601, -685, -760, -828, -886, -935,
	 -973, -1001, -1018, -1023, -1018, -1001,
	 -973, -935, -886, -828, -760, -685,
	 -601, -511, -416, -316, -212, -107,
	 0 };


int cos[] = {
	 1023, 1018, 1001, 973, 935, 886,
	 828, 760, 685, 601, 511, 416,
	 316, 212, 107, 0, -107, -212,
	 -316, -416, -512, -601, -685, -760,
	 -828, -886, -935, -973, -1001, -1018,
	 -1023, -1018, -1001, -973, -935, -886,
	 -828, -760, -685, -601, -511, -416,
	 -316, -212, -107, 0, 107, 212,
	 316, 416, 512, 601, 685, 760,
	 828, 886, 935, 973, 1001, 1018,
	 1023 };

typedef struct coord {
	int x,y;
	} coord;

/* coordinates of the hands at 12:00 */

coord second[] = { 1,-30,  0,485 };
coord minute[] = { 10,-10,  0,400,  -10,-10,  10,-10 };
coord hour[] = { 35,-10,  0,270,  -35,-10,  35,-10 };

coord big_tic[] = { -11,485,  0,450,  11,485 };
coord tic[] = { 0,485,  0,460 };

int h, old_h, m, old_m, s, old_s;

main(argc,argv)
int argc;
char **argv;
   {
   register int i;
   int cal=0;
   int dotime(), clean();
   char line[80];

   ckmgrterm( *argv );

   if (argc>1 && strcmp(argv[1],"-c")==0)
      cal++;

   /* setup mgr environment */

   m_setup(0);
   m_push(P_FLAGS|P_EVENT|P_CURSOR);
   m_ttyset();

   m_setevent(REDRAW,"R\r");
   m_setevent(RESHAPE,"R\r");
	m_setcursor(CS_INVIS);
   if (cal)
      m_setevent(ACTIVATE,"A\r");

   signal(SIGALRM,dotime);
   signal(SIGTERM,clean);
   signal(SIGINT,clean);
   signal(SIGHUP,clean);

   while(1) {
      m_func(B_SET);
      m_clear();
      m_ellipse(500,500,490,490);
      for(i=0;i<60;i+=5) 		/* the tic marks */
         if (i%15==0)
            draw(big_tic,2,i);
         else
            draw(tic,1,i);
   
      m_movecursor(500,500);
      get_time(&h, &m, &s);

      draw(hour,3,h);
      draw(minute,3,m);

      m_func(B_INVERT);
      draw(second,1,s);
      m_flush();
      dotime();

      /* wait for an event */

      while(1) {
	 extern int   errno;

         errno = 0;
	 *line = '\0';
         if (m_gets(line) == NULL  &&  errno  &&  errno != EINTR)
            clean(0);
         alarm(0);
         if (*line=='R')
            break;
         else if (cal && *line == 'A') {
            long time(), tmp = time(0);
            char *ctime();

            m_push(P_ALL);
            m_font(11);
            m_size(27,3);
            m_clear();
            m_printstr("\n "); 
            m_printstr(ctime(&tmp));
            m_flush();
            sleep(3);
            m_pop();
            m_clearmode(M_ACTIVATE);
            m_flush();
            }
         dotime();
         }
      }
   }

/* update the time */

int
dotime()
   {
   old_h=h, old_m=m, old_s=s;
   get_time(&h, &m, &s);

   draw(second,1,old_s);
   if (old_m != m) {
      m_func(B_CLEAR);
      draw(minute,3,old_m);
      draw(hour,3,old_h);
 
      m_func(B_SET);
      draw(hour,3,h);
      draw(minute,3,m);
      m_func(B_INVERT);
      }

   draw(second,1,s);
   m_flush();
   signal(SIGALRM,dotime);
   alarm(1);
   }

/* rotate and draw hands */
   
#define X(i)	(what[i].x)
#define Y(i)	(what[i].y)

draw(what, n, theta)
coord what[];			/* array of points */
int n;				/* number of points */
int theta;			/* angle (in 160th of a circle) */
   {
   register int i;

   for(i=0;i<n;i++) {
      m_line((X(i)*cos[theta]+Y(i)*sin[theta] + OFFSET)>>10,
           (X(i)*sin[theta]-Y(i)*cos[theta] + OFFSET)>>10,
           (X(i+1)*cos[theta]+Y(i+1)*sin[theta] + OFFSET)>>10,
           (X(i+1)*sin[theta]-Y(i+1)*cos[theta] + OFFSET)>>10);
           }
   }

/* convert time to angles */

get_time(h,m,s)
int *h, *m, *s;		/* hours, minutes, seconds */
   {
   struct tm *tme, *localtime();
   long tmp,time();
    
   tmp = time(0);
   tme = localtime(&tmp);

   *m = tme->tm_min;
   *s = tme->tm_sec;
   if (tme->tm_hour > 11)
      tme->tm_hour -= 12;
   *h = tme->tm_hour*5 + (2+tme->tm_min)/12;
   }

/* clean up and exit */

clean(n)
int	n;
   {
   m_popall();
   m_clear();
   m_flush();
   m_ttyreset();
   exit(1);
   }
