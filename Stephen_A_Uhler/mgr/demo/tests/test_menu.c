/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: test_menu.c,v 4.1 88/06/21 14:02:08 bianchi Exp $
	$Source: /tmp/mgrsrc/demo/tests/RCS/test_menu.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/tests/RCS/test_menu.c,v $$Revision: 4.1 $";


/* test out menus */

#include <stdio.h>
#include "term.h"
#include <signal.h>

#define TERM		"mgr"			/* name of valid terminal id */
#define MAX		3			/* # of menus */

char foo[25];

struct menu_entry menu1[] = {
	"menu 1","1",
	foo,"g",
	"cat","c",
	"mouse","m",
	"elephant","e",
	};

struct menu_entry menu2[] = {
	"menu 2","1",
	foo,"g",
	"slate","s",
	"sand","m",
	"quartz","q",
	};

struct menu_entry menu3[] = {
	"menu 3","3",
	foo,"g",
	"carrot","c",
	"egg plant","e",
	"string beans","q",
	};

struct menus {
   struct menu_entry *menu;
   int count;
   };

struct menus menus[] = {
   menu1, 5,
   menu2, 5,
   menu3, 5,
   (struct menu_entry *) 0, 0
   };


main(argc,argv)
int argc;
char **argv;
   {
   char *getenv();
   char *term = getenv("TERM");
   char line[80];			/* event input buffer */
   register char c;
   register int i, n;
   int x,y;
   int clean();
   
   /* make sure environment is ok */

   if (term!=NULL && strcmp(term,TERM)!=0) {
      fprintf(stderr,"%s only runs on %s terminals\n",argv[0],TERM);
      exit(1);
      }

   signal(SIGINT,clean);
   signal(SIGTERM,clean);

   m_setup(M_FLUSH);
   m_ttyset();
   m_push(P_MENU|P_EVENT);

   m_nomenu();
   m_setevent(BUTTON_2,"[%p]");
   m_setevent(BUTTON_2U,"$");
   m_setraw();

   fprintf(stderr,"Use the middle button to activate a menu\r\n");
   while ((c=getc(m_termin)) != 'q') {
     switch(c) {
        case '[':				/* button down */
           fscanf(m_termin,"%d %d]",&x,&y);
           fprintf(stderr,"got %d %d selecting %d\t",x,y,x*MAX/1000+1);
           n = x * MAX/1000;
           sprintf(foo,"at %d,%d\n",x,y);
           menu_load(n+1,menus[n].count,menus[n].menu);
           m_selectmenu(n+1);
           break;
        case '$':				/* button up */
           fprintf(stderr,"done\r\n");
           m_nomenu();
           break;
        default:				/* menu selection */
           fprintf(stderr,"got %c\t",c);
           break;
        }
     }
   clean(0);
   }

/* clean up and exit */

clean(n)
int n;
   {
   m_pop();
   m_ttyreset();
   exit(n);
   }
