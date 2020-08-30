/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/get_menus.c,v 1.1 91/02/11 09:47:18 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/get_menus.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/get_menus.c,v $$Revision: 1.1 $";

/******************************************************************************
 *
 *	low level popup menu management routines
 */

#include <stdio.h>
#include "bitmap.h"
#include "menu.h"
#include "font.h"

#define MAX_LIST	100	/* max number of choices */
#define BLIP		8	/* size of cursor blip */

#define BETWEEN(a,x,b)	(x)<(a)?a:((x)>(b)?b:x)

#define Pr_ropall(S,f)	bit_blit(S,0,0,BIT_WIDE(S),BIT_HIGH(S),f,NULL_DATA,0,0)

#define BAR(s,z)	bit_blit(s, 0, (z+1)*state->bar_sizey, \
                               state->bar_sizex, state->bar_sizey, \
                               BIT_NOT(BIT_DST), NULL_DATA, 0, 0);

#ifdef NOTRACK
#define TRACKON(s,x,y)
#define TRACKOFF(s,x,y)
#else
#define TRACKON(s,x,y) { \
	bit_blit(&track,0,0,BLIP,BLIP, \
	BIT_SRC,s,(x)-BLIP/2,(y)-BLIP/2); \
	bit_blit(s,(x)-BLIP/2,(y)-BLIP/2,BLIP,BLIP, \
	BIT_NOT(BIT_SRC) & BIT_DST,&mouse_bull,0,0); \
	bit_blit(s,(x)-BLIP/2,(y)-BLIP/2,BLIP,BLIP, \
	BIT_SRC | BIT_DST,&mouse_bull2,0,0); \
	}
#define TRACKOFF(s,x,y) \
	bit_blit(s,(x)-BLIP/2,(y)-BLIP/2,BLIP,BLIP, \
	BIT_SRC,&track,0,0)
#endif

#ifndef Min
#define Min(x,y)		((x)>(y)?(y):(x))
#endif
#ifndef Max
#define Max(x,y)		((x)>(y)?(x):(y))
#endif
#define LIMIT(x,max)		(x)>(max)?1:(-(x)>(max)?-1:0)
#define Abs(x)			((x)>0?(x):-(x))
#define MENU			struct menu_state

/*	The height of each selection area (i.e. word) in the pop-up menu.
	The 2 extra pixels create a 1-pixel boarder above and below each word.
*/
#define HIGH		(font->head.high+2)

extern BITMAP mouse_bull, mouse_bull2;
static unsigned short save_bits[BLIP];
static bit_static(track,BLIP,BLIP,save_bits,DEPTH);

/* allocate space for and initialize menu */

struct menu_state *
menu_define(font,list,values,max,fg,bg)
struct font *font;		/* which font to use for menu */
char *list[];			/* list of menu items */
char *values[];			/* list of return values */
int max;			/* max number of menu items */
int fg,bg;		/* clt values for menu color */
   {
   register int i, incr, count;	/* counters */
   int size_x=0, size_y=0;
   struct menu_state *state;	/* menu state */
   char *malloc(), *save_line();
   BITMAP *menu,			/* menu image */
	 *box,			/* menu pix_rect */
	 *inside,		/* box - border */
	 *save;			/* part of screen covered by menu */
   int box_x, box_y;		/* dimensions of menu box */

   /* find size of box */

   for(count=0;list[count]!=(char *) 0 && count<(max>0?max:MAX_LIST); count++) {
      size_x = Max(size_x,strlen(list[count]));
      }

   /*	The 2 extra pixels are to allow a 1-pixel border to the left and right
	of each word.
   */
   size_x = size_x * font->head.wide + 2;
   size_y = count * HIGH;
   box_x =size_x+2*MENU_BORDER;
   box_y =size_y+2*MENU_BORDER;

   /* build box */

   menu = bit_alloc(box_x,box_y,NULL_DATA,DEPTH);
   inside = bit_create(menu,MENU_BORDER,MENU_BORDER,size_x,size_y);

   /* paint text into box */

#ifdef COLOR
   Pr_ropall(menu,BIT_SRC|GETCOLOR(fg));
   Pr_ropall(inside,BIT_SRC|GETCOLOR(bg));
#else
   Pr_ropall(menu,BIT_SET);
   Pr_ropall(inside,BIT_CLR);
#endif
   for(i=0,incr=HIGH-1;i<count;i++,incr+=HIGH) {
#ifdef COLOR
      /* do local color here */
      put_str(inside,1,incr,font,
              BIT_SRC^BIT_DST | GETCOLOR(bg)^GETCOLOR(fg),list[i]);
#else
      put_str(inside,1,incr,font,BIT_SRC,list[i]);
#endif
      }

   /* save the menu state */

   if ((state = (MENU *) malloc(sizeof(struct menu_state))) == (MENU *) 0) {
      bit_destroy(inside);
      bit_destroy(menu);
      return(state);
      }

   /* get the values */

   if (values != (char **) 0) {
      state -> action = (struct menu_action *) 
               malloc(count * sizeof(struct menu_action));
      if (state->action) for(i=0;i<count;i++) {
         state->action[i].value = save_line(values[i]);
         state->action[i].next_menu = -1;
         }
      
      }
   else
      state->action = (struct menu_action *) 0;

   state -> menu = menu;
   state -> bar_sizex = size_x;
   state -> bar_sizey = HIGH;
   state -> count = count;
   state -> current = 0;
   state -> next = -1;
   state->flags = 0;
   state -> screen = (BITMAP *) 0;
   state -> save = (BITMAP *) 0;

   bit_destroy(inside);
   return(state);
   }

