/* show strength of audio signals (sound edittor version) */

#include <sys/ioctl.h>	/* for debug only */
#include <sun/audioio.h> /* for debug only */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/signal.h>
#include <stdio.h>
#include "term.h"
#include "se.h"

#define OTHER	NULL							/* future title expansion */
#define ICONIFY	"\rII\r"					/* iconify message */
#define DEFAULT	"tmp.au"					/* default file to edit */

extern struct menu_entry menu[];
extern struct menu_entry scale_menu[];
extern struct menu_entry file_menu[];
extern struct menu_entry filter_menu[];
extern struct menu_entry play_menu[];
extern struct menu_entry paste_menu[];
extern struct menu_entry icon_menu[];
extern struct menu_entry fft_menu[];
extern char item[];

extern struct icon icon;

extern int append;		/* append mode botch */

char line[512];		/* mgr input line */
char s_buff[80];		/* sprintf buffer */
char mail[30] = "-Mail-";			/* place to hold mail menu option */
char tmp_name[30];	/* place to hold tmp name */

static int clean();
static int show_it();
static int stop_play();

int debug;				/* for debugging */
int seg_num = 1;		/* arbitrary segment # for debugging */
int audio_fd = 0;		/* fd for /dev/audio */

int icon_x = -1, icon_y;	/* icon position */
int border;						/* window border size */
struct global global_b, *global = &global_b;			/* global display data */
int msg_id = 0;		/* for remote messages */
int sflg=0;			/* display samples instead of seconds */

int do_hist();

