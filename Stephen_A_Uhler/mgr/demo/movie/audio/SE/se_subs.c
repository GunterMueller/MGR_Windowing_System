/* non-mgr related stuff for se */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "se.h"

/* blast the data to silence */ 

#define LMAX		100			/* maximum limit */

int fade_limit = LMAX;

int
blast(data,count,x)
unsigned char *data;							/* data hunk to play */
int count;							/* # of items */
int x;								/* type 1=zap 2=fade-up 3=fadedown 4=up 5=down */
	{
	static int n;		/* # of samples */
	static int limit;	/* how far to fade */
	int sample;

	if (data==NULL && count > 0) {		/* first call */
		n = count;
		dprintf(stderr,"blasting\n");
		}
	else if (count>0) {		/* blast a hunk */
		dprintf(stderr,"blasting %d\n",count);
		switch(x) {
			case 1:				/* make fade_limit */
				while (count-- > 0)
					*data++ = Sb2i[I2sb[*data]*fade_limit/LMAX];
				break;
			case 2:				/* fade up */
				while (count-- > 0)
					*data++ =
						Sb2i[(I2sb[*data]*((fade_limit-LMAX)*count/n+LMAX))/LMAX];
				break;
			case 3:				/* fade down */
				while (count-- > 0)
					*data++ =
						Sb2i[(I2sb[*data]*((LMAX-fade_limit)*count/n+fade_limit))/LMAX];
				break;
			case 4:				/* up 3db */
				while (count-- > 0) {
					sample = I2sb[*data]*3/2;
					*data++ =  Sb2i[SB13LIMIT(sample)];
					}
				break;
			case 5:				/* down 3db */
				while (count-- > 0)
					*data++ =  Sb2i[I2sb[*data]*2/3];
				break;
			}
		}
	else {		/* last call */
		dprintf(stderr,"Blast done\n");
		}
	return(1);
	}

/* merge 2 samples */

int
merge(data,count,other)
unsigned char *data;							/* data hunk to play */
int count;							/* # of items */
unsigned char *other;					/* data to merge */
	{
	static unsigned char *src;
	int sample;

	if (data==NULL && count > 0) {		/* first call */
		src = other;
		dprintf(stderr,"merging\n");
		}
	else if (count>0) {		/* blast a hunk */
		dprintf(stderr,"merging %d\n",count);
			while (count-- > 0) {
				sample = I2sb[*data]+I2sb[*src++];
				*data++ =  Sb2i[SB13LIMIT(sample)];
				}
		}
	else {		/* last call */
		dprintf(stderr,"merge done\n");
		}
	return(1);
	}

/* play the sample */ 

int
play(data,count,dummy)
char *data;							/* data hunk to play */
int count;							/* # of items */
char *dummy;						/* unused */
	{
	int ok = 1;

	if (data==NULL && count > 0) {		/* first call */
		alarm(0);
		if (audio_fd == 0 &&  (audio_fd = open(AUDIO,1)) < 0) {
			dprintf(stderr,"Cant open audio\n");
			return(0);
			}
		}

	else if (count>0) {		/* play a hunk */
		dprintf(stderr,"playing %d\n",count);
		ok=(write(audio_fd,data,count)==count);
		}

	else {		/* last call */
		alarm(TIMEOUT);
		}
	return(ok);
	}

/* close the audio fd after unused for too long */

int
timer(n)
int n;
	{
	if (audio_fd > 0) {
		close(audio_fd);
		audio_fd = 0;
		dprintf(stderr,"Closing /dev/audio\n");
		}
	}

/* cut at the mark */

