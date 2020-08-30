/*                        Copyright (c) 1988 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: /home/sau/mgr/demo/icon/snap.c,v 1.1 91/07/18 08:19:39 sau Exp Locker: sau $
	$Source: /home/sau/mgr/demo/icon/snap.c,v $
*/
static char	RCSid_[] = "$Source: /home/sau/mgr/demo/icon/snap.c,v $$Revision: 1.1 $";

/* snap a piece of the screen -- only works locally */

#include <signal.h>
#include <sys/file.h>
#include "term.h"
#include "bitmap.h"
#include "dump.h"
#ifdef PBM
#include <pbm.h>
#endif

#define ICON   "easel"
#define SCREEN   "/dev/bwtwo0"
#define min(x,y)      ((x)>(y)?(y):(x))
#define dprintf   if(debug)fprintf
#define PRINTER   "lp"         /* default printer name */
#define CANCEL      10            /* time after which REVIEW is canceled */

static char buff[100];         /* mgr input buffer */
static char cmd[100];            /* lpr command buffer */
static char *name;               /* file name */
static char my_host[32];            /* my-host */
static char mgr_host[32];            /* mgr host */
static char mode[25];						/* snap mode */
#ifdef PBM
static char tmp_name[100];			/* temporary file name */
#endif

static int debug=0;
static int review=0;            /* review more set */
static int func = BIT_SRC;

#define MENU_COUNT      (sizeof(menu)/sizeof(struct menu_entry))

static struct menu_entry menu[] = {
	mode,"F\rZ\r",
   "print","Print\r",
   "file","File\r",
   "review =>","View\r",
   "quit","Quit\r",
};
static struct menu_entry rop[] = {
   "set","-Set\r",
   "paint","-Paint\r",
   "xor","-Xor\r",
   "mask","-Mask\r",
};