main(argc,argv)
int argc;
char **argv;
	{
	struct edit edit_b, *edit = &edit_b;			/* the edit list */
	struct cut cut_b, *cut = &cut_b;					/* the cut buffer */
	struct segment *seg;									/* current segment */
	char *name;												/* name to save audio in */
	void (*save)();										/* old signal handler */
	char my_host[32];										/* my host */
	char *index(), *rindex(), *c;

	unsigned char *data;	/* the data goes here */
	int x=0;					/* x coord */
	int y1,y2;				/* y coords */
	int fd;					/* file descriptor of sound file */
	int dmax;				/* maximum data extent */	
	int reshow=0;				/* reshow */
	int remark=0;			/* redo the marks */
	struct stat buf;		/* stat buff */
	register int i=0;		/* # of total samples */
	int mx,my;					/* mouse position */
	int iconified = 0;	/* currently iconified */
	int iflg = 0;			/* initial iconify flag */

	debug = getenv("DEBUG");

	if (argc>2 && strcmp(argv[1],"-s")==0) {
		sflg++;
		argc--, argv++;
		}

	if (argc>2 && strcmp(argv[1],"-i")==0) {
		iflg++;
		argc--, argv++;
		}

	if (argc<2) {
		fprintf(stderr,"usage: %s <sound file>\n",*argv);
		exit(1);
		}

	name = argc>1 ? argv[1] : DEFAULT;

	if (stat(name,&buf) != 0) {
		fprintf(stderr,"Cant open %s\n",name);
		exit(1);
		}

	if ((fd = open(name,0)) <=0) {
		fprintf(stderr,"Cant open %s\n",name);
		exit(1);
		}

	if ((data = (unsigned char *) malloc(buf.st_size)) == NULL) {
		fprintf(stderr,"Cant malloc %d\n",buf.st_size);
		exit(1);
		}

	read(fd,data,buf.st_size);
	close(fd);

	/* set up initial edit (this effort will support undo someday) */


	E(name) = argv[1];			/* file name */
	E(chunk) = 30;					/* chunk size */
	E(left) = 0;					/* where to start the display */
	E(size) = buf.st_size;		/* size of file */
	E(num) = 1;						/* only one segment - the entire file */
	E(start) = 0;					/* 1st byte of file */
	E(end) = buf.st_size;		/* last bytes of file */
	E(seg) = seg = (struct segment *) malloc(sizeof(struct segment));

	S(data) = data;			/* addr of 1st segment */
	S(count) = buf.st_size;
	S(next) = NULL;		/* first segment */
	S(num) = seg_num++;

	cut->data = NULL;
	cut->count = 0;

	/* set up window environment */

	signal(SIGINT,SIG_IGN);
	signal(SIGSEGV,clean);
	signal(SIGALRM,timer);

	m_setup(M_MODEOK);
	m_ttyset();
	m_push(P_MENU | P_EVENT | P_FLAGS);
	m_setmode(M_ABS);
	m_setmode(M_NOWRAP);
	m_setcursor(CS_INVIS);
	m_clear();

	/* initialize menus */

	menu_load(1,11,menu);
	menu_load(2,11,scale_menu);
	menu_load(3,4,file_menu);
	menu_load(4,3,play_menu);
	menu_load(5,5,filter_menu);
	menu_load(6,2,paste_menu);
	menu_load(8,7,fft_menu);
	menu_links();
	m_menuitem(2,2);
	m_nomenu();

	/* initialize events */

	m_setevent(BUTTON_1,"\rB1 %p\r");
	m_setevent(BUTTON_2,"\rB2 %p\r");
	m_setevent(BUTTON_2U,"\r$\r");
	m_setevent(BUTTON_1U,"\r$\r");
	m_setevent(RESHAPE,"\rR\r");
	m_setevent(REDRAW,"\rR\r");
	m_setevent(DEACTIVATE,"\rD\r");       /* window deactivated */ 

	/* message stuff */

	fchmod(fileno(m_termin),0600);			/* activate MGR messages */
	gethostname(my_host,sizeof(my_host));	/* my host */
	sprintf(tmp_name,"/tmp/se.%d",getpid());	/* tmp file name */

	download_icon(&icon,1);						/* for iconify mode */

	/* setup global data */

	G(thick) = THICK;
	G(bar) = BAR;
	get_font(&G(fw),&G(fh));

	m_sendme("\rR\r");		/* simulate redraw */

	/* main command loop.  Wait for a comand */

	while(m_gets(line)) {
		clear_message(global);
		reshow = remark = 0;
		dprintf(stderr,"SE got: %s",line);
		switch (*line) {
			case 'M':		/* handle message stuff */
				msg_id = atoi(line+2);
				dprintf(stderr,"Got a message from %d\n",msg_id);
				if ((c=index(line,'(')) && c[1] == 'H') { /* help message */
					sprintf(s_buff,"F;audio/ulaw;edit some sound");
					m_sendto(msg_id,s_buff);
					dprintf(stderr,"SE got HELP message from %d\n",msg_id);
					msg_id = 0;
					m_setmode(M_WOB);
					m_flush();
					sleep(1);
					m_clearmode(M_WOB);
					}
				else if (c && c[1] == 'M') {				  /* mail stuff */
					*rindex(c,')') = '\0';
					sprintf(s_buff,"F;audio/ulaw;edit some sound");
					m_sendto(msg_id,s_buff);
					dprintf(stderr,"SE got MAIL message from %d\n",msg_id);
					m_sendme("\rIA\r");
					}
				else if (c && c[1] == 'D') {				  /* mail done (broken??) */
					unlink(tmp_name);
					dprintf(stderr,"SE unlinking %s\n",tmp_name);
					msg_id = 0;
					}
				else {
					dprintf(stderr,"snap got UNKNOWN message from %d\n",msg_id);
					m_sendto(msg_id,"?");
					msg_id = 0;
					}
				break;
			case 'D':		/* Deactivated window in server mode */
				if (msg_id) {					
					m_sendto(msg_id,"X");
					m_sendme(ICONIFY);
					strcpy(item,"save =>");
					menu_load(1,10,menu);
					menu_links();
					dprintf(stderr,"SE: message aborted\n");
					msg_id = 0;
					title(edit,cut,OTHER);
					}
				break;
			case 'R':		/* reshape, draw  */
				m_clear();
				get_global(global);
				title(edit,cut,OTHER);
				panel(global);
				reshow++;
				break;
			case 'B':		/* button  1 and 2 (needs fixing) */
				sscanf(line+2,"%d %d\n",&mx,&my);
				mx = BETWEEN(0,mx-3,G(w));
				my = BETWEEN(0,my-3,G(h));

				if (my > G(d2)) { 					/* on scroll bar */
					E(left) = BETWEEN(0,E(size) * mx / G(w),E(size)-G(w)*E(chunk));
					reshow++;
					}
	
				else if (*(line+1) == '1') {		/* button 1 - move right mark */
					E(end) = E(left) + mx * E(chunk);
					if (E(end) > E(size))
						E(end) = E(size);
					if (E(end) < E(start))
						E(start) = E(end);
					title(edit,cut,OTHER);
					remark++;
					}
	
				else if (my < G(d1)) {				/* button 2 - do the menu */
						m_selectmenu(1);
						}

				else {									/* button 2, move left mark */
					E(start) = E(left) + mx * E(chunk);
					if (E(start) > E(size))
						E(start) = E(size);
					if (E(end) < E(start))
						E(end) = E(start);
					title(edit,cut,OTHER);
					remark++;
					}
				break;
			case '$':		/* button up -- turn off the menu */
				dprintf(stderr,"SE: NO MENU\n");
				m_nomenu();
				break;
			case 'P':		/* play */
				x = atoi(line+2);
				save = signal(SIGINT,stop_play);
				switch(x) {
					case 2:		/* cut buffer */
						if (cut->count && play(NULL,cut->count,NULL)) {
							play(cut->data,cut->count,NULL);
							play(NULL,NULL,NULL);
							}
						break;
					case 3:		/* mark */
						step(edit,E(start),E(end)-E(start),play,NULL);
						break;
					case 1:		/* whole file */
					default:
						step(edit,0,E(size),play,NULL);
						break;
					}
				signal(SIGINT,save);
				break;
			case 'X':		/* blast the data to silence (temporary) */
				x = atoi(line+2);

				/* just get parameters */

				if (x==0) {
					sprintf(s_buff,"Enter fade %% (%d):",fade_limit);
					x = atoi(get_str(s_buff));
					if (x>0 && x <=100)
						fade_limit = x;
					clear_message();
					}
				else {
					step(edit,E(start),E(end)-E(start),blast,x);
					reshow++;
					}
				break;
			case 'F':		/* do the histogram */
				x = atoi(line+2);
				step(edit,E(start),E(end)-E(start),do_hist,(char *) NULL);
				break;
			case 's':		/* change scale */
				x = atoi(line+2);
				if (x>0) {
					E(chunk) = x;
					title(edit,cut,OTHER);
					dprintf(stderr,"Chunk size is %d\n",E(chunk));
					reshow++;
					}
				break;
			case 'g':		/* get file to cut buffer */
				(void) get_str("Enter file to fetch into cut buffer: ");
				fetch_it(cut,line);
				title(edit,cut,OTHER);
				break;
			case 'f':				/* save to a file */
				x = atoi(line+2);
				if (msg_id) {		/* sending file as server, make up name */
					name = tmp_name;
					unlink(tmp_name);
					}
				else {
					sprintf(s_buff,"enter file name (default is %s):",E(name));
					(void) get_str(s_buff);
					name = strlen(line) ? line : E(name);
					}
				switch(x) {
					case 2:		/* cut buffer */
						if (cut->count && file_it(NULL,cut->count,name)) {
							file_it(cut->data,cut->count,name);
							file_it(NULL,NULL,name);
							}
						break;
					case 3:		/* mark */
						step(edit,E(start),E(end)-E(start),file_it,name);
						break;
					case 4:		/* mark (append) */
						append = 1;
						step(edit,E(start),E(end)-E(start),file_it,name);
						break;
					case 1:		/* whole file */
					default:
						step(edit,0,E(size),file_it,name);
						break;
					}
				if (msg_id) {				/* send filename to mail */
					sprintf(s_buff,"F;%s:%s",my_host,name);
					dprintf(stderr,"SE: Sending message [%s]\n",s_buff);
					m_sendto(msg_id,s_buff);
					msg_id=0;
					message(global,"Sound sent to mail");
					strcpy(item,"save =>");
					menu_load(1,10,menu);
					menu_links();
					m_clearmode(M_ACTIVATE);
					m_flush();
					sleep(2);
					clear_message(global);
					m_sendme(ICONIFY);
					}
				break;
			case 'p':		/* paste */
				x = atoi(line+2);
				if (x==2) {	/* merge */
					step(edit,E(start),cut->count,merge,cut->data);
					}
				else {
					paste(edit,cut,E(start));
					E(end) = E(start) + cut->count;
					}
				reshow++;
				break;
			case 'd':		/* debug */
				do_debug(global,edit,cut);
				m_sendme("R\r");	/* simulate a redraw */
				break;
			case 'c':		/* cut */
				cut_it(edit,cut);
				E(left) = BETWEEN(0,E(left),E(size)-G(w)*E(chunk));
				title(edit,cut,OTHER);
				reshow++;
				break;
			case 'I':		/* iconify stuff */
				switch(line[1]) {
					case 'I':		/* iconify */
						m_push(P_WINDOW|P_EVENT|P_FLAGS|P_POSITION|P_MENU);
						m_setevent(MOVE,"\rIM\r");
						m_setevent(RESHAPE,"\rIS\r");
						m_setevent(REDRAW,"\rIR\r");
						m_setevent(ACCEPT,"\rM %f (%m)\r");		/* accept message */
						m_setevent(NOTIFY,"se");					/* type of thingy */
						menu_load(1,2,icon_menu);
						m_selectmenu(1);
						m_shapewindow(icon_x,icon_y,icon.w+2*border,icon.h+2*border);
						m_setmode(M_ABS);
						m_func(B_SRC);
						download_icon(&icon,1);
						m_bitcopyto(0,0,icon.w,icon.h,0,0,0,1);
						m_clearmode(M_ACTIVATE);
						m_gets(line); 		/* swallow button up */
						iconified = 1;
						break;
					case 'A':		/* un-iconify */
						iconified = 0;
						m_pop();
						if (msg_id) {					/* note we are in service mode */
							strcpy(item,"mail =>");
							message(global,"Activated from mail");
							menu_load(1,10,menu);
							menu_links();
							}
						title(edit,cut,OTHER);
						break;
					case 'M':		/* move the icon */
						get_size(&icon_x,&icon_y,0,0);
						break;
					case 'S':		/* shape the icon */
						get_size(&icon_x,&icon_y,0,0);
						m_shapewindow(icon_x,icon_y,icon.w+2*border,icon.h+2*border);
						/* no break */
					case 'R':		/* redraw the icon */
						m_bitcopyto(0,0,icon.w,icon.h,0,0,0,1);
						break;
					}
				break;
			case 'q':		/* exit */
				clean(0);
				break;
			}
		if (reshow) {
			int num = E(chunk) == 1 ? G(w)/2 : G(w)*E(chunk);
			step(edit,E(left),num,show_it,(char *)edit);
			}
		if (remark || reshow)
			mark_it(global,edit);
		if (iflg && reshow) {			/* initial iconification */
			iflg = 0;
			m_sendme(ICONIFY);
			}
		m_flush();
		}
	clean(0);
	}