int
cut_it(edit,cut)
struct edit *edit;
struct cut *cut;
	{
	struct segment *seg;					/* current segment */
	struct segment *old=NULL;			/* previous segment */
	struct segment *tmp;					/* next segment */
	int offset = 0;						/* current offset (in samples) */

	dprintf(stderr,"Cutting %d\n",E(end)-E(start));
	if (E(end) <= E(start) || E(end)>E(size) || E(start<0))
		return(0);

	/* build cut buffer */

	step(edit,E(start),E(end)-E(start),do_cut,(char *)cut);
	sprintf(s_buff,"cutting %d.%d seconds",CNVT(E(end)-E(start)));
	message(global,s_buff);

	/* cut out samples */

	for (seg = E(seg),old=NULL;seg;old=seg,seg = tmp) {
		tmp = S(next);

		/* entirely in fron of cut regeion, accumulate offset */

		if (E(end) < offset) {	
			offset += S(count);
			dprintf(stderr,"Skipping %d (%d bytes), offset=%d\n",
					S(num), S(count), offset);
			}

		/* segment entirely within cut region - delete it */

		if (E(start) <= offset && E(end) >= offset + S(count)) {
			dprintf(stderr,"Cutting: unlink %d (%d bytes) starting at %d\n",
						 S(num), S(count),offset);
			offset += S(count);
			unlink_seg(edit,old);
			}

		/* past cut region, we're done */

		else if (E(end) < offset) {
			dprintf(stderr,"Cutting: done, seg %d left alone\n",S(num));
			break;
			}

		/* cut region entirely contained in this segment, break in 2 */

		else if (E(start) > offset && E(end) < offset + S(count)) {
			dprintf(stderr,"Cutting  %d from middle of %d (%d samples)\n",
					E(end)-E(start), S(num), offset, S(count));

			(void) append_seg(edit,seg);			/* for samples after cut */
			S(next) -> data = S(data) + E(end)-offset;
			S(next) -> count = offset + S(count) - E(end);
			S(count) = E(start) - offset;
			break;
			}

		/* end of segment in cut region */

		else if (E(start) > offset) {
			dprintf(stderr,"Cutting end of %d\n",S(num));
			offset += S(count);
			S(count) = E(start) - (offset - S(count));
			}

		/* front of segment in cut region (done after this) */

		else if (E(end) < offset + S(count)) {
			dprintf(stderr,"Cutting front of %d\n", S(num));
			S(count) -= E(end) - offset;
			S(data) += E(end) - offset;
			break;
			}
		}
	E(size) -= E(end) - E(start);
	E(end) = E(start);
	}

/* append a new segment after current one, first one if seg==NULL */

struct segment *
append_seg(edit,seg)
struct edit *edit;
struct segment *seg;
	{
	struct segment *tmp = (struct segment *) malloc(sizeof(struct segment));

	if (seg) {
		dprintf(stderr,"   appending new segment %d after %d\n",seg_num,S(num));
		tmp->next = S(next);
		S(next) = tmp;
		}
	else {		/* first segment */
		dprintf(stderr,"   appending new segment %d at front\n",seg_num);
		tmp->next = E(seg);
		E(seg) = tmp;
		}
		
	tmp->num = seg_num++;
	E(num)++;
	return(tmp);
	}

/* unlink segment "seg" points to, or 1st seg iff NULL  */

unlink_seg(edit,seg)
struct edit *edit;
struct segment *seg;
	{
	struct segment *tmp;

	if (seg && (tmp=S(next))){
		S(next) = tmp->next;
		dprintf(stderr,"   unlinking segment %d (%d samples)\n",
				tmp->num,tmp->count);
		free(tmp);
		E(num)--;
		}
	else if (!seg && E(seg)) {
		E(seg) = E(seg)->next;
		dprintf(stderr,"unlinking first segment %d (%d samples)\n",
				tmp->num,tmp->count);
		free(tmp);
		E(num)--;
		}
	}

/* copy an edit region */

struct edit*
copy_edit(edit)
struct edit *edit;
	{
	struct edit *result;
	struct segment *seg, *newseg=NULL;

	result = (struct edit *) malloc(sizeof(struct edit));
	*result = *edit;			/* structure copy */
	result->seg = NULL;

	for (seg = E(seg);seg;seg = S(next)) {
		newseg = append_seg(result,newseg);
		newseg->data = S(data);
		newseg->count = S(count);
		}
	return(result);
	}

/* free an edit list , free entire edit iff flag */

