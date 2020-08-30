/* assemble bitmaps */

#include <stdio.h>
#include "bitmap.h"

char line[1024];
char name[512];

main(argc,argv)
int argc;
char **argv;
	{
	register int i;
	char *getenv();
	BITMAP *bitmapread();
	BITMAP *item;							/* current image item */
	BITMAP *whole;							/* entire resultant image */
	FILE *in;								/* input file */
	int count;								/* # items in spec file */
	int x,y,w,h;							/* item size */

	int wide,high;							/* overall size */

	if (argc<3) {
		fprintf(stderr,"usage: %s <wide> <high>  < <spec_file>\n",*argv);
		fprintf(stderr,"  spec_file:   <name> <x> <y>  [w] [h]\n");
		exit(1);
		}

	wide = atoi(argv[1]);
	high = atoi(argv[2]);

	
	whole = bit_alloc(wide,high,NULL,1);	
	bit_blit(whole,0,0,wide,high,BIT_CLR,NULL,0,0);

	while(gets(line)) {
		count = sscanf(line,"%s %d %d %d %d\n",name,&x,&y,&w,&h);
		if (count < 3 || *line == '#') {
			fprintf(stderr, "Skipping [%s]\n",line);
			continue;
			}
			
		if ((in = fopen(name,"r")) == NULL) {
			fprintf(stderr,"Cant open %s\n",name);
			continue;
			}

		if ((item = bitmapread(in))==NULL) {
			fprintf(stderr,"Cant read %s\n",name);
			fclose(in);
			continue;
			}

		if (count < 4)
			w = BIT_WIDE(item);
		if (count < 5)
			h = BIT_HIGH(item);

		bit_blit(whole,x,y,w,h,BIT_SRC,item,0,0);
		bit_destroy(item);
		}

	bitmapwrite(stdout,whole,1);
	exit(0);
	}
