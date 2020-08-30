/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/src/RCS/data.c,v 1.1 91/02/11 09:46:19 sau Exp Locker: sau $
	$Source: /home/sau/mgr/src/RCS/data.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/src/RCS/data.c,v $$Revision: 1.1 $";

/* static data items for window manager */

#include "bitmap.h"
#include "defs.h"
#include "font.h"

/* bit maps */

#include "icons.h"

/* space to store screen covered by mouse */

static char c_save[ BIT_Size(16,16,DEPTH) ];


/* structures for mouse icons  all icons are 32 bits wide
 * even though currently on 16 bits are used.  This way all
 * versions can use 32 bit aligned bitmaps
 */

bit_static(pattern,32,4,pat_data,1);
bit_static(mouse_arrow,32,32,c_arrow1,1);
bit_static(mouse_box,32,32,c_box,1);
bit_static(mouse_cut,32,32,c_cut,1);
bit_static(mouse_cup,32,32,c_cup,1);
bit_static(mouse_save,32,16,c_save,DEPTH);
bit_static(mouse_cross,32,16,c_cross,1);
bit_static(mouse_bull,32,8,c_bull,1);
bit_static(mouse_bull2,32,8,c_bull2,1);

/* menus */

char *active_menu[] = {		/* active-window menu */
   "reshape",
   "move",
   "bury",
#ifdef CUT
   "cut",
   "paste",
#endif
   "- - - -",
   "destroy",
   (char *) 0};

char *main_menu[] = {		/* primary menu */
   "new window",
   "redraw",
   "quit",
   (char *) 0};

char *full_menu[] = {		/* primary menu  - no more windows allowed */
   "redraw",
   "quit",
   (char *) 0};

char *quit_menu[] = {		/* to verify quit */
   "cancel",
   "suspend",
   "- - - -",
   "really quit",
   (char *) 0};
  

/* menu functions - these have a 1-1 corrospondance with the menu items */

int new_window(), shape_window(), move_window(), destroy_window(), quit();
int redraw(), hide_win(), nothing();
#ifdef CUT
int cut(), paste();
#endif

function main_functions[] = {
   new_window,
   redraw,
   quit,
   (function) 0 };

function full_functions[] = {
   redraw,
   quit,
   (function) 0 };

function active_functions[] = {
   shape_window,
   move_window,
   hide_win,
#ifdef CUT
   cut,
   paste,
#endif
   nothing,
   destroy_window,
   (function) 0 };

/* default font info */

char *font_dir = FONTDIR;
char *fontlist[MAXFONT];

/* default icon info */
char *icon_dir = ICONDIR;

/* raster of table for inverted screen (this doesn't belong here) */

int rev_ops[] = {
   0xf,0x7,0xb,0x3,0xd,0x5,0x9,0x1,0xe,0x6,0xa,0x2,0xc,0x4,0x8,0x0
   };

BITMAP *m_rop;								/* current mouse bit map */
int next_window=0;						/* next available window count */
struct font *font;						/* default font */
BITMAP *screen;							/* default screen */
WINDOW *active = (WINDOW *) 0;		/* window connected to keyboard */
WINDOW *last_active = (WINDOW *) 0;	/* previous window connected to keyboard */
int button_state = 0;					/* state of the mouse buttons */
int mouse, mousex, mousey;				/* mouse fd, x-coord, y-coord */
int debug = 0;								/* ==1 for debug prints */
int mouse_on = 0;							/* 1 iff mouse track is on */
char *snarf = (char *) 0;				/* place to keep snarfed text */
char *message = (char *) 0;			/* place to keep message */
char *start_command;						/* command for shell at window start */
char *init_command;						/* str to send window upon creation */
int id_message = 0;						/* id of message sender */
short buckey_map = 0;					/* mapping from ptty to buckey key */
unsigned int init_flags = INIT_FLAGS;	/* initial flags for new windows */
int mask = 0;								/* process mask for select */
int poll = 0;								/* processes with non-processed but
												 * already read data */
#ifdef DEBUG
char debug_level[] = "                                ";	/* debug flags */
#endif

/* aarg! */

nothing() {}
