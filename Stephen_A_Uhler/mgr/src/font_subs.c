/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/font_subs.c,v 1.2 91/02/11 09:47:02 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/font_subs.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/font_subs.c,v $$Revision: 1.2 $";

/* font routines */

#include "bitmap.h"
#include "font.h"
#include "default_font.h"
#include "window.h"
#include <stdio.h>
#ifdef DEBUG
#   include "defs.h"
#endif

#define HEAD(x)		font->head.x

/**************************************************************************
 *
 *	set up a font file
 */

struct font *
open_font(file)
char *file;			/* name of font file */
   {
   FILE *fp;
   int size;
   char *malloc();
   struct font *font, *open_sfont();

   if (file == (char *) 0 || *file == '\0') {
      return(open_sfont(default_font_head,&default_font));
      }

#ifdef DEBUG
      dprintf(f)(stderr,"Opening font file [%s]\n",file);
#endif

   if ((fp=fopen(file,"r"))  == NULL)
      return((struct font *)0);

   if ((font=(struct font *) malloc(sizeof(struct font))) == (struct font *)0) {
      fclose(fp);
      return((struct font *)0);
      }

   if (fread(&(font->head),HEADER_SIZE,1,fp) != 1) {
      free((char *) font);
      fclose(fp);
      return((struct font *)0);
      }

   if (HEAD(type) != FONT_A) {
      free((char *) font);
      fclose(fp);
      return((struct font *)0);
      }
                               
	/* fonts are always 32 bit aligned */

	size = (HEAD(wide)*HEAD(count)+31)&~31;

   font->data = bit_alloc(size,HEAD(high),NULL_DATA,1);
   font->table = (struct entry **) 0;

   /* read in font data */

   size = BIT_SIZE(font->data);
   fread(BIT_DATA(font->data), size, 1, fp);
#ifdef MOVIE
	SET_DIRTY(font->data);
#endif

   /* create individual characters */

   glyph_create( font );

   fclose(fp);
   return(font);
   }


/**************************************************************************
 *
 *	deallocate a font
 */

int
free_font(dead_font)
struct font *dead_font;		/* font to be deallocated */
   {
   register int i;

   if (!dead_font)
      return(-1);

   for(i=0;i<MAXGLYPHS;i++)
      if (dead_font->glyph[i])
         bit_destroy(dead_font->glyph[i]);
   if (dead_font->head.type != FONT_S)
      bit_destroy(dead_font->data);
   zap_fhash(dead_font);		/* free up hash table space */
   i=font_purge(dead_font);	/* eliminate references to dead font */

#ifdef DEBUG
      dprintf(f)(stderr,"freeing font %d (%d references)\n",
      dead_font->ident,i);
#endif

   free((char *) dead_font);
   }

/**************************************************************************
 *
 *	set up a static font file
 */

struct font *
open_sfont(head,data)
struct font_header head;	/* font header */
BITMAP *data;		/* array of bits */
   {
   char *malloc();
   struct font *font;

   if ((font=(struct font *) malloc(sizeof(struct font))) == (struct font *)0)
      return((struct font *)0);

   font->head = head;
   font->data = data;
   font->head.type = FONT_S;
   font->table = (struct entry **) 0;

   /* create individual characters */

   glyph_create( font );

   return(font);
   }


static
glyph_create( font )
struct font *font;
   {
   register int i, x;
   int		first = HEAD(start);
   int		last = HEAD(start) + HEAD(count);
   int		wide = HEAD(wide);
   int		high = HEAD(high);
   int		nochar;
   
   /* Pick the character to be printed for characters not in the set.
      Normally, it is the character specified by C_NOCHAR, but it that isn't
      in the range of the set, we pick the first character (which is usually
      a space).
   */
   nochar = C_NOCHAR - HEAD(start);
   if( nochar >= last )
	nochar = 0;
   nochar *= wide;

   x = 0;
   for(i=0; i<MAXGLYPHS; i++)
      if (i < first || i >= last)
         font->glyph[i] = bit_create(font->data, nochar, 0, wide, high);
      else {
         font->glyph[i] = bit_create(font->data, x, 0, wide, high);
         x += wide;
         }
   }
