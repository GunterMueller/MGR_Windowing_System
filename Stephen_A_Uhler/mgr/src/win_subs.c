/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/win_subs.c,v 1.1 91/02/11 09:48:21 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/win_subs.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/win_subs.c,v $$Revision: 1.1 $";

/* Teminal emulator functions called from put_window() */

#include "bitmap.h"
#include "font.h"
#include "defs.h"
#include "window.h"
#include "clip.h"
#include <stdio.h>

#define FSIZE(c)	((int) (W(font)->head.c))
#define Abs(x)		((x)>0?(x):-(x))

/*****************************************************************************
 *	Do raster ops
 */

int
win_rop(win,window)
register WINDOW *win;
BITMAP *window;
    {
    register int *p = W(esc);
    register int op;

    if (W(flags)&W_REVERSE)
       op = ROP_INVERT(W(op));
    else
       op = W(op);

#ifdef DEBUG
    dprintf(B)(stderr,"%s: blit\t",W(tty));
#endif
    switch (W(esc_cnt)) {
       case 0:		/* set raster op function */
               W(op) = W(op)&~NOCOLOR | NOCOLOR&GET_OP(*p);
               if (W(flags)&W_OVER) {
                  W(style) = GET_OP(*p);
                  if ((W(flags)&W_REVERSE) ^ (W(flags)&W_STANDOUT))
                     W(style) = ROP_INVERT(*p);
                  }
#ifdef DEBUG
               dprintf(B)(stderr,"setting function %d\n",p[0]);
#endif
               break;
       case 1:    /* set raster op color && function */
               W(op) = (NOCOLOR&GET_OP(p[0])) | (GETCOLOR(p[1]));
#ifdef DEBUG
               dprintf(B)(stderr,"setting mode %d, color %d\n",p[0],p[1]);
#endif
               break;

       case 3:		/* ras_write */
               bit_blit(window,Scalex(p[0]),Scaley(p[1]),
                               Scalex(p[2]),Scaley(p[3]),
                               op,NULL_DATA,0,0);
               if (Do_clip())
                  Set_clip(Scalex(p[0]),Scaley(p[1]),
                           Scalex(p[0])+Scalex(p[2]),
                           Scaley(p[1])+Scaley(p[3]));
               break;
       case 4:		/* ras_write  specify dest */
               if (p[4]>MAXBITMAPS)
                  break;
               if (p[4]>0 && W(bitmaps)[p[4]-1]== (BITMAP *) 0)
                  W(bitmaps)[p[4]-1] = bit_alloc(
                              Scalex(p[0])+Scalex(p[2]),
                              Scaley(p[1])+Scaley(p[3]),NULL_DATA,DEPTH);
               bit_blit(p[4]?W(bitmaps)[p[4]-1]:window,
                               Scalex(p[0]),Scaley(p[1]),
                               Scalex(p[2]),Scaley(p[3]),
                               op,0,0,0);
               if (Do_clip() && p[4]==0)
                  Set_clip(Scalex(p[0]),Scaley(p[1]),
                           Scalex(p[0])+Scalex(p[2]),
                           Scaley(p[1])+Scaley(p[3]));
               break;
       case 5:		/* ras_copy */
               bit_blit(window,Scalex(p[0]),Scaley(p[1]),
                               Scalex(p[2]),Scaley(p[3]),
                               op,window,
                               Scalex(p[4]),Scaley(p[5]));
               if (Do_clip())
                  Set_clip(Scalex(p[0]),Scaley(p[1]),
                           Scalex(p[0])+Scalex(p[2]),
                           Scaley(p[1])+Scaley(p[3]));
               break;
       case 7:		/* ras_copy specify dst,src */
               if (p[6]>MAXBITMAPS || p[7]>MAXBITMAPS)
                  break;
               if (p[6]>0 && W(bitmaps)[p[6]-1]== (BITMAP *) 0) {
                  W(bitmaps)[p[6]-1] = bit_alloc(
                              Scalex(p[0])+Scalex(p[2]),
                              Scaley(p[1])+Scaley(p[3]),NULL_DATA,DEPTH);
                  }
#ifdef DEBUG
               dprintf(B)(stderr,"blitting %d to %d\n",p[7],p[6]);
#endif
               bit_blit(p[6]?W(bitmaps)[p[6]-1]:window,
                           Scalex(p[0]),Scaley(p[1]),
                           Scalex(p[2]),Scaley(p[3]),op,
                           p[7]?W(bitmaps)[p[7]-1]:window,
                           Scalex(p[4]),Scaley(p[5]));
               if (Do_clip() && p[6]==0)
                  Set_clip(Scalex(p[0]),Scaley(p[1]),
                           Scalex(p[0])+Scalex(p[2]),
                           Scaley(p[1])+Scaley(p[3]));
       }
   }