free_edit(edit,flag)
struct edit *edit;
int flag;
	{
	struct segment *seg,*tmp;

	for (seg = E(seg);seg;seg = tmp) {
		tmp=S(next);
		free(seg);
		}

	if (flag)
		free(edit);
	else
		E(seg) = NULL;
	}

/* step though edit region, calling a function to do the work
 * The first call gives the count, then next 'n' calls pass a hunk
 * of the data.  The last call, (count 0) gets the return code
 */

int
step(edit,s1,num,func,arg)
struct edit *edit;				/* sound to step through */
int s1,num;							/* starting and sample and count to play */
int (*func)();						/* function to call at each step */
char *arg;							/* extra argument for func */
	{
	struct segment *seg;			/* first segment */
	unsigned char *data;			/* current data pointer */
	int count;						/* # of bytes to play in sample */
	int ok;							/* func() call was ok */
	int offset = 0;				/* current sample offset */

	if (s1<0)
		s1=0;
	if (num+s1 > E(size))
		num = E(size)-s1;
	if (num <=0 || s1 <0)
		return(-1);

	ok = (*func)(NULL,num,arg);		/* initialize with count */
	dprintf(stderr,"Stepping\n");

	for (seg = E(seg);ok && seg;offset+=S(count),seg = S(next)) {

		if (s1 <= offset && s1+num  >= offset + S(count)) { /* entire seg */
			dprintf(stderr,"step whole seg (%d)\n",S(num));
			ok = (*func)(S(data),S(count),arg);
			}
		else if (s1+num < offset) {		/* past the end */
			break;
			}
		else if (s1 > offset + S(count)) {		/* not there yet */
			continue;
			}
		else if (s1 > offset && s1+num < offset + S(count)) { /* middle */
			dprintf(stderr,"step middle of seg %d\n",S(num));
			ok = (*func)(S(data)+s1-offset,num,arg);
			}
		else if (s1 > offset) {		/* end */
			dprintf(stderr,"step end of seg %d\n",S(num));
			ok=(*func)(S(data)+s1-offset,offset+S(count)-s1,arg);
			}
		else if (s1+num < offset + S(count)) { /* front */
			dprintf(stderr,"step front of seg %d\n",S(num));
			ok = (*func)(S(data),s1+num-offset,arg);
			break;
			}
		else
			dprintf(stderr,"ERROR IN STEP\n");
		}
	return ((*func)(NULL,0,arg));		/* final call, get return code */
	}

/* fetch file to cut buffer */
#define TOOBIG		(9000*60*10)		/* 10 minutes */

int
fetch_it(cut,name)
struct cut *cut;
char *name;
	{
	int fd;
	struct stat buf;
	int size;

	if ((fd = open(name,O_RDONLY)) <= 0) {
		dprintf(stderr,"Cant open %s for fetching\n",name);
		sprintf(s_buff,"Cant open file %s",name);
		message(global,s_buff);
		return(0);
		}

	if (stat(name,&buf)< 0 || (size=buf.st_size) < 0 || size > TOOBIG) {
		dprintf(stderr,"Cant fetch file %s\n",name);
		sprintf(s_buff,"Cant fetch file %s (too big or small)",name);
		message(global,s_buff);
		return(0);
		}

	if (cut->data)
		free(cut->data);

	cut->data = (unsigned char *) malloc(size);
	read(fd,cut->data,size);
	cut->count=size;
	sprintf(s_buff,"Got %s into cut buffer (%d.%d seconds)",name,CNVT(size));
	message(global,s_buff);
	return(1);
	}

/* save mark to file */

int append = 0;

