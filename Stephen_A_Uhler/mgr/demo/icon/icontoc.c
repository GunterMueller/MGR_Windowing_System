/* convert an mgr image into a c file */

#include <stdio.h>
#include "bitmap.h"
#include "dump.h"

main(argc,argv)
int argc;
char **argv;
	{
	BITMAP *bitmapread();		/* image to convert */
	BITMAP *image;		/* image to convert */
	unsigned char *data;			/* pointer to image data */
	FILE *file;			/* pointer to file */
	int bytes;			/* file size (in bytes) */
	int i;

	if (argc<3) {
		fprintf(stderr,"usage: %s <image_file> <identifier>\n",*argv);
		exit(1);
		}

	file = strcmp(argv[1],"-")==0 ? stdin : fopen(argv[1],"r");

	if (!file) {
		fprintf(stderr,"can't open %s\n",argv[1]);
		exit(2);
		}

	image = bitmapread(file);
	if (!image) {
		fprintf(stderr,"stdin was not a valid icon\n");
		exit(3);
		}

	data = (unsigned char*) BIT_DATA(image);
	bytes=BIT_SIZE(image);

	/* write out the header */

	printf("/* static bitmap %s created by %s */\n\n",
			strcmp(argv[1],"-")==0 ? "standard-input" : argv[1],argv[0]);

	printf("bit_static(%s,%d,%d,%s_data,%d);\n",
			argv[2],BIT_WIDE(image),BIT_HIGH(image),argv[2],BIT_DEPTH(image));
		
	printf("unsigned char %s_data[] = {\n\t",
			argv[2]);	

	for(i=1;i<=bytes;i++) {
		printf("0x%02x,%s",*data++, i%10 ? " ":"\n\t");
		}
	printf("\n\t};\n");

	exit(0);
	}