/* put the menu on the display */

struct menu_state *
menu_setup(state,screen,x,y,start)
struct menu_state *state;	/* existing menu state */
BITMAP *screen;			/* where to put the menu */
int x,y;			/* current offset of mouse on screen */
int start;			/* preselected item */
   {
   register int i, incr, count;	/* counters */
   char *malloc();

   /* position the box on the screen */

   if (state->BIT_WIDE(menu)>BIT_WIDE(screen) ||
       state->BIT_WIDE(menu)>BIT_WIDE(screen) ||
       state->save)
       return((MENU *) 0);

   x = Min(x,BIT_WIDE(screen) - state->BIT_WIDE(menu));
   y = Min(y,BIT_HIGH(screen) - 
                 state->BIT_HIGH(menu) - state->bar_sizey);
   y = Max(y,state->bar_sizey+BLIP);

   state->save = bit_alloc(state->BIT_WIDE(menu),state->BIT_HIGH(menu),
                  NULL_DATA,DEPTH);
   bit_blit(state->save,0,0,state->BIT_WIDE(menu),
          state->BIT_HIGH(menu),BIT_SRC,screen,x,y);

   /* initialize the menu */

   state -> screen = screen;
   state -> current = start;
   state -> menu_startx = x;
   state -> menu_starty = y;
   state -> x_pos = state-> bar_sizex/2;

   bit_blit(screen,x,y,state->BIT_WIDE(menu),state->BIT_HIGH(menu),BIT_SRC,
          state->menu,0,0);

   if (start>=0 && start<state->count) {
       BITMAP *inside = bit_create(screen,state->menu_startx+MENU_BORDER,
                    state->menu_starty+MENU_BORDER,
                    state->bar_sizex,state->bar_sizey*state->count);
       BAR(inside,start-1);
       bit_destroy(inside);
       }
   
   return(state);
   }


/******************************************************************************

allow user to select an item 

 */

