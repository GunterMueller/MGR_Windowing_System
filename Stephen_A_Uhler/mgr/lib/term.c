/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: term.c,v 4.3 88/07/01 09:33:49 bianchi Exp $
	$Source: /tmp/mgrsrc/lib/RCS/term.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/lib/RCS/term.c,v $$Revision: 4.3 $";

/* routines for writing to mgr terminal emulator */

#include "term.h"
#include "restart.h"

FILE	*m_termout;
FILE	*m_termin;
int	m_flags;
int	m_envcount = 0;
int	m_saveenvcount = 0;
char	m_escchar = ESC;
char	m_menuchar = M_DELIM;

jmp_buf _env;

struct sgttyb	sgtty__save[TTYMAX];
int		sgtty_cnt = 0;
char		m_linebuf[MAXLINE];
static char	*m_fields[16];

/******************************************************************************
 *
 *	setup
 */

int
m_setup(flags)
int flags;
   {
   m_flags = flags;

   if (!(m_flags&M_DEBUG)) {
      m_termout = fopen(M_DEVICEOUT,"w");
      m_termin = fopen(M_DEVICEIN,"r");
      }

   if (m_termin == NULL || m_termout == NULL) 
      m_flags |= M_DEBUG;

   if (m_flags&M_DEBUG) {
      m_termin = stdin;
      m_termout = stdout;
      }
   return(m_flags);
   }

/******************************************************************************
 *
 *	get generic window parameters
 */

int
get_info(type,list)
int type;
char **list;
   { 
   if (type > G_MAX )
      return(-1);
   switch( type ) {
   case G_ALL:
   case G_ALLMINE:
	return(-1);
   }
   _m_ttyset();
   m_getinfo(type);
   m_gets(m_linebuf);
   _m_ttyreset();
   return  mparse(m_linebuf,list); 
   }

/******************************************************************************
 *
 *	read window parameters off of standard input
 */

int
get_windata(windatap)
struct window_data *windatap;
   { 
   if( mparse(m_gets(m_linebuf),m_fields) < 8 )
	return 0;
   windatap->x = atoi(m_fields[0]);
   windatap->y = atoi(m_fields[1]);
   windatap->w = atoi(m_fields[2]);
   windatap->h = atoi(m_fields[3]);
   strcpy(windatap->tty,m_fields[4]);
   windatap->num = atoi(m_fields[5]);
   windatap->status = *m_fields[6];
   windatap->setid = atoi(m_fields[7]);
   return 1;
}

/******************************************************************************
 *
 *	Get window parameters, one window at a time.
 *	Returns 1 if window_data structure has been filled, 0 otherwise.
 *	It is important to call get_eachwin() in a tight loop that doesn't
 *	ever exit, so that all the data is picked up.
 */

int
get_eachwin( windatap )
struct window_data *windatap;
   { 
   static int i = 0;

   if( !i ) {
      _m_ttyset();
      m_getinfo(G_ALL);
   }
   i = get_windata( windatap );
   if( !i )
      _m_ttyreset();
   return(i);
   }


/******************************************************************************
 *
 *	Get window parameters for the current window set, one window at a time.
 *	Returns 1 if window_data structure has been filled, 0 otherwise.
 *	It is important to call get_eachcleintwin() in a tight loop that
 *	doesn' tever exit, so that all the data is picked up.
 */

int
get_eachclientwin( windatap )
struct window_data *windatap;
   { 
   static int i = 0;

   if( !i ) {
      _m_ttyset();
      m_getinfo(G_ALLMINE);
   }
   i = get_windata( windatap );
   if( !i )
      _m_ttyreset();
   return(i);
   }

/******************************************************************************
 *
 *	Get all window parameters.
 *	NOTE CAREFULLY: The array of window_data structures pointed to by
 *	list must be more than the total number of windows on the screen;
 *	not a robust technique.
 *	get_eachwin() is recommended above this.
 */

int
get_all(list)
struct window_data *list;
   { 
   register int i;

   for(i=0;  get_eachwin( list );  i++ )
      list++;
   return(i);
   }

