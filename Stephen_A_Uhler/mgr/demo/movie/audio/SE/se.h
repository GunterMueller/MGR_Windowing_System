/* defines and structure defs for se */

#define dprintf if (debug)fprintf
#define MAX			4096		/* maximum sound value */
#define MAX_SEG	25			/* max # of pieces */
#define THICK		2			/* internal border thickness */
#define BAR			20			/* scroll bar thickness */
#define AUDIO		"/dev/audio"
#define TIMEOUT	5			/* seconds to wait after last play */

/* Mu law defines */

#define	B13ZERO	0x1000		/* zero offset in unsigned 13-bit tables */
#define	B13RNG	0x1000		/* range from min to zero */
#define	B13MAX	0x1FFF		/* largest offset in unsigned 13-bit tables */
#define	B13LIMIT(a)	(((a)<0)?0:(((a)>B13MAX)?B13MAX:(a)))
#define	SB13LIMIT(a)	(((a)<-B13RNG)?-B13RNG:(((a)>=B13RNG)?B13RNG-1:(a)))

/* short hand */

#define E(x)		edit->x
#define G(x)		global->x
#define S(x)		seg->x
#define CNVT(x)	(x)/8000,((x)/80)%100

#define Max(a,b)		((a) > (b) ? (a) : (b))
#define Min(a,b)		((a) < (b) ? (a) : (b))
#define BETWEEN(a,x,b)      (x)<(a)?a:((x)>(b)?b:x)

/* generic buffers (global to save space??) */

extern char line[512];		/* mgr input line */
extern char s_buff[80];		/* sprintf buffer */

/* global procedures */

char *malloc();
int blast();
int cut_it();
int do_cut();
int fetch_it();
int file_it();
int merge();
int play();
int step();
int timer();
char *get_str();
struct segment *append_seg();
struct edit *copy_edit();

/* structures */

struct global {					/* global display info */
	int w,h;							/* window size */
	int fw,fh;						/* font size */
	int thick;						/* spacer size */
	int bar;							/* bar size */
	int d1,d2,d3;						/* extent of data area (derived) */
	};

struct segment {					/* a sound segment */
	struct segment *next;		/* next segment (or NULL) */
	unsigned char *data;			/* pointer to sound data */
	int count;						/* # of samples in this segment */
	int num;							/* segment # for debugging */
	};

struct edit {						/* defines an edit script */
	char *name;						/* name of this edit */
	int chunk;						/* displayed chunk size */
	int left;						/* left most sample displayed */
	int size;						/* total number of samples */
	int num;							/* # of segments */
	int start;						/* starting mark */
	int end;							/* ending mark */
	struct segment *seg;			/* list of segments */
	};

struct undo {						/* list of previous edits */
	struct undo *next;			/* the next one */
	struct edit *edit;			/* the  edit */
	};

struct cut {
	int count;							/* number of samples */
	unsigned char *data;				/* data */
	};

/* globals */

extern int fade_limit;		/* limit to fade (temporary) */
extern int debug;				/* debugging flag */
extern int audio_fd;			/* audio fd (global for int handling routine) */
extern int seg_num;
extern short I2sb[];			/*	 mu-law to signed 13 bit binary */
extern unsigned char  *Sb2i; /* signed 13 bit binary to mu-law */
extern struct global *global;
