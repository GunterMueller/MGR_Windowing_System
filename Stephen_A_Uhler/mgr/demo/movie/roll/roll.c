/* roll credits across the display */

#include <sys/time.h>
#include <stdio.h>
#include "bitmap.h"

/* sleep 100ths of seconds */

#define fsleep(s) \
   { \
   struct timeval time; \
   time.tv_sec = (s)/100; \
   time.tv_usec = ((s)%100)*10000; \
   select(0,0,0,0,&time); \
   }

#define SCROLL		5		/* scroll interval */
#define MAX_ITEMS	50
#define MAX_FONTS	10
#define MAX_CHARS	96		/* max chars in a font */
#define FIRST		'!'	/* first char in set */
#define LAST		'~'	/* last char in set */

#ifndef FONT_DIR
#define FONT_DIR		"/u/sau/roll/fonts"		/* path to "fonts" */
#endif
#define FONT_SUFFIX	"mgr"										/* mgr bitmap suffix */
#define WIDTH_SUFFIX	"widths"									/* width table suffix */

#define dprintf	if (debug)fprintf

 
#define GET_OPT(i)   \
	strlen(argv[i])>2 ? argv[i]+2 : argv[++i]

struct item {
	char *text;		/* text to display */
	int x,y;			/* text position */
	int font;		/* index into font table */
	int gap;			/* extra space between letters */
	int mode;		/* drawing mode */
	BITMAP *image;	/* pointer to the text image */
	BITMAP *mask;	/* place for drop shadow mask */
	BITMAP *save;	/* where to build image contents */
	};

struct font {
	char *name;						/* font name */
	BITMAP *image;					/* font image */
	unsigned short widths[MAX_CHARS];	/* character origins */
	};

struct item items[MAX_ITEMS];
int max_item=0;

struct font fonts[MAX_FONTS];
int max_font=0;
	
int debug;					/* for debug output */
char line[128];			/* input line */
BITMAP *screen;			/* bitmap to scroll */
BITMAP *save;				/* saved copy of screen */
int lm,rm;					/* margins */
int drop = 2;				/* drop shadow size */
int gap=0;					/* added gap between letters */
int mode=0;					/* drawing mode */

extern int no_initial;     /* flag to (share) to not-save initial display */

/* global routines */

BITMAP *bitmapread();
BITMAP *bit_alloc();
BITMAP *bit_open();
char *save_line();
char *malloc();
char *font_dir;

main(argc,argv)
int argc;
char **argv;
	{
	struct item *item;
	register int i,j;
	int offset;					/* initial offset */
	int min_offset;			/* minimum offset */
	int incr=SCROLL;			/* scroll increment */
	int top=0;						/* top clipping line */
	int bottom=0;					/* bottom clipping line */
	int left = 0;					/* left clipping */
	int right=0;					/* right clipping */
	int end=0;						/* when to stop credits */
	int begin=0;					/* where to start credits (from bottom) */
	int wait=0;						/* wait at end */
	int idle = 1;					/* slow down !! (ms) */
	int stamp=1;					/* how many loops between time stamps */
	BITMAP *display;				/* entire display */
	char *getenv();

	debug = (int)getenv("DEBUG");

	if ((font_dir = getenv("ROLL_FONTS")) == NULL)
		font_dir = FONT_DIR;

	if ((display=bit_open("/dev/bwtwo0"))==NULL) {
		fprintf(stderr,"Cant open screen\n");
		exit(1);
		}

	/* get arguments */

	for(i=1;i<argc;i++) {
		if (*argv[i] == '-') {
			switch(argv[i][1]) {
				case 'w':		/* wait */
					wait = atoi(GET_OPT(i));
					break;
				case 'T':		/* time stamp incr */
					stamp = atoi(GET_OPT(i));
					break;
				case 's':		/* speed */
					incr = atoi(GET_OPT(i));
					break;
				case 'b':		/* bottom line */
					bottom = atoi(GET_OPT(i));
					break;
				case 't':		/* top line */
					top = atoi(GET_OPT(i));
					break;
				case 'l':		/* left line */
					left = atoi(GET_OPT(i));
					break;
				case 'r':		/* right line */
					right = atoi(GET_OPT(i));
					break;
				case 'S':		/* begin at line */
					begin = atoi(GET_OPT(i));
					break;
				case 'm':		/* set drawing mode 0=normal */
					mode = 1-mode;
					break;
				case 'i':		/* set idle timer */
					idle = atoi(GET_OPT(i));
					break;
				case 'g':		/*et intra-letter gap */
					gap = atoi(GET_OPT(i));
					break;
				case 'd':		/* fetch the drop shadows */
					drop = atoi(GET_OPT(i));
					gap = drop;
					break;
				case 'E':		/* end at line */
					end = atoi(GET_OPT(i));
					break;
				case 'z':		/* make a log */
					no_initial = 1;
   			  	start_log(stdout); 
					SEND_TIME();
					break;
				default:
					fprintf(stderr,"Bad flag, try s, b, ot t\n");
					break;
				}
			}
		}

	/* read in the items */

	if (bottom<= top)
		bottom = BIT_HIGH(display);

	if (right <= left)
		right = BIT_WIDE(display);

	screen = bit_create(display,left,top,right-left,bottom-top);

	/* default margins */

	lm=0;
	rm=0;

	while(gets(line)) {
		if (*line == '#' || strlen(line)<2)		/* a comment */
			continue;

		if (strncmp(line,"margin",strlen("margin"))==0) {
			sscanf(line,"%*s %d %d\n",&lm,&rm);
			continue;
			}

		get_item(line,incr);
		}

	/* remove the fonts */

	for(i=0;i<max_font;i++) {
		bit_destroy(fonts[i].image);	
		free(fonts[i].name);
		}
	max_font = 0;

	/* display the items */

	save = bit_alloc(BIT_WIDE(screen),BIT_HIGH(screen),0,1);
	bit_blit(save,0,0,BIT_WIDE(save),BIT_HIGH(save),BIT_SRC,screen,0,0);
	min_offset = end-(items[max_item-1].y + BIT_HIGH(items[max_item-1].image));
	
	for(j=0,offset=BIT_HIGH(screen)-incr-begin;offset>min_offset;offset-=incr) {
		for(item=items,i=0;i<max_item;i++,item++)
			setup_item(save,item,offset);
		for(item=items,i=0;i<max_item;i++,item++)
			blast_item(screen,item,offset);
		if ((j++ % stamp) == 0)
			SEND_TIME();
		if (idle>0)
			usleep(idle*1000);
		}
	SEND_TIME();
	if (wait)
		sleep(wait);
	SEND_TIME();
   end_log(); 
	}