/* manage marks */

static int mark1 = -1, mark2 = -1;		/* previous mark location */

/* clear marks */

reset_mark()
	{
	mark1 = mark2 = -1;
	}

mark_it(global,edit)
struct global *global;
struct edit *edit;
	{
	int x0,x1;	/* start of mark */
	int width;	/* width of mark */

	x0 = E(left)* G(w) / E(size);
	width = G(w) * G(w) * E(chunk) / E(size);
	dprintf(stderr,"mark: %d-%d\n",x0,width);

	/* the scroll bar */

	m_func(B_SET);
	m_bitwrite(0,G(d2),G(w),G(thick));
	m_bitwrite(0,G(d3)-G(thick),G(w),G(thick));
	m_func(B_CLEAR);
	m_bitwrite(G(thick),G(d2)+G(thick),G(w)-2*G(thick),G(bar));
	m_func(B_INVERT);
	m_bitwrite(G(thick)+x0,G(d2)+2*G(thick),width,G(bar)-2*G(thick));

	/* the marks in the scroll bar */

	x0 = G(thick) + E(start) * G(w) / E(size);
	m_bitwrite(x0,G(d2),G(thick),2*G(thick) + G(bar));
	x1 =  2*G(thick) +  E(end) * G(w) / E(size);
	m_bitwrite(x1,G(d2),G(thick),2*G(thick) + G(bar));

	/* the marks in the data region */

	/* calculate new marks */

	x0 = x1 = -1;
	if (E(start) >= E(left) && E(start) <= E(left) + E(chunk)*G(w))
		x0 = (E(start) - E(left)) / E(chunk) - G(thick);
	if (E(end) >= E(left) && E(end) <= E(left) + E(chunk)*G(w))
		x1 = (E(end) - E(left)) / E(chunk);


	if (mark1 >= 0) {		/* erase old marks */
		m_bitwrite(mark1,G(d1),THICK,G(d2)-G(d1));
		}
	if (mark2 >= 0) {		/* erase old marks */
		m_bitwrite(mark2,G(d1),THICK,G(d2)-G(d1));
		}

	/* draw new marks */

	if (x0 >= 0) {
		m_bitwrite(x0,G(d1),THICK,G(d2)-G(d1));
		}
	if (x1 >= 0) {
		m_bitwrite(x1,G(d1),THICK,G(d2)-G(d1));
		}

	mark1 = x0, mark2 = x1;
	}

