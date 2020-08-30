/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: test_rop.c,v 4.1 88/06/21 14:02:09 bianchi Exp $
	$Source: /tmp/mgrsrc/demo/tests/RCS/test_rop.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/tests/RCS/test_rop.c,v $$Revision: 4.1 $";


/* test raster op functions */

#include <signal.h>
#include "term.h"

char *op_names[] = {
   "0", 
   "~(destination | source)",
   "destination & ~source",
   "~source",
   "~destination & source",
   "~destination",
   "destination ^ source",
   "~(destination & source)",
   "destination & source",
   "~(destination ^ source)",
   "destination",
   "destination | ~source",
   "source",
   "~destination | source",
   "destination | source",
   "~0"	
   };
   
char buff[100];

main(argc,argv)
int argc;
char **argv;
   {
   register int i;
   int clean();
   int f_high;

   m_setup(M_FLUSH);
   m_ttyset();
   get_font(0,&f_high);

   signal(SIGINT,clean);
   signal(SIGTERM,clean);

   m_clear();

   m_moveprint( 70, 190, "destination" );
   m_moveprint( 70 + 333, 190, "source" );
   m_func(B_SET);
   for(i=0;i<3;i++)
      m_bitwrite(70+333*i,200,200,600);
   m_func(B_CLEAR);
   for(i=0;i<2;i++)
      m_bitwrite(75+333*i,205,190,590);
   m_func(B_SET);
   m_bitwrite(75+333*0,205,85,590);
   m_bitwrite(75+333*1,205,190,295);

   for(i=0;i<16;i++) {
      m_func(B_SET);
      m_moveprint(10,f_high*2,"destination ...");
      m_cleareol();
      m_moveprint( 70 + 333*2, 190, "destination" );
      m_cleareol();
      m_func(B_CLEAR);
      m_bitwrite(75+333*2,205,190,590);
      m_func(B_SET);
      m_bitwrite(75+333*2,205,85,590);
      m_func(i);
      sleep(1);
      m_bitcopy(75+333*2,205,190,590,75+333*1,205);
      sprintf(buff,"function %d: %s",i,op_names[i]);
      m_moveprint(10,f_high*2,buff);
      m_cleareol();
      sprintf( buff, "function %d", i );
      m_moveprint( 70 + 333*2, 190, buff );
      m_cleareol();
      m_cleareol();
      m_gets(buff);
      }
   clean(0);
   }

/* restore window state and exit */

clean(n)
int n;
   {
   m_ttyreset();
   m_popall();
   exit(n);
   }