/* read in an item */

int
get_item(line,incr)
char *line;			/* line read from script file */
int incr;			/* scroll increment */
	{
	int i=0;
	int w=0;
	int spacing;
	char name[16];
	char adjust[16];
	char text[128];
	int len;				/* text length */
	register struct item *item = &items[max_item];
	struct font *font;
	register char *textp = text, c;

	if (max_item >= MAX_ITEMS) {
		fprintf(stderr, "Too many items\n");
		exit(1);
		}

	sscanf(line,"%s %s %d %[^\n]", name, adjust,&spacing, text);

	item->text = save_line(text);
	item->font = find_font(name);
	item->gap = gap;
	font = &fonts[item->font];
	len = text_len(item) + gap*(strlen(text)-1);
	item->image = bit_alloc(len, BIT_HIGH(font->image),NULL,1);
	item->save = bit_alloc(2*drop+BIT_WIDE(item->image),
					2*drop+incr+BIT_HIGH(item->image),NULL,1);
	item->y = spacing*BIT_HIGH(item->image)/100;
	item->mode=mode;
	if (max_item>0) 
		item->y += BIT_HIGH(items[max_item-1].image) +
						items[max_item-1].y;

	if (item->image == NULL) {
		fprintf(stderr,"Cant a bit_alloc image for %s\n",text);
		exit(1);
		}

	switch (*adjust) {
		case 'l': case 'L':					/* left */
			item->x = lm;
			break;
		case 'r': case 'R':					/* right */
			item->x = BIT_WIDE(screen)-rm-BIT_WIDE(item->image);
			break;
		case '-': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			item->x = atoi(adjust);
			break;
		default:							/* center it */
			item->x = (BIT_WIDE(screen)-BIT_WIDE(item->image))/2;
			break;
		}

	dprintf(stderr,"Got item [%s] %d,%d %d x %d\n",
			text,item->x,item->y,BIT_WIDE(item->image),BIT_HIGH(item->image));

	/* fill in the image */

	bit_blit(item->image,0,0,BIT_WIDE(item->image),BIT_HIGH(item->image),
					BIT_CLR,0,0,0);
	for(i=0;c = *textp++; i+=w+item->gap) {
		w=get_width(c,font);
		if (c != ' ')
			bit_blit(item->image,i,0,w, BIT_HIGH(item->image),
						 BIT_DST|BIT_SRC,font->image,get_origin(c,font),0);
		else
			bit_blit(item->image,i,0,w, BIT_HIGH(item->image),
						 BIT_CLR,NULL,0,0);
		}

	/* do the drop shadows */

	if (drop) {
		register int i,j;

		item->mask = bit_alloc(2*drop+BIT_WIDE(item->image),
					2*drop+BIT_HIGH(item->image),NULL,1);
		bit_blit(item->mask,0,0,BIT_WIDE(item->mask),BIT_HIGH(item->mask),
						BIT_CLR,0,0,0);

		for(i=0;i<=2*drop;i++)
			for(j=0;j<=2*drop;j++) {
				if (i>0 && j>0 && i<2*drop && j<2*drop)
					continue;			/* skip middle regions */
				dprintf(stderr,"shadow at %d,%d\n",i,j);
				bit_blit(item->mask,i,j,BIT_WIDE(item->image),
							BIT_HIGH(item->image),BIT_SRC|BIT_DST,item->image,0,0);
				}
		}
	max_item++;
	}

