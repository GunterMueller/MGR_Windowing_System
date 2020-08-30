/* redirect sun console output to this window */

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sundev/kbio.h>

main(argc,argv)
int argc;
char **argv;
	{
	int fd;
	int kbd;
	int one = 1;
	int mode = 0;
   char *name, *ttyname();

	/* temporary */

	if (*argv[1] == 'f') {		/* reset console */
		fd = open("/dev/console");
      ioctl(fd,TIOCCONS,&one);
		fprintf(stderr,"Forcing console reset\n");
		exit(0);
		}

	/* make sure kbd is in direct mode */

	if ((kbd = open("/dev/kbd",0)) < 0 ) {
		perror("open");
		exit(1);
		}
	if (ioctl(kbd,KIOCGDIRECT,&mode) < 0 ) {
		perror("kbd ioctl");
		exit(1);
		}

	if (mode != 1) {
		fprintf(stderr,"Keyboard not in direct mode\n");
		exit(1);
		}

	if (**argv == 'r') 		/* reset console */
		fd = open("/dev/console");
	else
		fd = 2;

   if (fd) {
      name = ttyname(fd);
   	if (name) {
         ioctl(fd,TIOCCONS,&one);
         printf("Redirecting CONSOLE output to: %s\n",name);
         exit(0);
         }
      }
   }
