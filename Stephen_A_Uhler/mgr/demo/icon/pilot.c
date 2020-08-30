/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */

/* pan around in an MGR bitmap (sdh), editted by sau */

#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include "term.h"

#define TERM		"mgr"
#define dprintf	if (debug) fprintf
#define SCALE 6
#define MAXPAGES	20
#define BORDER 5
#define MAXQ	6

#ifndef E_XMENU
#define E_XMENU      'X'   /* extended menu operations */                 
#endif

#define fsleep(X) \
   { \
   struct timeval time; \
   time.tv_sec = ((X)/100); \
   time.tv_usec = ((X)%100)*10000; \
   select(0,0,0,0,&time); \
   }

int debug;
int wx, wy, ww, wh; /* window dims */
int mw, mh; /* map width and height */
int sx, sy, sw, sh; /* subwindow on map coords */
int old_x = -1 , old_y = -1;
int q = 0;
int fw, fh; /*	font size */
static char *usage = "Usage: %s [-r] file...\n";
int reverse, tilt;
int cleanup();
int timer();
char line[256];

static struct menu_entry menu[] = {
	"goto page =>","",
	"next page","n\r",
	"prev page","p\r",
   "quit","q\r",
};

struct menu_entry page_menu[] = {
	"1", "P 1\r", "2", "P 2\r", "3", "P 3\r", "4", "P 4\r", "5", "P 5\r",
	"6", "P 6\r", "7", "P 7\r", "8", "P 8\r", "9", "P 9\r", "10", "P 10\r",
	"11", "P 11\r", "12", "P 12\r", "13", "P 13\r", "14", "P 14\r",
	"15", "P 15\r", "16", "P 16\r", "17", "P 17\r", "18", "P 18\r",
	"19", "P 19\r", "20", "P 20\r",
	};

cleanup()
{
	m_bitdestroy(1);
	m_pop();
	m_ttyreset();
	exit(0);
}

main(argc,argv)
char **argv;
{
	char *getenv();
	char *term = getenv("TERM");
	char *fname, *fixfilename();
	int bx, by;
	int c;
	int file_no = 0;
	
	debug = (int) getenv("DEBUG");

	if (term!=NULL  && strcmp(term,TERM)!=0) {
		fprintf(stderr,"%s only works on %s terminals\n",argv[0],TERM);
		exit(1);
	}

	if (argc > 1 && strcmp(argv[1],"-r")==0) {
		reverse=1;
		argc--, argv++;
		}

	argv++; argc--;
	m_setup(M_MODEOK);
	m_push(P_CURSOR|P_POSITION|P_WINDOW|P_BITMAP|P_EVENT|P_FLAGS|P_MENU|P_TEXT);
	signal(SIGINT,cleanup);
	signal(SIGALRM,timer); 
	m_setcursor(CS_INVIS);
	m_setmode(M_ABS);

	/* page page menu */
   menu_load(1,4,menu);
	menu_load(2,argc>20?20:argc,page_menu);   
	m_linkmenu(1,0,2,0);	
   m_selectmenu(1);
	m_menuitem(1,1);
	get_font(&fw,&fh);
	fh += BORDER;
	get_size(&wx, &wy, &ww, &wh);
	wh -= fh;
	sx = sy = 0;
	sw = ww;
	sh = wh;
	m_setevent(RESHAPE,"S\r");
	m_setevent(REDRAW,"R\r");
	m_setevent(BUTTON_1,"B %p\r");
	m_setevent(BUTTON_1U,"$\r");
	
	m_ttyset();/* no echo */
	m_clear();
	m_setmode(M_ABS);
	m_scrollregion(0,0);
	m_func(B_SET);
	m_bitwrite(0,fh-BORDER+1,ww,BORDER-2);	
	m_func(B_COPY);
	file_no = get_file(argv,reverse,file_no,argc);
	while(m_gets(line) && !feof(m_termin)) {
		int sig = sigblock(sigmask(SIGALRM));
		c = *line;
		switch(c) {
		case 'P':   /* goto page */
			sscanf(line+1, "%d", &file_no);
			file_no = get_file(argv,reverse,file_no-1,argc);
			break;
		case 'n': case '\012':	/* next file */
			file_no = get_file(argv,reverse,file_no+1,argc);
			break;
		case 'p':   /* previous file */
			file_no = get_file(argv,reverse,file_no-1,argc);
			break;
		case '$':	/* button release */
			end_timer();
			break;
		case 'X':	/* button hit repeat */
			q--;
			sscanf(line+1,"%d %d",&bx,&by);
			move((bx-ww/2)/SCALE,(by-wh/2)/SCALE);
			break;
		case 'B':	/* button hit */
			sscanf(line+1,"%d %d",&bx,&by);
			move((bx-ww/2)/SCALE,(by-wh/2)/SCALE);
			q = 1;
			start_timer(2);
			break;
		case 'R':
			m_func(B_CLEAR);
			m_bitwrite(0,0,ww,wh+BORDER);
			m_func(B_SET);
			m_bitwrite(0,fh-BORDER+1,ww,BORDER-2);	
			m_func(B_COPY);
			m_bitcopyto(0, fh, sw, sh, sx, sy, 0, 1);
			break;
		case 'S':
			get_size(&wx, &wy, &ww, &wh);
			wh -= fh;
			sw = ww;
			sh = wh;
			m_func(B_SET);
			m_bitwrite(0,fh-BORDER+1,ww,BORDER-2);	
			m_func(B_COPY);
			m_bitcopyto(0, fh, sw, sh, sx, sy, 0, 1);
			break;
		case 'Q': case 'q': case '\004':
			cleanup();
			break;
		default:
			printf("Got [%o]\n",*line);
			break;
		}
	m_flush();
	sigsetmask(sig);
	}
	cleanup();
}

