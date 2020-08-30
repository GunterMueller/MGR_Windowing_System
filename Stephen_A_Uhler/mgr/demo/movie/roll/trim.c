/* trim a bitmap */

#include <stdio.h>
#include "bitmap.h"

#define GET_OPT(i)   \
   strlen(argv[i])>2 ? argv[i]+2 : argv[++i]

main(argc,argv)
int argc;
char **argv;
	{
	register int i;
	char *getenv();
	BITMAP *old;
	BITMAP *new;
	BITMAP *bitmapread();
	FILE *in = NULL;						/* input file */
	int gotfile=0;

	int l=0;		/* left margin */
	int t=0;		/* top margin */
	int w = 0;		/* width */
	int h = 0;		/* height */
	int r = 0;		/* right margin */
	int b = 0;		/* bottom margin */

	
	/* get arguments */

	for(i=1;i<argc;i++) {
		if (*argv[i] == '-') {
			switch(argv[i][1]) {
				case 'y':
				case 't':
					t = atoi(GET_OPT(i));
					break;
				case 'x':
				case 'l':
					l = atoi(GET_OPT(i));
					break;
				case 'h':
					h = atoi(GET_OPT(i));
					break;
				case 'r':
					r = atoi(GET_OPT(i));
					break;
				case 'w':
					w = atoi(GET_OPT(i));
					break;
				case 'b':
					b = atoi(GET_OPT(i));
					break;
				case '\000':		/* just a '-' */
					in = stdin;
					gotfile++;
					break;
				default:
					fprintf(stderr,"Bad flag, try again\n");
					break;
				}
			}
		else {
			in = fopen(argv[i],"r");
			gotfile++;
			}
		}

	if (!gotfile) {
		fprintf(stderr,"Usage: %s [options] <file> > new_bitmap\n",*argv);
		fprintf(stderr,"  -l(eft) nn  -r(ight) nn\n");
		fprintf(stderr,"  -t(op) nn  -b(ottom) nn\n");
		fprintf(stderr,"  -w(idth) nn  -h(eight) nn\n");
		exit(1);
		}

	if ((old = bitmapread(in))==NULL) {
		fprintf(stderr,"cant read bitmap file\n");
		exit(1);
		}

	if (w==0)
		w=BIT_WIDE(old)-r;
	if (h==0)
		h=BIT_HIGH(old)-b;

	new = bit_alloc(w,h,0,1);
	bit_blit(new,0,0,BIT_WIDE(old),BIT_HIGH(old),BIT_SRC,old,l,t);
	bitmapwrite(stdout,new,1);
	exit(1);
	}