/* clean up and exit */

static int
clean(n)
int n;
	{
	if (msg_id)
		m_sendto(msg_id,"X");
	m_textreset();
	m_popall();
	m_setcursor(0);
	m_ttyreset();
	m_clear();
	unlink(tmp_name);
	switch(n) {
		case SIGSEGV:
		case SIGBUS:
		case SIGQUIT:
			perror("DIE DIE DIE");
			fprintf(stderr,"Error %d\n",n);
			abort();
			break;
		case SIGINT:
			fprintf(stderr,"Interrupted \n");
			break;
		}
	exit(n);
	}

/* do the info line (needs lots of work) */

#define TITLE		"SE.1"

title(edit,cut,other)
struct edit *edit;	/* edit info */
struct cut *cut;
char *other;			/* other misc stuff */
	{
	dprintf(stderr,"Doing title\n");
	m_move(0,0);
	if (sflg)
		fprintf(m_termout,
			"%s %.14s %d [%d->%d] (%d) (scale %d) %s",
			TITLE,E(name), E(size), E(start), E(end), E(end)-E(start),
			E(chunk),other?other:"");
	else
		fprintf(m_termout,
			"%s %.14s %d.%02d [%d.%02d->%d.%02d] (%d.%02d) (scale %d) %s",
			TITLE,E(name), CNVT(E(size)), CNVT(E(start)), CNVT(E(end)),
			CNVT(E(end)-E(start)),
			E(chunk),other?other:"");
	if (cut->count)
		fprintf(m_termout," (cut %d.%02d)",CNVT(cut->count));
	m_cleareol();
	m_flush();
	}

