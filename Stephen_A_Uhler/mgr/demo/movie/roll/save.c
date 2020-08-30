/* save the image for later restoring */

#include <stdio.h>
#include "bitmap.h"


#define SCREEN "/dev/bwtwo0"

main()
	{
	BITMAP *screen = bit_open(SCREEN);
	BITMAP *data;

	data = bit_alloc(BIT_WIDE(screen),BIT_HIGH(screen),0,1);
	bit_blit(data,0,0,BIT_WIDE(data),BIT_HIGH(data),BIT_SRC,screen,0,0);
	bitmapwrite(stdout,data,0);
	}