int
file_it(data,count,name)
unsigned char *data;
int count;
char *name;
	{
	static int fd;
	int ok;

	dprintf(stderr,"Filing\n");
	if (data==NULL && count > 0) {		/* first call */
		if (append)
			fd = open(name,O_APPEND|O_CREAT,0644);		
		else
			fd = open(name,O_WRONLY|O_CREAT,0644);		
		append = 0;
		ok = (fd>0);
		if (!ok) {
			dprintf(stderr,"Cant save %d in %s\n",count,name);
			sprintf(s_buff,"Cant save %d bytes to file %s",count,name);
			message(global,s_buff);
			}
		else
			sprintf(s_buff,"Saving %d bytes to file %s",count,name);
			message(global,s_buff);
		}
	else if (count>0) {		/* write out a hunk */
		dprintf(stderr,"saving %d\n",count);
		ok=(write(fd,data,count)==count);
		}

	else {		/* last call */
		ftruncate(fd,tell(fd));
		close(fd);
		}
	return(ok);
	}

/* paste in the cut buffer */

int
paste(edit,cut,where)
struct edit *edit;			/* what to paste into */
struct cut *cut;				/* what to paste */
int where;						/* where to paste */
	{	
	struct segment *seg,*tmp;
	int offset=0;

	if (cut->count <=0 || cut->data == NULL)
		return(-1);

	E(size) += cut->count;

	dprintf(stderr,"In paste\n");
	for (seg = E(seg);seg;offset+=S(count),seg = tmp) {
		tmp = S(next);
		if (offset + S(count) < where){		/* in front */
			continue;
			}
		else if (where < offset) {				/* in back */
			break;
			}

		else if (where==offset+S(count)) {	/* insert after */
			seg = append_seg(edit,seg);
			S(count) = cut->count;
			S(data) =  (unsigned char *) malloc(cut->count);
			bcopy(cut->data,S(data),cut->count);
			break;
			}
		else {											/* in the middle */
			append_seg(edit,seg);			/* rest of old seg */
			S(next) -> data = S(data) + where - offset;
			S(next) -> count =  offset + S(count) - where;

			S(count) = where - offset;			/* fix up front part */
	
			seg = append_seg(edit,seg);			/* new segment */
			offset = where;
			S(count) = cut->count;
			S(data) =  (unsigned char *) malloc(cut->count);
			bcopy(cut->data,S(data),cut->count);
			}
		}
	return(0);
	}

/* fill the cut buffer (called from step) */

int
do_cut(data,count,arg)
unsigned char *data;
int count;
char *arg;
	{
	struct cut *cut = (struct cut *) arg;
	static int sum;					/* bytes so far */

	if (data==NULL && count > 0) {		/* first call */
		if (cut->data)
			free(cut->data);
		cut->data = (unsigned char *) malloc(count);
		cut->count = count;
		sum = 0;
		}
	else if (count > 0) {
		dprintf(stderr,"do_cut Copied %d starting %d\n",count,sum);
		bcopy(data,cut->data+sum,count);
		sum += count;
		}
	return(1);
	}

#ifdef LATER

/* fill the cut buffer (called from step) */

int
do_fft(data,count,arg)
unsigned char *data;
int count;
char *arg;
	{
	static int sum;					/* bytes so far */
	static int n;						/* # of samples */
	static char *result=NULL;
	static char *data=NULL;
	char *malloc();

	if (data==NULL && count > 0) {		/* first call */
		n = 1<< (int) arg;
		for(n=1<<(int)arg;n<count;n>>=1)
			;
		data = malloc(count);
		sum = 0;
		}
	else if (count > 0) {
		dprintf(stderr,"do_fft Copied %d starting %d\n",count,sum);
		bcopy(data,data+sum,count);
		sum += count;
		}
	else if (count == 0) {		/* do the fft */
		if (result)
			free(result);
		result = malloc(n/2+1);
		fft(data,n,result);		
		plot_fft(result,b);
		}	
	return(1);
	}
#endif

/* histogram the selected region */

int
do_hist(data,count,arg)
unsigned char *data;
int count;
char *arg;
	{
	unsigned int buckets[256];
	int i;

	if (data==NULL && count > 0) {		/* first call */
		bzero(buckets,sizeof(buckets));
		}
	else if (count > 0) {					/* divide into buckets */
		while (count-- > 0)
			buckets[*data++]++;
		}
	else if (count == 0) {					/* display the histogram */
		show_hist(buckets);
		}	
	return(1);
	}