/* draw the panel dividers */

panel(global)
struct global *global;
	{
	m_func(B_SET);
	m_bitwrite(0,G(fh),G(w),G(thick));
/*
	m_bitwrite(0,G(d3)-2*G(thick)-G(bar),G(thick),G(bar)+2*G(thick));
	m_bitwrite(G(w)-G(thick),G(d3)-2*G(thick)-G(bar),G(thick),G(bar)+2*G(thick));
*/
	}

/* fetch the global data */

get_global(global)
struct global *global;
	{
	int x,y;
	get_size(&x,&y,&G(w),&G(h));
	if (icon_x == -1) {
		get_param(0,0,0,&border);
		icon_x = x, icon_y = y;
		}
	G(d1) = G(fh) + G(thick);					/* top of data display */
	G(d2) = G(h)-3*G(thick) - G(bar) - G(fh);	/* bottom of data display */
	G(d3) = G(h)-G(fh) - G(thick);					/* bottom of bar display */
	}

/* print status info */

do_debug(global,edit,cut)
struct global *global;
struct edit *edit;
struct cut *cut;
	{
	struct segment *seg;
	int fd;		/* to play samples */
	int offset = 0;

	alarm(0);
	timer(0);
	fd = open(AUDIO,1);
	
	m_clear();
	fprintf(m_termout,"Geometry: win: %d x %d, font %d x %d, data: %d - %d\r\n",
		G(w),G(h),G(fw),G(fh),G(d1),G(d2));
	fprintf(m_termout,"edit %s, %d bytes, mark: %d - %d, display %d at %d\r\n",
		E(name), E(size), E(start), E(end), E(chunk), E(left));

	m_gets(line); 		/* swallow button up */
	for(seg=E(seg);seg;seg=S(next)) {
		fprintf(m_termout," 0x%x (%d): from %d to %d (%d) -> 0x%x...",
				seg,S(num),offset, offset + S(count),S(count),S(next));
		offset += S(count);
		m_flush();
		write(fd, S(data),S(count));
		ioctl(fd,AUDIO_DRAIN,NULL);
		fprintf(m_termout,"done\r\n");
		m_gets(line);
		}

	if (cut->count > 0) {
		fprintf(m_termout,"playing cut buffer %d...",cut->count);
		m_flush();
		write(fd, cut->data,cut->count);
		ioctl(fd,AUDIO_DRAIN,NULL);
		fprintf(m_termout,"done\r\n");
		}
		
	m_nomenu();
	m_flush();
	if (fd>0)
		close(fd);
	}


