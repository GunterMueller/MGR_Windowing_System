/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/cut.c,v 1.2 91/02/11 09:46:17 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/cut.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/cut.c,v $$Revision: 1.2 $";

/* cut and paste text */

#include <stdio.h> 
#include "bitmap.h"
#include "defs.h"
#include "font.h"
#include "window.h"
#include "event.h"

/* stuff global buffer into input stream */

int 
paste()
   {
   if (snarf) {
      do_event(EVENT_PASTE,active,E_MAIN);
      Write(ACTIVE(to_fd),snarf,strlen(snarf));
#ifdef DEBUG
      dprintf(y)(stderr,"%s: Pasting [%s]\n",ACTIVE(tty), snarf?snarf:"EMPTY");
#endif
      }
#ifdef DEBUG
   else {
      dprintf(y)(stderr,"%s: Nothing to paste\n",ACTIVE(tty));
      }
#endif
   }

/* cut text from a window, put in global buffer */


#define MAXROWS		64		/* greatest char height */
#define MAXCOLS		32		/* widest char (bits in u-long) */

BITMAP *glyph;				/* spot for glyph comparison */
unsigned long data[MAXROWS];		/* bit data for glyph */
BITMAP *check;				/* other spot for glyph comparison */
unsigned long data2[MAXROWS];		/* bit data for other glyph */

static struct entry **table;		/* hash table */

/* sweep out and cut text */