/* find the length of the text */

int
text_len(item)
struct item *item;
	{
	register int len=0;
	register char *ch = item->text;
	register struct font *font = &fonts[item->font];
	register char c;

	while(c = *ch++) {
		len += get_width(c,font);
		}
	dprintf(stderr,"len(%s) of %s is %d\n",
		item->text,font->name,len);
	return(len);
	}

/* find the width of a character */

int
get_width(c,font)
char c;				/* char to find width of */
struct font *font;	/* which font the character is in */
	{
	int wth;

	if (c == ' ')		/* use 'n' spaces */
		c = 'n';
	else if (c<' ')
		return(0);
	else if (c>'~')
		return(0);

	wth = font->widths[c-FIRST+1] - font->widths[c-FIRST];
	return(wth);
	}

/* find the origin of a character in the bitmap*/

int
get_origin(c,font)
char c;				/* char to find width of */
struct font *font;	/* which font the character is in */
	{
	int origin;

	if (c == ' ')		/* use 'n' spaces */
		c = 'n';
	else if (c<=FIRST || c>'~')
		return(0);

	origin = font->widths[c-FIRST];
	return(origin);
	}

/* malloc space for and save a line */

char *
save_line(s)
char *s;
	{	
	char *malloc(), *strcpy();

	return(strcpy(malloc(strlen(s)+1),s));
	}

/* find a font in the font table */

int
find_font(name)
char *name;
	{
	register int i;

	for(i=0;i<max_font;i++)  {
		if (strcmp(name,fonts[i].name)==0)
			return(i);
		}
	return(get_font(name));
	}

/* read in a font structure */

int
get_font(name)
char *name;
	{
	FILE *font_file;
	FILE *width_file;		/* font width info goes here */
	struct font *font = &fonts[max_font];
	register int i;
	int w;
	int sum=0;

	if (max_font >= MAX_FONTS) {
		fprintf(stderr,"cant open %s, To many fonts\n",name);
		exit(1);
		}

	sprintf(line,"%s/%s.%s",font_dir,name,FONT_SUFFIX);
	if ((font_file=fopen(line,"r"))==NULL) {
		fprintf(stderr,"Cant open %s\n",line);
		exit(1);
		}

	sprintf(line,"%s/%s.%s",FONT_DIR,name,WIDTH_SUFFIX);
	if ((width_file=fopen(line,"r"))==NULL) {
		fprintf(stderr,"Cant open %s\n",line);
		exit(1);
		}
	
	font->name = save_line(name);
	font->image = bitmapread(font_file);

	if (font->image == NULL) {
		fprintf(stderr,"Cant read in font image for %s\n",name);
		exit(1);
		}

	dprintf(stderr,"Getting font %s (%dx%d)\n",
			name,BIT_WIDE(font->image),BIT_HIGH(font->image));

	fscanf(width_file,"%*s %d\n",&sum);
	for(i=0;i<MAX_CHARS;i++) {
		fscanf(width_file,"%*s %d\n",&w);
		font->widths[i] = sum;
		sum += w;
		}
	

	fclose(width_file);
	fclose(font_file);
	return(max_font++);
	}

/* prepare an item to be displayed */

setup_item(src,item,offset)
BITMAP *src;							/* source for background */
register struct item *item;		/* item to prepare */
int offset;								/* offset */
	{
	int drop_op =  item->mode ? BIT_DST|BIT_SRC :
							BIT_DST&BIT_NOT(BIT_SRC);
	int op =       item->mode ? BIT_DST & BIT_NOT(BIT_SRC) :
							BIT_SRC|BIT_DST;
	

	bit_blit(item->save,0,0,BIT_WIDE(item->save),BIT_HIGH(item->save),
				BIT_SRC,src,item->x-drop,item->y+offset-drop);
	if (drop)
		bit_blit(item->save,0,0,BIT_WIDE(item->mask),BIT_HIGH(item->mask),
					drop_op,item->mask,0,0);
	bit_blit(item->save,drop,drop,BIT_WIDE(item->image),BIT_HIGH(item->image),
				op,item->image,0,0);
	}

/* blast a prepared item on the display */

blast_item(dst,item,offset)
BITMAP *dst;							/* destination for image */
register struct item *item;		/* item to prepare */
int offset;								/* offset */
	{
	bit_blit(dst,item->x-drop,item->y+offset-drop,
				BIT_WIDE(item->save),BIT_HIGH(item->save),BIT_SRC,item->save,0,0);
	}