/* manage sound data area - called via step */

static int
show_it(data,count,arg)
unsigned char *data;
int count;
char *arg;
	{
	struct edit *edit = (struct edit *) arg;
	int sample;						/* current sample */
	static int chunk;				/* samples per line segment */
	static int min;				/* minimum sample value */
	static int max;				/* maximum, for chunk */
	static int wide;				/* plotting width */
	static int x;					/* x position */
	static int last_x, last_y;	/* previous position */

	if (data==NULL && count > 0) {		/* first call */

		/* clear the display area */

		m_func(B_CLEAR);
		m_bitwrite(0,G(d1),G(w),G(d2)-G(d1));
		m_func(B_SET);
		min = MAX;				/* minimum sample value */
		max = -MAX;				/* maximum, for chunk */
		chunk = E(chunk);		/* chunk size for plotting */
		wide = G(d2)-G(d1);
		x = 0;					/* start at left edge */
		m_go(0,wide/2);
		}

	else if (count > 0) {		/* draw some data */
		while (count-- > 0) {
			sample = I2sb[*data++];
			if (E(chunk) <= 1) {
				m_draw(x,(MAX+sample)*wide/(2*MAX));
				x+=2;
				continue;
				}
			else {
				if (sample > max) max = sample;
				if (sample < min) min = sample;
				if (--chunk <= 0) {
					int y1 = (MAX+min)*wide/(2*MAX);
					int y2 = (MAX+max)*wide/(2*MAX);
					if (y2==y1) y2++;
					chunk = E(chunk);
					m_bitwrite(x,G(d1)+y1,1,y2-y1);
					min = MAX;
					max = -MAX;
					x++;
					}
				}
			}
		}
	else {							/* done */
		reset_mark();
		}
	return(1);
	}

/* stop "play" in progress */

static int
stop_play()
	{
	dprintf(stderr,"Stopping audio\n");
	message(global,"Stopping play");
	ioctl(audio_fd,I_FLUSH,FLUSHW);
	}

/* display a message */

