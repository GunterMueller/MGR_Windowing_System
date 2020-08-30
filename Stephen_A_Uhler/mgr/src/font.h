/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/font.h,v 1.1 91/02/11 09:46:04 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/font.h,v $
*/
static char	h_font_[] = "$Source: /home/sau/mgr/src/RCS/font.h,v $$Revision: 1.1 $";

/* Constant spaced font format */

#define FONT_A			'\030'	/* fixed width fonts 32 bit alignment*/
#define FONT_X			'\026'	/* fixed width fonts 16 bit alignment (obsolete) */
#define FONT_S			'\027'	/* proportional fonts (not used yet) */

struct font_header {
   unsigned char type;		/* font type */
   unsigned char wide;		/* character width */
   unsigned char high;		/* char height */
   unsigned char baseline;	/* pixels from bottom */
   unsigned char count;		/* number of chars in font */
   char          start;		/* starting char in font */
   };

#define H_SIZE	499		/* size of hash table for cut/paste */

struct entry {
   unsigned char value;		/* character at this location */
   struct entry *next;		/* pntr to next char */
   };

#define	MAXGLYPHS	256

struct font {
   struct font_header head;	/* font header */
   BITMAP *data;		/* all the glyphs strung together */
   BITMAP *glyph[MAXGLYPHS];	/* pointers to individual glyphs */
   short ident;			/* font id */
   struct entry **table;	/* pointer to hash table for cut/paste */
   };

#define HEADER_SIZE	sizeof(struct font_header)

struct font *open_font();