int
cut()
   {
   register int i,j;
   register WINDOW *win = active;		/* window to cut text from */
   int count;					/* # of snarfed chars */
   int errors = 0;				/* number of misses */
   int cols=1, rows=0;				/* rows and cols swept */
   int col,row;					/* starting col and row */
   int maxcol;					/* # of cols in row */
   int x,y;					/* bit position in bitmap */
   int hcode;					/* hash code */
   int button;					/* button from move_mouse */
   char c;					/* matched char */
   char *pntr;					/* current char in line */
   struct entry *entry;				/* current hash table entry */
   char *line;					/* buffer to receive text */
   char get_match(),*malloc();
   char *to_tabs(), *fixline();

   /* return immediately if window is not snarffable */

   if ((W(flags) & W_SNARFABLE) ==0)
      return(0);

   /* initialize comparison registers */

   glyph = bit_alloc(32,FSIZE(high),data,1);
   check = bit_alloc(32,FSIZE(high),data2,1);

   bit_blit(check,0,0,32,FSIZE(high),BIT_CLR,NULL_DATA,0,0);

   /* build hash table */

   if ((table = W(font)->table) == NULL) {
#ifdef DEBUG
      dprintf(C)(stderr,"building cut table\n");
#endif
      table = W(font)->table =
                    (struct entry **) malloc (sizeof (struct entry) * H_SIZE);
      bzero(table, sizeof(struct entry) * H_SIZE);
   
      for(i = FSIZE(start);i<FSIZE(start)+FSIZE(count);i++) {
         if (W(font)->glyph[i] && i >= ' ') {
            hcode = get_hash(W(font)->glyph[i],0,0,FSIZE(wide),FSIZE(high),0);
            enter(hcode,i);
            }
         }
      }

   /* find cut region */

   SETMOUSEICON(&mouse_cut);
   button = move_mouse(screen,mouse,&mousex,&mousey,0);
   SETMOUSEICON(&mouse_arrow);
   i = get_text(screen,mouse,mousex,mousey,&cols,&rows,win,E_SWTEXTT);
   if (i == 0) {
      do_button(0);
      return(0);
   }

   /* find extent of cut region */

   col = (mousex-(W(x0)+SUM_BDR+W(text.x)))/FSIZE(wide);
   maxcol = (W(text.wide) ? W(text.wide) : BIT_WIDE(W(window)))/FSIZE(wide);
   row = (mousey-(W(y0)+SUM_BDR+W(text.y)))/FSIZE(high);

   if (W(flags)&W_SNARFLINES) {		/* snarf lines only */
#ifdef DEBUG
      dprintf(C)(stderr,"Cutting lines only\n");
#endif
      col = 0;
      cols = maxcol;
      }

#ifdef DEBUG
   dprintf(C)(stderr,"Cut got %d,%d  %d x %d\n",col,row,cols,rows);
#endif

   /* look up characters */

   pntr = line = malloc(1+(1+maxcol)*(rows+1));	/* max possible cut */
   switch(rows) {
      case 0:			/* 1 row */
         y = W(text.y)+row*FSIZE(high);
         for(x=W(text.x)+col*FSIZE(wide),i=0;i<cols;i++,x+=FSIZE(wide)) {
            c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
            *pntr++ = c ? c : (errors++,C_NOCHAR);
            }
         if (col+cols == maxcol && c==' ')
            pntr = fixline(line,pntr);
         break;
      case 1:			/* 2 rows */
         y = W(text.y)+row*FSIZE(high);
         for(x=W(text.x)+col*FSIZE(wide),i=0;i<maxcol;i++,x+=FSIZE(wide)) {
            c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
            *pntr++ = c ? c : (errors++,C_NOCHAR);
            }
         pntr = fixline(line,pntr);

         y += FSIZE(high);
         for(x=W(text.x),i=0;i<col+cols;i++,x+=FSIZE(wide)) {
            c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
            *pntr++ = c ? c : (errors++,C_NOCHAR);
            }
         if (col+cols == maxcol && c==' ')
            pntr = fixline(line,pntr);
         break;

      default:			/* > 2 rows */
         y = W(text.y)+row*FSIZE(high);
         for(x=W(text.x)+col*FSIZE(wide),i=0;i<maxcol;i++,x+=FSIZE(wide)) {
            c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
            *pntr++ = c ? c : (errors++,C_NOCHAR);
            }
         pntr = fixline(line,pntr);

         for(j=0;j<rows-1;j++) {
            y += FSIZE(high);
            for(x=W(text.x),i=0;i<maxcol;i++,x+=FSIZE(wide)) {
               c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
               *pntr++ = c ? c : (errors++,C_NOCHAR);
               }
            pntr = fixline(line,pntr);
            }

         y += FSIZE(high);
         for(x=W(text.x),i=0;i<col+cols;i++,x+=FSIZE(wide)) {
            c = get_match(W(window),x,y,FSIZE(wide),FSIZE(high));
            *pntr++ = c ? c : (errors++,C_NOCHAR);
            }
         if (col+cols == maxcol && c==' ')
            pntr = fixline(line,pntr);

         break;
      }
   *pntr = '\0';

	/* dont use bit_free */
   free(check);
   free(glyph);

   /* put text into snarf buffer */

   count = pntr-line;
#ifdef DEBUG
   dprintf(C)(stderr,"snarfed %d chars, %d errors\n",count,errors);
   dprintf(C)(stderr,"snarfed [%s]\n",line);
#endif

   if (!(W(flags)&W_SNARFHARD)  && errors > 0 || 2*errors > count) {
      oops();
      count = 0;
      }
   else {
      if (W(flags)&W_SNARFTABS)
         to_tabs(col,line,line);

      if (snarf && button < BUTTON_SYS) {			/* add to cut buffer */
         char *tmp = malloc(strlen(snarf) + strlen(line) +1);
         count += strlen(snarf);
         strcpy(tmp,snarf);
         strcat(tmp,line);
         free(snarf);
         free(line);
         snarf = tmp;
         }
      else if (snarf) {					/* replace cut buffer */
         free(snarf);
         snarf = line;
         }
      else						/* new cut buffer */
         snarf = line;

      /* send snarf events (if any) */
      id_message = W(pid);
      for(win=active;win != (WINDOW *) 0;win=W(next))
         do_event(EVENT_SNARFED,win,E_MAIN);
      }
   do_button(0);
   return(count);
   }

/* given bitmap, get hash code */