static int msg = 0;
int
message(global,message)
struct global *global;
char *message;
	{
	m_movecursor(0,G(h));
	fprintf(m_termout,message);
	m_cleareol();
	m_move(0,0);
	m_flush();
	msg = 3;
	}

/* clear a message from the message line */

int
clear_message(global)
struct global *global;
	{
	if (msg-- == 1) {
		m_movecursor(0,G(h));
		m_cleareol();
		m_move(0,0);
		m_flush();
		}
	}

/* fetch a string from the user, put result in "line"  */

char
*get_str(message)
char *message;
	{
	int x,y,w,h;		/* text region coords */
	/* print message, set up input environment */

	x = G(fw)*strlen(message);
	y = G(h) - G(fh);
	w = G(w) - x;
	h = G(h);

	m_movecursor(0,G(h));
	fprintf(m_termout,message);
	m_textregion(x,y,w,h);
	m_clear();
	m_setcursor(CS_BOX);

	/* fetch the line */

	m_nomenu();
	m_push(P_EVENT|P_MENU);
	m_flush();
	m_setecho();
	m_gets(line);
	m_setnoecho();
	m_flush();
	line[strlen(line)-1] = '\0';	/* strip off NL */
	m_pop();

	/* reset the environment */

	m_setcursor(CS_INVIS);
	m_textreset();
	m_flush();
	return(line);
	}

/* down load an icon (taken from iconmail) */

static int  local_mode = -1;	/* local mode bits for tty */

download_icon(icon,where)
register struct icon *icon;   /* name of icon to download */
int where;        /* bitmap to download icon to */
   {

   int size;
   int w_in=0, h_in=0;

	/* first try the local machine */

	dprintf(stderr,"looking for %s\n",icon->name);
	m_bitfile(where, icon->name, &w_in, &h_in );

   if (h_in==0 || w_in==0) {  /* can't find icon */
      dprintf(stderr,"Couldn't find %s, downloading\n",icon->name);
      if (local_mode == -1)
         ioctl(fileno(m_termout),TIOCLGET,&local_mode);
      local_mode |= LLITOUT;
      ioctl(fileno(m_termout),TIOCLSET,&local_mode);

      size = icon->h * (((icon->w+15)&~15)>>3);
      m_bitldto(icon->w,icon->h,0,0,where,size);
      m_flush();
      write(fileno(m_termout),icon->data,size);
      local_mode &= ~LLITOUT;
      ioctl(fileno(m_termout),TIOCLSET,&local_mode);
      }
   else {
      dprintf(stderr,"Found %s (%d x %d) expected %d x %d\n",
               icon->name,w_in,h_in,icon->w,icon->h);
      icon->w = w_in;
      icon->h = h_in;
      }
   icon->type = where;
   }

menu_links()
	{
	m_linkmenu(1,1,2,MF_AUTO);
	m_linkmenu(1,4,3,MF_AUTO);
	m_linkmenu(1,0,4,MF_AUTO);
	m_linkmenu(1,7,5,MF_AUTO);
	m_linkmenu(1,3,6,MF_AUTO);
	m_linkmenu(1,8,8,MF_AUTO);
	}

/* this is temporary */

int
show_hist(hist)
unsigned int hist[];		/* list of bucket values */
	{
	register int i;
	register unsigned int sum=0;
	int num = (G(w)/G(fw)/4)-1;	/* entries per row */
	int rows = G(h)/G(fh) - 1;
	char line[10];

	for(i=0;i<256;i++)
		sum += hist[i];

	m_push(P_WINDOW|P_TEXT);
	m_textreset();
	m_clear();
	m_gets(line);		/* eat button up */
	for(i=0;i<256;i++) {
		if (!(i%num))
			fprintf(m_termout,"\r\n%.3d: ",i);
		fprintf(m_termout,"%.3d ",hist[i]);
		if ((i+1)%(num*rows)==0) {
			m_flush();
			m_gets(line);
			}
		}
	m_flush();
	m_gets(line);
	m_pop();
	m_sendme("\r$\r");
	m_flush();
	}
