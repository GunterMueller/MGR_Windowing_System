/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: test_text.c,v 4.1 88/06/21 14:02:11 bianchi Exp $
	$Source: /tmp/mgrsrc/demo/tests/RCS/test_text.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/tests/RCS/test_text.c,v $$Revision: 4.1 $";

/* test vector drawn text routines */

#include "term.h"

#define SIZE	500
#define FONTS	5

main(argc,argv)
char **argv;
   {
   register int font;
   register int angle;

   char buff[256];

   if (argc<2) {
      fprintf(stderr,"usage: %s <text>\n",*argv);
      exit(1);   
      }

   if (*argv[1] != ' ') {
      strcpy(buff,"  ");
      strcat(buff,argv[1]);
      }
   else
      strcpy(buff,argv[1]);

   m_setup(M_DEBUG);
   m_func(B_SET);
   for(font=0;font<FONTS;font++) {
      m_clear();
      for(angle = 0;angle<360;angle +=30)
         text(buff,500,500,font,angle,SIZE/strlen(buff),SIZE/strlen(buff));
      m_flush();
      sleep(3);
      }
   exit(0);
   }
