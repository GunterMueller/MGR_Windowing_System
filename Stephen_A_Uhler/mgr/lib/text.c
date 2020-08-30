/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: text.c,v 4.1 88/06/21 13:41:05 bianchi Exp $
	$Source: /tmp/mgrsrc/lib/RCS/text.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/lib/RCS/text.c,v $$Revision: 4.1 $";

#include "term.h"

#define TRUE	1
#define FALSE	0

int
text(s,x,y,font,angle,size_x, size_y)
register char *s; 	/* string to be printed */
register int x, y;	/* starting coordinates */
int font; 		/* font #, from 0-4 --- 0: line font, 1: seriff type, */
          		/* 2: Greek, 3: cursive, 4: old English             */
int angle; 		/* string rotation in degrees */
int size_x; 		/* character size  1000 ~= full window sized character */
int size_y; 		/* character size  1000 ~= full window sized character */
{
	register char ch;	/* the character being printed */
	register int i; 	/* the "workin' man" of variables */
	register int xc,yc;	/* current character coordinates */
	register int penup;	/* a flag */
	short xmax, xmin;	/* maximum character extent */
	short pts[250];	 	/* vector points */
	int npts; 		/* number of vector points */
	int isin(), icos();	/* integer sin, cosine functions */

	/* compute scaled x-form data */

 	int sinx = size_x *isin(angle);
	int cosx = size_x *icos(angle);
 	int siny = size_y *isin(angle);
	int cosy = size_y *icos(angle);

	if (font > 4 || font < 0)
		font = 0;

	while(ch = *s++){ 
		if(font == 0)
			sfont(0,ch,&xmin,&xmax,&npts,pts);
		else
			scribe(font-1,ch,&xmin,&xmax,&npts,pts);

		penup = TRUE;  		/* pen starts up on each letter */
				
		for(i=0; i < npts; i += 2)
			if(pts[i] == 31)
				penup = TRUE;
			else{ 
				xc = (pts[i]*cosx - pts[i+1]*siny)>>14;
				yc = (pts[i]*sinx + pts[i+1]*cosy)>>14;

				if(penup)	/* go to the new point */
					m_go(x + xc, y - yc);
				else		/* draw to the next point */
					m_draw(x + xc,y - yc);
				penup = FALSE;
			}
		x += ((xmax-xmin)*cosx)>>14;
		y -= ((xmax-xmin)*sinx)>>14;
	}
}