/*****************************************************************************
 *	down load a bit map  - parse escape sequence
 */

int
win_map(win,window)
register WINDOW *win;
register BITMAP  *window;
   {
   register int cnt = W(esc_cnt);
   register int *p = W(esc);
   register int op;

#ifdef MOVIE
   SET_DIRTY(W(bitmap));
#endif
   if (W(flags)&W_REVERSE)
      op = ROP_INVERT(W(op));
   else
      op = W(op);

   switch(cnt) {
      case 2:		/* bitmap to graphics point */
         bit_blit(window,Scalex(W(gx)),Scaley(W(gy)),
                         p[0],p[1],op,W(bitmap),0,0);
         bit_destroy(W(bitmap));
         if (Do_clip()) {
            Set_clip(Scalex(W(gx)),Scaley(W(gy)),
                     Scalex(W(gx))+p[0],Scaley(W(gy))+p[1]);
            }
         break;
      case 3:		/* bitmap to graphics point  specify dest*/
         if (p[2] > MAXBITMAPS)
            break;
         if (p[2]>0 && W(bitmaps)[p[2]-1]== (BITMAP *) 0)
            W(bitmaps)[p[2]-1]=W(bitmap);
         else {
            bit_blit(p[2]?W(bitmaps)[p[2]-1]:window,
                         Scalex(W(gx)),Scaley(W(gy)),
                         p[0],p[1],op,W(bitmap),0,0);
            bit_destroy(W(bitmap));
            }
         break;
      case 4:		/* bitmap to specified point */
         bit_blit(window,p[2],p[3],p[0],p[1],op,W(bitmap),0,0);
         bit_destroy(W(bitmap));
         if (Do_clip()) {
            Set_clip(p[2],p[3],p[2]+p[0],p[3]+p[1]);
            }
         break;
      case 5:		/* bitmap to specified point  specify dest */
         if (p[4] > MAXBITMAPS)
            break;
         if (p[4]>0 && W(bitmaps)[p[4]-1]== (BITMAP *) 0)
            W(bitmaps)[p[4]-1]=W(bitmap);
         else {
            bit_blit(p[4]?W(bitmaps)[p[4]-1]:window,
                        p[2],p[3],p[0],p[1],op,W(bitmap),0,0);
            bit_destroy(W(bitmap));
            }
         break;
      }
   W(snarf) = (char *) 0;
	W(bitmap) = NULL;			/* so destroy() doesn't destroy us 2ce sau 5/91 */
   }

/*****************************************************************************
 *	plot a line 
 */

int
win_plot(win,window)
register WINDOW *win;
BITMAP *window;
    {
    register int *p = W(esc);
    int op;

    if (W(flags)&W_REVERSE)
       op = ROP_INVERT(W(op));
    else
       op = W(op);

    switch (W(esc_cnt)) {
       case 0:			/* set cursor to graphics point */
               W(x) = Scalex(W(gx));
               W(y) = Scaley(W(gy));
               break;
       case 1:			/* draw to graphics point */
               Bit_line(win,window,Scalex(W(gx)),Scaley(W(gy)),
                               Scalex(p[0]),Scaley(p[1]),op);
               W(gx) = p[0];
               W(gy) = p[1];
               break;
       case 3:
               Bit_line(win,window,Scalex(p[0]),Scaley(p[1]),
                               Scalex(p[2]),Scaley(p[3]),op);
					W(gx) = p[2];
					W(gy) = p[3];

               break;
       case 4:
               if (p[4]<MAXBITMAPS && W(bitmaps)[p[4]-1])
                  bit_line(W(bitmaps)[p[4]-1],Scalex(p[0]),Scaley(p[1]),
                               Scalex(p[2]),Scaley(p[3]),op);
               break;
       }
    }

Bit_line(win,dst,x1,y1,x2,y2,op)
register WINDOW *win;
register BITMAP *dst;
register int x1,y1,x2,y2,op;
   {
#ifdef DEBUG
   dprintf(l)(stderr,"%s: line [%d] %d,%d %d,%d\n",W(tty),op,x1,y1,x2,y2);
#endif
   bit_line(dst,x1,y1,x2,y2,op);
   if (Do_clip()) {
      Set_clip(x1,y1,x2+1,y2+1); 
      Set_clip(x2,y2,x1+1,y1+1); 
      }
   }


/* experimantal graphics crunch mode */