move(dh, dv)
int dh, dv;
{
	sx += dh;
	if (sx + sw > mw) sx = mw - sw;
	if (sx < 0) sx = 0;

	sy += dv;
	if (sy + sh > mh) sy = mh - sh;
	if (sy < 0) sy = 0;

	dprintf(stderr,"Moving %d x %d to %d , %d\n",
				sw,sh,sx,sy);
	if (old_x != sx || old_y != sy) {
		m_bitcopyto(0, fh, sw, sh, sx, sy, 0, 1);
		old_x = sx, old_y = sy;
		}
	
}

char *
fixfilename(s)
char *s;
{
	char path[1024], *getenv();
	int i;

	if (!s || *s == '/') return(s);
	getwd(path);
	i = strlen(path);
	strcat(path, "/");
	strcat(path, s);
	return(path);
}

/* get the next file, return index */

int
get_file(name,rev,num,max)
char **name;		/* list of file names */
int rev;				/* true if reverse */
int num;				/* number in list */
int max;				/* max number in list */
	{
	char *fname;

	if (num < 0) return(0);
	if (num > max) return(max);
	
	if (!name[num]) {
		fprintf(m_termout, "\rfile (%d of %d) not found \033c",num+1,max);
		return(num);
		}
	fname = fixfilename(name[num]);
	dprintf(stderr,"\rGetting file %s\r\n",fname);
	m_bitfromfile(1, fname);
	m_gets(line);
	sscanf(line, "%d %d", &mw, &mh);
	if (mw == 0 || mh == 0) {
		fprintf(m_termout, "\rfile %s (%d of %d) invalid format \033c",
				fname,num+1,max);
		return(num);
		}
	if (rev) {
		m_func(BIT_NOT(B_DST));
		m_bitwriteto(0, 0, mw, mh, 1);
		m_func(B_COPY);
		}
	sx = sy = 0;
	sw = ww;
	sh = wh;
	old_x = old_y = -1;
	m_bitcopyto(0, fh, sw, sh, sx, sy, 0, 1);
	m_menuitem(2,num);
	if (num == 0)
		m_menuitem(1,1);
	else if (num >= max-1)
		m_menuitem(1,2);
	fprintf(m_termout, "\rfile %s (%d of %d) \033c",
			fname,num+1,max);
	return(num);
	}

/* timer routines */

static struct itimerval val;
static struct timeval time;
static int e_count = 0;
 
int
timer()
   {
   if (q > MAXQ) {
		dprintf(stderr,"max_q at %d\n",e_count);
      return(0);
      }

	m_sendme("X %p\r");
   m_flush();
   q++;
	e_count++;
   return(0);
   }

/* start the timer to "go off" on a regular basis */

int
start_timer(n)
int n;               /* 100ths of seconds */
   {
	static struct timeval itime = {0,300000};	/* initial value of timer */

   time.tv_sec = n/100;
   time.tv_usec = (n%100)*10000;
   val.it_interval = time;
   val.it_value = itime;
   return(setitimer(ITIMER_REAL,&val,NULL));
   }

int
end_timer(n)
int n;               /* 100ths of seconds */
   {
   time.tv_sec = 0;
   time.tv_usec = 0;
   val.it_interval = time;
   val.it_value = time;
   return(setitimer(ITIMER_REAL,&val,NULL));
   }