int menu_get(state,mouse,button,exit)
struct menu_state *state;
int mouse;			/* fd to read mouse data from */
int button;			/* button termination condition (not yet)*/
int exit;			/* off-menu exit codes */
   {
   register BITMAP *inside;	/* the menu */
   register int y_position;
   register int x_position;
   int push;
   int x_mouse, y_mouse;	/* mouse delta's */
   int done=0;
   int inverse;			/* selected item */
   int count;			/* number of items */
   int old;

   if (state == (MENU *) 0)
       return(-1);

   old = inverse = state -> current;
   count = state -> count;
   state->exit=0;

   /* set up text region */

   inside = bit_create(state->screen,state->menu_startx+MENU_BORDER,
                    state->menu_starty+MENU_BORDER - state->bar_sizey,
                    state->bar_sizex,state->bar_sizey*(count+2));

   /* make sure we aren't already exited */

   if (exit&EXIT_BOTTOM && inverse >= count) {
      old = inverse = count-1;
      BAR(inside,inverse);	/* on */
      }

   /* set initial blip position */

   x_position = state->x_pos;
   y_position = state->bar_sizey*(inverse+1) + state->bar_sizey/2;

   /* track the mouse */

   TRACKON(inside,x_position,y_position);		/* on */
   do {
      push = mouse_get(mouse,&x_mouse,&y_mouse);
      TRACKOFF(inside,x_position,y_position);		/* off */
      x_position += x_mouse;
      y_position -= y_mouse;
      y_position = BETWEEN(BLIP, y_position, (2+count)*state->bar_sizey-BLIP);

      if (x_position <= 0 && (exit&EXIT_LEFT)) {
         state->exit = EXIT_LEFT;
         done++;
         }
      else if (x_position >= BIT_WIDE(inside) && (exit&EXIT_RIGHT)) {
         state->exit = EXIT_RIGHT;
         done++;
         }

      x_position = BETWEEN(BLIP/2, x_position, BIT_WIDE(inside) - BLIP/2);

      TRACKON(inside,x_position,y_position);		/* on */

      if (done)
         break;

      /* fix bar */

      inverse = (2+count) * y_position / BIT_HIGH(inside) - 1;
      if (inverse != old) {
         TRACKOFF(inside,x_position,y_position);		/* off */
         if (old >=0 && old < count)
            BAR(inside,old);		/* off */
         if (inverse >=0 && inverse < count)
            BAR(inside,inverse);	/* on */
         old = inverse;

         if (inverse < 0 && exit&EXIT_TOP) {
            state->exit = EXIT_TOP;
            done++;
            }
         else if (inverse >= count && exit&EXIT_BOTTOM) {
            state->exit = EXIT_BOTTOM;
            done++;
            }
         TRACKON(inside,x_position,y_position);		/* on */
         }
      }
      while (push != button && !done);
   state->current = inverse;
   state->x_pos = x_position;
   TRACKOFF(inside,x_position,y_position);		/* off */
   bit_destroy(inside);
   return(0);
   }

/******************************************************************************

remove the menu drom the screen, restore previous screen contents

 */

struct menu_state *
menu_remove(state)
struct menu_state *state;
   {
   if (state == (MENU *) 0) return(state);
   if (state->save != (BITMAP *) 0) {
      bit_blit(state->screen,state->menu_startx,state->menu_starty,
         state->BIT_WIDE(save),state->BIT_HIGH(save),BIT_SRC,state->save,0,0);
      bit_destroy(state->save);
      state->save = (BITMAP *) 0;
      }
   return(state);
   }

/* free space associated with a menu */

int
menu_destroy(state)
struct menu_state *state;
   {
   register int i;

   menu_remove(state);

   if (state->menu != (BITMAP *)0)
      bit_destroy(state->menu);

   if (state->action != (struct menu_action *) 0) {
      for(i=0;i<state->count;i++)
         if (state->action[i].value)
            free(state->action[i].value);
      free(state->action);
      }

   free(state);
   return(0);
   }

/* put a character string into a bitmap - only used for menus */

int
put_str(map,x,y,font,op,str)
BITMAP *map;
register int x;
int y;
struct font *font;
int op;
register char *str;
   {
   register char c;
   register int wide = font->head.wide;
   register int high = font->head.high;

   while (c = *str++) {
      bit_blit(map,x,y-high,wide,high,op,font->glyph[c],0,0);
      x+=wide;
      }
   }

/* copy a menu  - for environment stacks */

struct menu_state *
menu_copy(menu)
register struct menu_state *menu;
   {
   register struct menu_state *tmp;
   register int i;
   char *alloc(), *malloc(), *save_line();

   if (menu == (struct menu_state *) 0)
      return(menu);

   if ((tmp = (struct menu_state *) malloc(sizeof(struct menu_state))) 
                                                   == (struct menu_state *) 0)
      return((struct menu_state *) 0);

   bcopy((char *) menu,(char *)tmp,sizeof(struct menu_state));

   /* copy menu image */

   if (menu->menu) {
      tmp->menu = bit_alloc(BIT_WIDE(menu->menu),
                            BIT_HIGH(menu->menu),NULL_DATA,DEPTH);
      bit_blit(tmp->menu,0,0,BIT_WIDE(tmp->menu),BIT_HIGH(tmp->menu),
                            BIT_SRC,menu->menu,0,0);
      }

   /* copy menu values */

   if (menu->action != (struct menu_action *) 0) {
      tmp->action = (struct menu_action *) 
                    malloc(sizeof(struct menu_action)*menu->count);
      if (tmp->action)
         for(i=0;i<menu->count;i++) {
            tmp->action[i].value = save_line(menu->action[i].value);
            tmp->action[i].next_menu = menu->action[i].next_menu;
            } 
      }

   return(tmp);
   }