int
grunch(win,dst)
WINDOW *win;	/* window */
BITMAP *dst;	/* destination */
	{
   register char *buf = W(snarf);
   register int cnt = W(esc)[W(esc_cnt)];
   int op;
	int penup = 0;
   int *p = W(esc);
	register int x,y,x1,y1;

   if (W(flags)&W_REVERSE)
      op = ROP_INVERT(W(op));
   else
      op = W(op);

	/* set starting point */

	if (W(esc_cnt) > 1) {
      x =  p[0];
      y =  p[1];
      }
	else {
      x = W(gx);
      y = W(gy);
		}
	while (cnt-- > 0) {
		x1 = (*buf>>4 & 0xf) - 8;
		y1 = (*buf & 0xf) - 8;
		if (x1==0 && y1 ==0)
         penup = 1;
      else if (penup == 0){
         bit_line(dst,Scalex(x),Scaley(y),Scalex(x+x1),Scaley(y+y1),op);
#ifdef DEBUG
         dprintf(y)(stderr,"%s: line [%d] %d,%d + %d,%d\n",W(tty),op,x,y,x1,y1);
#endif
			x += x1;
         y += y1;
			}
		else {
			x += x1;
         y += y1;
			penup = 0;
			}
		buf++;
		}
	W(gx) = x;
   W(gy) = y;
	}

/*****************************************************************************
 *	plot a circle 
 */

int
circle_plot(win,window)
register WINDOW *win;
BITMAP *window;
    {
    register int *p = W(esc);
    int op;

    if (W(flags)&W_REVERSE)
       op = ROP_INVERT(W(op));
    else
       op = W(op);


    switch (W(esc_cnt)) {
       case 0:		/* draw a 'circle'  at graphics point*/
               circle(window,Scalex(W(gx)),Scaley(W(gy)),Scalexy(p[0]),op);
               break;
       case 1:		/* draw an 'ellipse' at graphics point */
               ellipse(window, Scalex(W(gx)), Scaley(W(gy)),
                               Scalex(p[0]), Scaley(p[1]), op);
               break;
       case 2:		/* draw a 'circle' */
               circle(window,Scalex(p[0]),Scaley(p[1]),Scalexy(p[2]),op);
               break;
       case 3:		/* draw an 'ellipse' */
               ellipse(window, Scalex(p[0]), Scaley(p[1]),
                               Scalex(p[2]), Scaley(p[3]), op);
               break;
       case 4:		/* draw an 'ellipse' to offscreen bitmap */
               if (p[4]<MAXBITMAPS && W(bitmaps)[p[4]-1])
                  ellipse(W(bitmaps)[p[4]-1], Scalex(p[0]), Scaley(p[1]),
                               Scalex(p[2]), Scaley(p[3]), op);
               break;
       case 5:		/* draw an arc  ccw centered at p0,p1 */
               arc(window, Scalex(p[0]), Scaley(p[1]), Scalex(p[2]), Scaley(p[3]),
                               Scalex(p[4]), Scaley(p[5]), op);
               break;
       case 6:		/* draw an arc  ccw centered at p0,p1  to offscreen bitmap */
               if (p[6]<MAXBITMAPS && W(bitmaps)[p[6]-1])
                  arc(W(bitmaps)[p[6]-1], Scalex(p[0]), Scaley(p[1]), Scalex(p[2]),
                                Scaley(p[3]), Scalex(p[4]), Scaley(p[5]), op);
               break;
       }
    if (Do_clip())
       Set_all();
    }

/*****************************************************************************
 *	move the graphics pointer
 */

int
win_go(win)
register WINDOW *win;
    {
    register int *p = W(esc);

    switch (W(esc_cnt)) {
       case 0:			/* set the graphics point to cursor pos */
               W(gx) = W(x);
               W(gy) = W(y);
               break;
       case 1: case 2:		/* set the graphics point */
               W(gx) =  p[0];
               W(gy) =  p[1];
               break;
       }
    }

/* determine bitmap style from size */

int
bmap_size(w,h,count)
int w,h;          /* size of bitmap */
int count;        /* # of data bytes */
   {
   int format;    /* bitmap format */
	int bytes = count/h;

   if (count >= w*h && DEPTH>1)       /* 8 bit color */
      format=DEPTH;
   else if (bytes*8 == ((w+31)&~31))  /* 1 bit, 32 bit aligned */
      format=31;
   else if (bytes*8 == ((w+15)&~15)) /* 1 bit 16 bit aligned */
      format=15;
   else                             /* unknown format */
      format=0;
   return(format);
   }