/******************************************************************************
 *
 *	Get window parameters for client windows.
 *	NOTE CAREFULLY: The array of window_data structures pointed to by
 *	list must be more than the total number of windows on the screen;
 *	not a robust technique.
 *	get_eachclientwin() is recommended above this.
 */

int
get_client(list)
struct window_data *list;
   { 
   register int i;

   _m_ttyset();
   m_getinfo(G_ALLMINE);
   for(i=0;  get_windata( list );  i++ )
      list++;
   _m_ttyreset();
   return(i);
   }

/******************************************************************************
 *
 *	get the window size
 */

int
get_size(x,y,wide,high)
int *x, *y, *wide, *high;

   { 
   register int count;

   if ((count = get_info(G_COORDS,m_fields)) >= 4) {
      if (x)
         *x = atoi(m_fields[0]); 
      if (y)
         *y = atoi(m_fields[1]); 
      if (wide)
         *wide = atoi(m_fields[2]); 
      if (high)
         *high = atoi(m_fields[3]); 
      return(count);
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	get the mouse coords
 */

int
get_mouse(x,y)
int *x, *y;

   { 
   register int count;

   if ((count = get_info(G_MOUSE2,m_fields)) >= 3) {
      if (x)
         *x = atoi(m_fields[0]); 
      if (y)
         *y = atoi(m_fields[1]); 
      return(atoi(m_fields[2]));
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	get system parameters
 */

int
get_param(host,xmax,ymax,border)
char *host;
int *xmax, *ymax, *border;

   { 
   register int count;

   if ((count = get_info(G_SYSTEM,m_fields)) >= 4) {
      if (host)
         strcpy(host,m_fields[0]);
      if (xmax)
         *xmax = atoi(m_fields[1]); 
      if (ymax)
         *ymax = atoi(m_fields[2]); 
      if (border)
         *border = atoi(m_fields[3]); 
      return(count);
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	get the cursor position
 */

int
get_cursor(x,y)
int *x, *y;

   { 
   register int count;

   if ((count = get_info(G_CURSOR,m_fields)) > 2) {
      if (x)
         *x = atoi(m_fields[0]); 
      if (y)
         *y = atoi(m_fields[1]); 
      return(2);
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	get the window size - rows and columns
 */

int
get_colrow(cols,rows)
int *cols, *rows;

   { 
   register int count;

   if ((count = get_info(G_WINSIZE,m_fields)) == 2) {
      if (cols)
         *cols = atoi(m_fields[0]); 
      if (rows)
         *rows = atoi(m_fields[1]); 
      return(2);
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	get the termcap entry
 */

char *
get_termcap()
   { 
   _m_ttyset();
   m_getinfo(G_TERMCAP);
   m_gets(m_linebuf);
   _m_ttyreset();
   return(m_linebuf);
   }

/******************************************************************************
 *
 *	get the font size
 */

int
get_font(wide,high)
int  *wide, *high;

   { 
   register int count, result;

   if ((count = get_info(G_FONT,m_fields)) >= 3) {
      if (wide)
         *wide = atoi(m_fields[0]); 
      if (high)
         *high = atoi(m_fields[1]); 
      result = atoi(m_fields[2]); 
      return(result);
      }
   else return(-count);
   }

/******************************************************************************
 *
 *	make a new window
 */

int
m_makewindow(x,y,wide,high)
int  x,y,wide,high;
   { 
   register int count, result;
   _m_ttyset();
   m_newwin(x,y,wide,high);
   m_gets(m_linebuf);
   _m_ttyreset();
   return(atoi(m_linebuf));
   }

/******************************************************************************
 *
 *	see if window is active
 */

int
is_active()
   { 
   *m_linebuf = '\0';
   get_info(G_STATUS,m_fields);
   return(*m_linebuf == 'a');
   }

/******************************************************************************
 *
 *	return last line read
 */

char *
m_lastline()
   {
   return(m_linebuf);
   }   

/******************************************************************************
 *
 *	down load a menu
 */

menu_load(n,count,text)
int n;				/* menu number */
int count;			/* number of menu items */
struct menu_entry *text;	/* menu choices */
   {
   register int i, len;

   if (text == (struct menu_entry *) 0)
      return (-1);

   /* calculate string lengths */

   len = 2 * count + 1;

   for (i=0;i<count;i++)
       len += strlen(text[i].value) + strlen(text[i].action);
   
   fprintf(m_termout,"%c%d,%d%c%c",m_escchar,n,len,E_MENU,m_menuchar);

   for (i=0;i<count;i++)
      fprintf(m_termout,"%s%c",text[i].value,m_menuchar);

   for (i=0;i<count;i++)
      fprintf(m_termout,"%s%c",text[i].action,m_menuchar);

   m_flush();
   }

/******************************************************************************
 *
 *	download a bitmap 
 */

m_bitload(x,y,w,h,data)
int x,y;
int w,h;
register char *data;
   {
   register int size = h * ((w+15)&~15)/8;		/* round to 16 bit boundary */
   m_bitld(w,h,x,y,size);
   while(size-- > 0)
      fputc(*data++,m_termout);
   m_flush();
   }

/******************************************************************************
 *
 *	Set and save the terminal mode  (if required);
 */

m_ttyset()
   {
	int code;
   struct sgttyb buff;

   code = gtty(fileno(m_termout),sgtty__save + sgtty_cnt);

   if (sgtty__save[sgtty_cnt].sg_flags&(ECHO|RAW)) {
      buff = sgtty__save[sgtty_cnt];
      buff.sg_flags &= ~(ECHO|RAW);
      m_flush();
      stty(fileno(m_termout),&buff);
      }

   if (sgtty_cnt < TTYMAX)
      sgtty_cnt++;

	return(code);
   }


/******************************************************************************
 *
 *	Restore the terminal mode 
 */

m_ttyreset()
   {
   if (sgtty_cnt)
      sgtty_cnt--;
   else
      return(1);

   if (sgtty__save[sgtty_cnt].sg_flags&(ECHO|RAW)) {
      m_flush();
      return(stty(fileno(m_termout),sgtty__save + sgtty_cnt));
      }
   else
      return(0);
   }

/******************************************************************************
 *
 *	change the terminal modes
 */

m_resetflags(flags)
   {
   struct sgttyb buff;
      gtty(fileno(m_termin),&buff);
      if (buff.sg_flags & flags) {
         buff.sg_flags &= ~flags;
	 m_flush();
         stty(fileno(m_termin),&buff);
         }
   }

m_setflags(flags)
   {
   struct sgttyb buff;
      gtty(fileno(m_termin),&buff);
      if (!( buff.sg_flags & flags)) {
         buff.sg_flags |= flags;
	 m_flush();
         stty(fileno(m_termin),&buff);
         }
   }

/**
	Given a bitmap id and an icon name,
	have mgr load that icon into that bitmap, returning the icon width
	and height via the given integer pointers.
	Return a positive number if successful.
	If the icon is not loaded, set the width and height values to 0 and
	return 0.
*/
int
m_bitfile( bitmapid, iconname, iconwidthp, iconheightp )
int	bitmapid;
char	*iconname;
int	*iconwidthp,
	*iconheightp;
   {
	*iconwidthp = *iconheightp = 0;
	m_bitfromfile( bitmapid, iconname );
	m_flush();
	return( sscanf( m_get(), "%d %d", iconwidthp, iconheightp ) == 2 );
   }

/*****************************************************************************
 *	parse a line into fields
 */

#ifndef iswhite
#define iswhite(x)	((x)==' ' || (x)=='\t')
#endif

int
mparse(line,fields)
register char *line;
register char **fields;
   {
   int inword = 0;
   int count = 0;
   char *start;
   register char c;

   for(start = line;(c = *line) && c != '\n';line++)
      if (inword && iswhite(c)) {
         inword = 0;
         *line = '\0';
         *fields++ = start;
         count++;
         }
      else if (!inword && !iswhite(c)) {
         start = line;
         inword = 1;
         }

   if (inword) {
      *fields++ = start;
      count++;
      if (c == '\n')
         *line = '\0';
      }
   *fields = (char *) 0;
   return(count);
   }

/******************************************
 *	stuff for restarting
 */

_Catch()
   {
   ioctl(fileno(m_termin),TIOCFLUSH,0);
   longjmp(_env,1);
   }

_Clean()
   {
   m_popall();
   exit(1);
   }
