/* mix 2 audio channels together (brute force version) */

#include <stdio.h>

#define dprintf if (debug)fprintf

#define DIV		256			/* scale factor */

int debug = 0;

/*		conversion tables: */

extern unsigned char  *Sb2i;	/*  signed 13 bit binary to mu-law */
extern short I2sb[];				/*	 mu-law to signed 13 bit binary */

main(argc,argv)
int argc;
char **argv;
	{
	FILE *f1, *f2;		/* audio files to mix */
	char *name1, *name2;
	char *me = argv[0];

	int master = DIV;			/* total gain (out of DIV) */
	int a = 6*DIV/10;				/* fraction of 'a' channel */
	int b = 4*DIV/10;				/* fraction of 'b' channel */
	int slip = 0;					/* slip the first chan */
	int flush = 0;					/* flush faster for use in a pipe */
	register int c1,c2;					/* current input byte */

	int count=0, clipped=0;		/* stats for debugging */

	/* parse -X arguments */

	while(argc>1 && *argv[1] == '-' && strlen(argv[1])>1) {
		switch(argv[1][1]) {
			case 'f':	/* flush output after nnn bytes */
				flush = 1<<(atoi(argv[1]+2)) - 1;
				dprintf(stderr,"Setting flush to %d bytes\n",master);
				break;
			case 's':	/* slip file 'a' by nnn bytes */
				slip = atoi(argv[1]+2);
				dprintf(stderr,"Setting slip to %d/8000 seconds\n",master);
				break;
			case 'm':	/* master volume (0-256) */
				master = atoi(argv[1]+2);
				dprintf(stderr,"Setting master volume to %d/256\n",master);
				break;
			case 'a':	/* chan a volume (0-256) */
				a = atoi(argv[1]+2);
				dprintf(stderr,"Setting channel A volume to %d/256\n",a);
				break;
			case 'b':	/* chan b volume (0-256) */
				b = atoi(argv[1]+2);
				dprintf(stderr,"Setting channel B volume to %d/256\n",b);
				break;
			case 'd':	/* turn on debuggong */
				debug = 1;
				dprintf(stderr,"Setting debugging on\n");
				break;
			}
		argv++;
		argc--;
		}

	if (argc<3) {
		fprintf(stderr,"usage: %s [options] <file1> <file2>\n",*argv);
		fprintf(stderr,"	-annn	chan a volume (0-%d)\n",DIV);
		fprintf(stderr,"	-bnnn	chan b volume (0-%d)\n",DIV);
		fprintf(stderr,"	-mnnn	master volume (0-%d)\n",DIV);
		fprintf(stderr,"	-fnnn	flush after 2^n-1 bytes\n");
		fprintf(stderr,"	-d	turn on debug\n");
		exit(1);
		}

	/* get files */

	name1 = argv[1];
	name2 = argv[2];

/*
	if (strcmp(name1,name2)== 0) {
		fprintf(stderr,"Must have 2 different files\n");
		exit(1);
		}
*/

	f1 = strcmp(name1,"-") ? fopen(name1,"r") : stdin;
	f2 = strcmp(name2,"-") ? fopen(name2,"r") : stdin;

	if (f1==NULL) {
		fprintf(stderr,"Can't open %s\n",name1);
		exit(1);
		}
	
	if (f2==NULL) {
		fprintf(stderr,"Can't open %s\n",name2);
		exit(1);
		}

	dprintf(stderr,"Name1=%s, name2=%s\n",name1,name2);

	/* slip file 1 */

	while(slip-- > 0)
		getc(f1);

	/* combine inputs */
	
	while(1) {
		c1 = getc(f1);
		c2 = getc(f2);
		if(feof(f1) || feof(f2))
			break;
		c1 = (I2sb[c1]*a) / DIV;
		c2 = (I2sb[c2]*b) / DIV;
		c1 = (master*(c1+c2)) / DIV;
		if (c1>4095) c1 = 4095,clipped++;
		if (c1< -4095) c1 = -4095,clipped++;
		putchar(Sb2i[c1]);
		count++;
		if (flush && count&flush==0)
			fflush(stdout);
		}

	dprintf(stderr,"Combined %d chars (%d clipped)\n",count,clipped);
	count=clipped=0;

	/* handle left over data */

	while(!feof(f1)) {
		c1 = (I2sb[c1]*a) / DIV;
		c1 = (master*c1) / DIV;
		if (c1>4095) c1 = 4095;
		if (c1< -4095) c1 = -4095;
		putchar(Sb2i[c1]);
		count++;
		if (flush && count&flush==0)
			fflush(stdout);
		c1 = getc(f1);
		}

	while(!feof(f2)) {
		c1 = (I2sb[c2]*b) / DIV;
		c1 = (master*c1) / DIV;
		if (c1>4095) c1 = 4095;
		if (c1< -4095) c1 = -4095;
		putchar(Sb2i[c1]);
		clipped++;
		if (flush && clipped&flush==0)
			fflush(stdout);
		c2 = getc(f2);
		}

	if (count>0)
		dprintf(stderr,"%d extra chars from %s\n",count,name1);
	else if (clipped>0)
		dprintf(stderr,"%d extra chars from %s\n",clipped,name2);

	exit(0);
	}
