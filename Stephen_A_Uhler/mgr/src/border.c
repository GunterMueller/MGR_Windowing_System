/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/border.c,v 1.1 91/02/11 09:46:13 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/border.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/border.c,v $$Revision: 1.1 $";
/* draw a border around a window  */

#include "bitmap.h"
#include "defs.h"

border(win,b,w)
WINDOW  *win;			/* window */
int b, w;			/* black width, white width */
   {
	register int clr,set;
	register BITMAP *bdr;

#ifdef COLOR
	clr = BIT_SRC | (W(background)&~NOCOLOR);
	set = BIT_SRC ^ BIT_DST | (W(background)^W(style))&~NOCOLOR;
#else
   clr = BIT_CLR;
   set = BIT_SET;
#endif

	if (W(flags)&W_ACTIVE)
		bdr = W(border);
	else
		bdr = W(save);

   bit_blit(bdr,0,0,BIT_WIDE(bdr),(b+w),clr,NULL_DATA,0,0);
   bit_blit(bdr,BIT_WIDE(bdr)-(b+w),0,(b+w),BIT_HIGH(bdr),clr,NULL_DATA,0,0);
   bit_blit(bdr,0,BIT_HIGH(bdr)-(b+w),BIT_WIDE(bdr),(b+w),clr,NULL_DATA,0,0);
   bit_blit(bdr,0,0,(b+w),BIT_HIGH(bdr),clr,NULL_DATA,0,0);

   bit_blit(bdr,b,0,BIT_WIDE(bdr)-2*b,b,set,NULL_DATA,0,0);
   bit_blit(bdr,BIT_WIDE(bdr)-b,0,b,BIT_HIGH(bdr),set,NULL_DATA,0,0);
   bit_blit(bdr,b,BIT_HIGH(bdr)-b,BIT_WIDE(bdr)-2*b,b,set,NULL_DATA,0,0);
   bit_blit(bdr,0,0,b,BIT_HIGH(bdr),set,NULL_DATA,0,0);
   }
