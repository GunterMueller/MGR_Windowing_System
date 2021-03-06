/*                        Copyright (c) 1988 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/copyright.c,v 1.1 91/02/11 09:46:15 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/copyright.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/copyright.c,v $$Revision: 1.1 $";

/* flash a copyright notice at Mgr startup */

#include <sys/time.h> 
#include <sys/signal.h>
#include "bitmap.h"

/* the copyright bitmaps and data */

extern char cr_bits[];
extern char lb_0_bits[], lb_1_bits[], lb_2_bits[], lb_3_bits[];
extern char lb_4_bits[], lb_5_bits[], lb_6_bits[], lb_7_bits[];

bit_static(cr,472,83,cr_bits,1);
bit_static(lb_0,181,181,lb_0_bits,1);
bit_static(lb_1,181,181,lb_1_bits,1);
bit_static(lb_2,181,181,lb_2_bits,1);
bit_static(lb_3,181,181,lb_3_bits,1);
bit_static(lb_4,181,181,lb_4_bits,1);
bit_static(lb_5,181,181,lb_5_bits,1);
bit_static(lb_6,181,181,lb_6_bits,1);
bit_static(lb_7,181,181,lb_7_bits,1);

#define SSIZE	3		/* star size */

#define BG_COLOR		4	/* background color */
#define LOGO_COLOR	2	
#define CR_COLOR		5	/* copyright color */

static BITMAP *logo[] =
	{ &lb_0, &lb_1, &lb_2, &lb_3, &lb_4, &lb_5, &lb_6, &lb_7};

static struct timeval delay = {
   (long) 0, (long) 120000
   };

/* for "star trek" clip areas */

struct rect {
	short x1,y1;
	short x2,y2;
	}c1,c2;

copyright(where)
BITMAP *where;	/* where to blast the copyright */
	{
	BITMAP *notice = &cr;
	int x = (BIT_WIDE(where)-BIT_WIDE(notice))/2;
	int y = BIT_HIGH(where)/2;
	int high = BIT_HIGH(logo[0]);
	int wide = BIT_WIDE(logo[0]);
	int mask = 1;		/* select mask (1 == kbd) */
	int pid;				/* pid of "startrek" program */
	register int i;
	char c;

	c1.x1 = x - SSIZE;
	c1.y1 = y - SSIZE;
	c1.x2 = BIT_WIDE(notice) + x;
	c1.y2 = BIT_HIGH(notice) + y;

	bit_blit(where,0,0,BIT_WIDE(where),
            BIT_HIGH(where),BIT_SRC|GETCOLOR(BG_COLOR),0l,0,0);
	bit_blit(where,x,y,BIT_WIDE(notice),BIT_HIGH(notice),
            BIT_SRC^BIT_DST|GETCOLOR(BG_COLOR)^GETCOLOR(CR_COLOR),
            notice,0,0);

	x = (BIT_WIDE(where)-wide)/2;
	y = y - high - BIT_HIGH(notice);

	c2.x1 = x - SSIZE;
	c2.y1 = y - SSIZE;
	c2.x2 = wide + x;
	c2.y2 = high + y;

	/* kick off stars */

	if ((pid=fork()) == 0) /* child */
#ifndef NOFLY
		fly(where,c1,c2);
#else
		sleep(10000);
#endif
   else {
		for(i=0;select(32,&mask,0,0,&delay) <= 0;i++,mask=1)
			bit_blit(where,x,y,wide,high,
				BIT_SRC|GETCOLOR(BG_COLOR), logo[i%8],0,0);
		read(0,&c,1);
		kill(pid,SIGTERM);
		while(wait(0)!=pid);
		}
	}

/* star trek effect */


/*
 * porter.c  Steve Hawley 4/3/87
 * rehacked 5/18/1988 for extra speed.
 * re-re hacked 6/20/88 for MGR (SAU)
 * A demo to get stars inspired by Star Trek without
 * using quickdraw, and by addressing the screen directly.
 * this version is roughly 8 times faster than its quickdraw
 * equivalent.
 * In considering the bit drawing routines, remember that
 * on the macintosh, a bit turned on is black, not white.
 */

#define MAXZ 500 /* maximum z depth */
#define NSTARS 256 /* maximum number of stars */
#define SPEED	8		/* star speed */
#define SCALE	(short)6	/* for rotator */
#define COUNT	(short)3	/* for rotator */
#define ON 1  /* plotting states */
#define OFF 0
#define Random() rand()

