/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: half.c,v 4.1 88/06/21 14:02:04 bianchi Exp $
	$Source: /tmp/mgrsrc/demo/tests/RCS/half.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/demo/tests/RCS/half.c,v $$Revision: 4.1 $";

/* Start a new window with a command running in it */

#include <stdio.h>
#include <sgtty.h>

#define WIDE	400
#define HIGH	300
#define X	200
#define Y	200

main(argc,argv)
int argc;
char **argv;
   {
   register int i;
   int group;		/* process group for new shell */
   int pid;		/* pid of new shell */
   char name[50];	/* name of tty to open */
   struct sgttyb modes;	/* for tty modes */
 
   /* check arguments */

   if (argc < 2) {
      fprintf(stderr,"usage: %s <command> \n",*argv);
      exit(1); 
      }

   /* get existing file modes, turn off echo */

   ioctl(0,TIOCGETP,&modes);
   modes.sg_flags &= ~ECHO;
   ioctl(0,TIOCSETP,&modes);
      
   /* make the window */

   printf("\033%d,%d,%d,%d%c",X,Y,WIDE,HIGH,'z');
   fflush(stdout);

   /* get tty name to open */

   gets(name);

   /* restore modes */

   modes.sg_flags |= ECHO;
   ioctl(0,TIOCSETP,&modes);

   if (strlen(name) < 4) {
      printf("%s: Sorry, couldn't create window\n",*argv);
      exit(1);
      }

   /* make sure we can open tty */

   if (open(name,2) < 0) {
      printf("Serious error, can't open %s\n",name);
      exit(1);
      }

   /* start new process */

   if ((pid=fork()) > 0) {	/* parent */
      printf("starting %s as %d on %s\n",argv[1],pid,name);
      exit(0);
      }

   else if (pid == 0) {		/* new shell (child) */

      /* remove controlling tty */

      ioctl(open("/dev/tty",0),TIOCNOTTY,0);

      /* close all files */

      for(i=0;i<getdtablesize();i++)
         close(i);

      /* open new ones */

      open(name,0);	/* stdin */
      open(name,1);	/* stdout */
      open(name,1);	/* stderr */
   
      /* get our own process group */
      
      group = getpid();
      setpgrp(group,group);
      ioctl(0,TIOCSPGRP,&group);      

      /* fix the tty modes */

      modes.sg_flags = ECHO|CRMOD|EVENP|ODDP;
      ioctl(0,TIOCSETP,&modes);

      /* start the command */

      execvp(argv[1],argv+1);
      printf("%s: Can't find %s\n",argv[0],argv[1]);
      sleep(2);
      exit(1);
      }
   else {
      printf("Can't fork!!\n");
      }
   }