static int
get_hash(map,x,y,w,h,how)
BITMAP *map;
int x,y,w,h;		/* where in map */  
int how;		/* 0-> normal, 1->inverted */
   {
   register unsigned long sum = 0;
   register int i,j;
   bit_blit(glyph,0,0,32,h,BIT_CLR,NULL_DATA,0,0);
   bit_blit(glyph,32-w,0,w,h,how ? BIT_NOT(BIT_SRC) : BIT_SRC,map,x,y);
         for (j=0;j<h;j++)
            sum += data[j]*(j+1);
   return(sum%H_SIZE);
   }

/* enter a glyph into the hash table */

static int
enter(item,value)
int item;
unsigned char value;
   {
   register struct entry *entry;
   char *malloc();

   if (table[item] == (struct entry *) 0) {
      table[item] = (struct entry *) malloc(sizeof (struct entry));
      table[item] -> value = value;
      table[item] -> next = (struct entry *) 0;
      }
   else {
      for(entry = table[item];entry->next;entry = entry -> next);
      entry -> next  = (struct entry *) malloc(sizeof (struct entry));
      entry -> next -> value = value;
      entry -> next -> next = (struct entry *) 0;
      }
   }

/* find a character match in current font */

static char
get_match(map,x,y,w,h)
BITMAP *map;				/* bitmap containing text */
int x,y,w,h;				/* position of glyph in "map" */
   {
   register struct entry *entry;
   register WINDOW *win = active;
   int code;				/* hash code */
   int size = sizeof(long) * h;

   code = get_hash(map,x,y,w,h,0);		/* leaves char in glyph */
   for(entry=table[code];entry;entry=entry->next) {
      bit_blit(check,32-w,0,w,h,BIT_SRC,W(font)->glyph[entry->value],0,0);
      if (bcmp(data,data2,size)==0) {
         return(entry->value);
         }
      }

   /* try inverse video version */

   code = get_hash(map,x,y,w,h,1);		/* leaves char in glyph */
   for(entry=table[code];entry;entry=entry->next) {
      bit_blit(check,32-w,0,w,h,BIT_SRC,W(font)->glyph[entry->value],0,0);
      if (bcmp(data,data2,size)==0) {
         return(entry->value);
         }
      }
   return('\0');
   }
   
/* zap a font hash table */

zap_fhash(fnt)
struct font *fnt;
   {
   register struct entry *entry, *next;
   register int i;

   if (fnt->table) {
      for(i=0;i<H_SIZE;i++)
         for(entry=table[i];entry;entry=next) {
            next = entry->next;
            free(entry);
            }
      free(fnt->table);
      }
   }

/* change trailing white space into \n */

char *
fixline(s,pnt)
char *s;
register char *pnt;
   {
   while (*--pnt == ' ' && pnt > s);
   *++pnt = '\n';
   return(++pnt);
   }

/* change spaces to tabs */

static char *
to_tabs(pos,in,out)
register int pos;				/* starting col # */
register char *in;				/* input str */
register char *out;				/* output str - tabs */
   {
   char *s = out;				/* start of out str */
   register char c;				/* current input char */
   register int spaces = 0;			/* # pending spaces */
   
#ifdef DEBUG
   dprintf(C)(stderr,"-> TABS");
#endif
   while(pos++,c = *in++) {

      if (c=='\n' || c=='\r' || c=='\f')	/* reset column counter */
         pos =0;

      if (c == ' ') {
         spaces++;
         if (pos && pos%8 == 0) {		/* ' ' -> \t */
            c = '\t';
#ifdef DEBUG
            dprintf(C)(stderr,".");
#endif
            spaces=0;
            }
         }
      else for(;spaces>0;spaces--) {		/* output spaces */
         *out++ = ' ';
         }
         
      if (spaces == 0)
         *out++ = c;
      }
   *out = '\0';
#ifdef DEBUG
   dprintf(C)(stderr,"\n");
#endif
   return(s);
   }

/* can't cut, ring bell */

static int
oops()
   {
   register WINDOW *win = active;

   CLEAR(W(window),BIT_NOT(BIT_DST));
   write(2,"\007",1);
   CLEAR(W(window),BIT_NOT(BIT_DST));
   }
