/*                        Copyright (c) 1988 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	get_screen		(S A Uhler)
 *
 *	Capture an image of the SUN screen
 */
/*	$Header: getscreen.c,v 1.2 88/07/08 08:25:28 sau Exp $
	$Source: /tmp/mgrsrc/misc/RCS/getscreen.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/misc/RCS/getscreen.c,v $$Revision: 1.2 $";

#include <sys/ioctl.h>
#include <sun/fbio.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <stdio.h>
#include "dump.h"

#define SCREEN	"/dev/bwtwo0"	/* name of sun frame buffer */
#define BUFF	512		/* size of write buffer */
#define BAR	  3		/* width of copy bar */

main()
   {
   int fd;
   char  *malloc();
   register unsigned char *addr;
   struct fbtype buff;
   int temp,pagesize;
   register int i, wide;
   struct b_header b_buff, *head = &b_buff;	/* for bitmap header */

   /* open the SUN screen */

   if ((fd = open(SCREEN,O_RDWR)) <0) {
      fprintf(stderr,"Can' open %s\n",SCREEN);
      exit(1);
      }

   /* get the frame buffer size */

   ioctl(fd,FBIOGTYPE,&buff);
   pagesize = getpagesize();
   buff.fb_size = (buff.fb_size+pagesize-1) &~ (pagesize-1);

   /* malloc space for frame buffer */

   if ((temp = (int) malloc(buff.fb_size+pagesize)) == 0) {
      fprintf(stderr,"couldn't malloc %d bytes\n",buff.fb_size+pagesize);
      exit(1);
      }

   /* align space on a page boundary */

   addr = (unsigned char *)((temp+pagesize-1) & ~(pagesize-1));

   /* map the frame buffer into malloc'd space */

#ifdef _MAP_NEW
   addr = mmap(addr,buff.fb_size,PROT_WRITE|PROT_READ,_MAP_NEW|MAP_SHARED,fd,0);
#else
   mmap(addr,buff.fb_size,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
#endif
  
   /* write header to stdout */

   B_PUTOLDHDR(head,buff.fb_width,buff.fb_height);
   write(1,head,B_HSIZE);
   
   /* write the screen to stdout */

   wide = buff.fb_width/8;
   for(i=0;i<buff.fb_height;i++) {
      if (i>BAR) invert(addr-BAR*wide,wide);
      write(1,addr,wide);
      invert(addr,wide);
      addr += wide;
      }
   invert(addr-BAR*wide,BAR*wide);

   /* clean up stuff */

   munmap(addr,buff.fb_size);
   free(temp);
   }
   
invert(addr,count)
register char *addr;
int count;
   {
   register char *max = addr + count;
   while(addr<max) *addr++ = ~*addr;
   }