short maxv, maxh; /* display size */
short hmaxv, hmaxh;	/* 1/2 display size */

struct st {
        short x, y, z;
	short color;
        } stars[NSTARS]; /* our galaxy */

fly (where,clip1,clip2)
BITMAP *where;
struct rect clip1, clip2;		/* "holes" in galaxy */
{
        register short i;
        register struct st *stp;

        init_all(where);     /* set up global variables */
        for (i=0, stp = stars; i< NSTARS; i++, stp++) {
                /* initialize galaxy */
                do {
                        stp->x = Random();
                        stp->y = Random();
                        stp->z = (Random() % MAXZ) + 1;
                        stp->color = Random() % 23;
								if (stp->color == BG_COLOR)
                        	stp->color++;
			
                } while(project(where,stp->x, stp->y, stp->z, stp->color, ON,clip1,clip2)); /* on screen? */
        }
        while (1) { /* loop 'til button */
                i = NSTARS;
                stp = stars;
                do {
                        project(where,stp->x, stp->y, stp->z, stp->color, OFF,clip1,clip2); /* turn star off*/
                        if ((stp->z -= SPEED) <= 0) { /* star went past us */
                                stp->x = Random();
                                stp->y = Random();
                                stp->z = MAXZ;
                        }
								else {		/* rotate universe */
									cordic(&stp->x,&stp->y,SCALE,COUNT);
								}
                        if (project(where,stp->x, stp->y, stp->z, stp->color, ON,clip1,clip2)) {
                                /* if projection is off screen, get a new position */
                                stp->x = Random();
                                stp->y = Random();
                                stp->z = MAXZ;
                        }
                ++stp;
                } while(--i);
        }
}

project(where,x, y, z, col, state,clip1,clip2)
register BITMAP *where;
register short x, y, z;
register int col;
register short state;
struct rect clip1, clip2;
{
        
        /* one-point perspective projection */
        /* the offsets (maxh/2) and maxv/2) ensure that the
         * projection is screen centered
         */
		x = (x/z) + hmaxh;
		y = (y/z) + hmaxv;
        return(xplot(where,x, y, col, state,clip1,clip2));

}

init_all(where)
register BITMAP *where;
{
   maxv = BIT_HIGH(where);
   hmaxv = maxv>>1;
   maxh = BIT_WIDE(where);
   hmaxh = maxh>>1;
}       

xplot(where,x, y, col, state,clip1,clip2)
register BITMAP *where;
register int x, y;
register int col;
int state;
struct rect clip1, clip2;
{
        register short *line, b;
        
        /* are we on the screen? If not, let the caller know*/
        if (x < 0 || x >= maxh || y < 0 || y >= maxv )
            return(1);
        if (!(x < clip1.x1 || x >= clip1.x2 || y < clip1.y1 || y >= clip1.y2 ))
            return(0);
        if (!(x < clip2.x1 || x >= clip2.x2 || y < clip2.y1 || y >= clip2.y2 ))
            return(0);

        bit_blit(where,x,y,SSIZE,SSIZE, state ?
		BIT_SRC^BIT_DST | GETCOLOR(col)^GETCOLOR(BG_COLOR) :
		BIT_SRC | GETCOLOR(BG_COLOR),
                 0l,0,0);
        return(0);
}

/* CORDIC rotator. Takes as args a point (x,y) and spins it */
/* count steps counter-clockwise.                   1       */
/*                                Rotates atan( --------- ) */
/*                                                  scale   */
/*                                                 2        */
/* Therefore a scale of 5 is 1.79 degrees/step and          */
/* a scale of 4 is 3.57 degrees/step                        */

cordic(x, y, scale, count)
short *x, *y;
register short scale, count;

{
   register short tempx, tempy;

   tempx = *x;
   tempy = *y;

   if (count > 0) /* positive count (counter-clockwise) */
      for (; count; count--){
         tempx -= (tempy >> scale);
         tempy += (tempx >> scale); 
      }
   else          /* negative count (clockwise) */
      for (; count; count++){
         tempx += (tempy >> scale);
         tempy += (tempx >> scale);
      }

   *x = tempx;
   *y = tempy;
}
