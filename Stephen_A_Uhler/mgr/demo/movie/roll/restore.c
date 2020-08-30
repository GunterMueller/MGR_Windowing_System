/* restore the image previously saveds */

#include <stdio.h>
#include "bitmap.h"

#define SCREEN "/dev/bwtwo0"
#define GET_OPT(i)   \
   strlen(argv[i])>2 ? argv[i]+2 : argv[++i]

main(argc,argv)
int argc;
char **argv;
	{
	register int i;
	char *getenv();
	char *sname = getenv("SCREEN") ? getenv("SCREEN") : SCREEN;
	BITMAP *screen = bit_open(sname);
	BITMAP *data;
	BITMAP *save;
	BITMAP *bitmapread();
	int op = BIT_SRC;				/* bitblit function */
	FILE *in = stdin;						/* input file */
	int restore = -1;				/* restore after n seconds */
	int share=0;
	int end = 0;					/* addional wait at end */

	/* get arguments */

	for(i=1;i<argc;i++) {
		if (*argv[i] == '-') {
			switch(argv[i][1]) {
				case 'o':		/* Bitmap function */
					op = 0xf&atoi(GET_OPT(i));
					break;
				case 'e':		/* addirionaoakakaka wait at end */
					end = atoi(GET_OPT(i));
					break;
				case 'r':		/* restore after n seconds */
					restore = atoi(GET_OPT(i));
					break;
				case 'z':		/* make a log */
					share++;
					break;
				default:
					fprintf(stderr,"Bad flag, try again\n");
					break;
				}
			}
		else
			in = fopen(argv[i],"r");
		}

	if (!screen) {
		fprintf(stderr, "couldn't open %s\n",sname);
		exit(1);
		}

	data = bitmapread(in);
	save = bit_alloc(BIT_WIDE(screen),BIT_HIGH(screen),0,1);
	bit_blit(save,0,0,BIT_WIDE(screen),BIT_HIGH(screen),BIT_SRC,screen,0,0);

	if (share) {
		start_log(stdout); 
		SEND_TIME();
		}
	bit_blit(screen,0,0,BIT_WIDE(data),BIT_HIGH(data),op,data,0,0);
	if (restore >=0) {	
		SEND_TIME();
		sleep(restore);
		SEND_TIME();
		bit_blit(screen,0,0,BIT_WIDE(data),BIT_HIGH(data),BIT_SRC,save,0,0);
		SEND_TIME();
		}
	if (end)
		sleep(end);
	SEND_TIME();
   end_log(); 
	}