main(argc,argv)
int argc;
char **argv;
   {
   BITMAP *screen , *tmp = (BITMAP *) 0;
   char *printer, *getenv();

   FILE *fp;      /* file to write */
   int w,h;
   int wide,high;   /* picture size */
   int x,y;      /* window pos */
   int x1,y1;   /* sweep coords */
   int xmax, ymax;      /* display size */
   int n;
   int snapping = 0;      /* ready to snap */
	int message=0;			/* responding to message */
   int format = NEW_BHDR;			/* new format */
   int cancel(),clean();
	char *c, *index(), *rindex();

   FILE *pf;
   
   ckmgrterm();

   debug = (int) getenv("DEBUG");

   if (argc > 1 && strcmp("-n",argv[1])==0) {
      format=NEW_BHDR;
      argc--;
      argv++;
		}

   if (argc != 2) {
      fprintf(stderr,"Usage: snap [-n] <file>\n");
      exit(1);
      }

   name = argv[1];
#ifdef PBM
	sprintf(tmp_name,"/tmp/snap.%d",getpid());
#endif

   if ((screen = bit_open(SCREEN)) == (BITMAP *) 0) {
      fprintf(stderr,"%s: Can't find %s\n",*argv,SCREEN);
      exit(1);
      }

   if ((fp = fopen(name, "w")) == NULL) {
      perror("fopen");
      fprintf(stderr,"%s: Can't fopen %s\n",*argv,name);
      exit(1);
      }

   if ((printer = getenv("PRINTER")) == (char *) 0)
      printer = PRINTER;
   sprintf(cmd,"lpr -P%s -J%s -v",printer,name);

   /* setup mgr library */

	if (debug)
   	m_setup(M_FLUSH);
	else
   	m_setup(M_MODEOK);

   get_param(mgr_host,&xmax,&ymax,0);
   gethostname(my_host,sizeof(my_host));

   if (strcmp(my_host,mgr_host) != 0) {
      fprintf(stderr,"%s only works on host: %s\n",
               argv[0],mgr_host);
      exit(1);
       }

   m_push(P_EVENT|P_CURSOR|P_FONT|P_FLAGS|P_MENU|P_POSITION);
   m_ttyset();
   signal(SIGALRM,cancel);
   signal(SIGTERM,clean);
   signal(SIGINT,clean);
   signal(SIGHUP,clean);

   m_setmode(M_NOWRAP);
   m_setmode(M_ABS);
	m_setcursor(CS_INVIS);
   m_func(B_COPY);
   m_bitfromfile(1,ICON);
   m_flush();
   m_gets(buff);
   n = sscanf(buff,"%d %d",&w,&h);
   if (n < 2) {
      fprintf(stderr,"%s: Can't find %s\n",*argv,ICON);
      clean(1);
      }
   setup(1,w,h,mode);
   m_setevent(BUTTON_1,"S%R\r");   /* get coords */
   m_setevent(REDRAW,"Redraw\r");	/* get coords */
   m_setevent(RESHAPE,"Reshape\r");	/* get coords */

	/* message stuff */
	fchmod(0,0600);				/* activate messages */
   m_setevent(ACCEPT,"M %f (%m)\r");	/* accept message */
   m_setevent(NOTIFY,"snap"); /* type of thingy */

   menu_load(2,4,rop);
	menu_setup(0);
   m_clearmode(M_ACTIVATE);

   m_flush();
   while(m_gets(buff)) {
      dprintf(stderr,"SNAP got %s",buff);
      switch (*buff) {
      case 'M':            /* Got a message */
			message = atoi(buff+2);
			dprintf(stderr,"Got a message from %d\n",message);
			if ((c=index(buff,'(')) && c[1] == 'H') { /* help message */
#ifdef PBM
				sprintf(buff,"F;image/pbm;capture an image");
#else
				sprintf(buff,"F;image/mgr;capture an image");
#endif
				m_sendto(message,buff);
				dprintf(stderr,"SNAP got HELP message from %d\n",message);
				message = 0;
				m_setmode(M_WOB);
				m_flush();
				sleep(1);
				m_clearmode(M_WOB);
				}
			else if (c && c[1] == 'M') {					/* mail stuff */	
				*rindex(c,')') = '\0';
				strcpy(mode,c+3);
         	setup(1,w,h,mode);
#ifdef PBM
				sprintf(buff,"F;image/pbm;capture an image");
#else
				sprintf(buff,"F;image/mgr;capture an image");
#endif
				m_sendto(message,buff);
				dprintf(stderr,"SNAP got MAIL message from %d\n",message);
         	m_setmode(M_ACTIVATE);
				m_setevent(DEACTIVATE,"D\r");			/* window deactivated */
				menu_setup(1);
				}
         else if (c && c[1] == 'D') {              /* done */
#ifdef PBM
				unlink(tmp_name);
#endif
				dprintf(stderr,"snap: got done message from %d\n",message);
            }
			else {	
				dprintf(stderr,"snap got UNKNOWN message from %d\n",message);
				m_sendto(message,"?");
				message = 0;
				}
         break;
      case 'D':            /* deactivated in message mode */
   		m_clearevent(DEACTIVATE);	
			m_sendto(message,"X");
         dprintf(stderr,"SNAP sent abort message to %d\n",message);
			menu_setup(0);
			*mode = '\0';
         setup(1,w,h,mode);
			message = 0;
			m_clearmode(M_ACTIVATE);
         break;
      case 'R':            /* redraw */
         setup(1,w,h,mode);
         break;
      case 'S':            /* set up to snap a picture */
         n = sscanf(buff+1,"%d %d %d %d",&x,&y,&x1,&y1);
         if (n < 4)
            break;
         m_setmode(M_WOB);
         wide = abs(x1-x);
         high = abs(y1-y);
         x = min(x,x1);
         y = min(y,y1);
         m_push(P_MOUSE);
         if (x > 16 || y > 16)
            m_movemouse(0,0);         /* get mouse out of the picture */
         else 
            m_movemouse(xmax-17,ymax-17);
         if (review) {
            alarm(0);
            m_sendme("B review\r");                     /* synchronize review */
            }
         else {
            m_sendme("E snap\r");                     /* synchronize snap */
            snapping = 1;
            if (tmp) {
               bit_destroy(tmp);
               tmp = NULL;
               }
            tmp = bit_alloc(wide,high,BIT_NULL,1);
            }
         break;
      case 'B':               /* review the picture */
         if (review && tmp && !snapping) {
            review = 0;
            if (wide < 20 && high < 20) {
               wide = BIT_WIDE(tmp);
               high = BIT_HIGH(tmp);
               }
      
            dprintf(stderr,"review %d,%d by %d,%d\n",x,y,wide,high);
            bit_blit(screen,x,y,wide,high,func,tmp,0,0);
            m_pop();
            }
         m_clearmode(M_WOB);
         break;
      case 'Z':               /* send the picture (to mail) */
#ifdef PBM
			sprintf(buff,"< %s mgrtopbm > %s",name,tmp_name);
			dprintf(stderr,"SNAP running: %s\n",buff);
			if (system(buff) != 0) {
				sprintf(buff,"X");
				dprintf(stderr,"SNAP cant convert image to pbm\n");
				}
			else
				sprintf(buff,"F;%s:%s",my_host,tmp_name);
#else
			sprintf(buff,"F;%s:%s",my_host,name);
			dprintf(stderr,"SNAP sending (%s) to %d\n",buff,message);
#endif
			m_sendto(message,buff);
			message=0;
			*mode = '\0';
   		setup(1,w,h,mode);
			menu_setup(0);
			m_clearevent(DEACTIVATE);
			m_clearmode(M_ACTIVATE);
         break;
      case 'E':               /* get the picture */
         if (snapping && x!=16 && y!=16) {
            bit_blit(tmp,0,0,wide,high,BIT_SRC,screen,x,y);
            m_clearmode(M_WOB);
            }
			m_pop();
         snapping = 0;
         break;
      case 'F':                  /* file it */
         if (tmp && !snapping) {
            fseek(fp,0L,0);
            ftruncate(fileno(fp),0);
	         if (!bitmapwrite(fp,tmp,format) ) {
               m_push(P_ALL);
               m_font(0);
               m_size(27,3);
               m_clear();
               m_printstr("unable to write file\n");
               m_printstr(name);
               m_flush();
               sleep(3);
               m_pop();
               m_flush();
               }
            dprintf(stderr,"SNAP filing\n");
            }
			fflush(fp);
         break;
      case 'Q':                  /* quit */
         clean(0);
         break;
      case 'P':                  /* print */
         if (tmp && !snapping && (pf = popen(cmd,"w"))) {
				bitmapwrite(pf,tmp,format);
            dprintf(stderr,"SNAP printing [%s]\n",cmd);
            pclose(pf);
            }
         break;
      case '-':                  /* set review mode*/
         if (review) 
            switch (*(buff+1)) {
               case 'S':         /* set */
                  func = BIT_SRC;
                  break;
               case 'P':         /* paint */
                  func = BIT_SRC | BIT_DST;
                  break;
               case 'X':         /* XOR */
                  func = BIT_SRC ^ BIT_DST;
                  break;
               case 'M':         /* MASK */
                  func = BIT_SRC & BIT_DST;
                  break;
               }
      case 'V':                  /* review */
         if (tmp && !snapping) {
            alarm(CANCEL);
            review++;
            m_setmode(M_WOB);
            }
         break;
         }
      m_flush();
      }
   
   clean(0);
   }

int clean(n)
int n;
   {
   m_ttyreset();
   m_popall();
   exit(n);
   }

setup(where,w,h,msg)
int where;      /* bitmap # */
int w,h;         /* window size */
char *msg;		 /* message to overwrite */
   {
   int wx,wy;	/* window position */

   get_size(&wx,&wy,0,0);
   m_shapewindow(wx,wy,w+10,h+10);
   m_clear();
   m_bitcopyto(0,0,w,h,0,0,0,where);
   m_setcursor(CS_INVIS);
	if (msg && *msg) {
		int fw,fh;		/* font size */
		get_font(&fw,&fh);
		m_func(B_OR);
		m_stringto(0,(w-fw*strlen(msg))/2,h-2,msg);
      dprintf(stderr,"displaying %s\n",msg);
		m_func(B_COPY);
		}
   }

int
cancel()
   {
   review = 0;
   m_putchar('\007');
   m_clearmode(M_WOB);
   m_flush();
   }

menu_setup(n)
int n;		/* true iff mail menu */
	{
   menu_load(1,4+n,menu+1-n);
   m_selectmenu(1);
   m_linkmenu(1,2+n,2,6);
	dprintf(stderr,"snap: menu %s\n",n?menu[0].value:"NONE");
	}
