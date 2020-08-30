/*                        Copyright (c) 1987 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: sin.c,v 4.1 88/06/21 13:40:58 bianchi Exp $
	$Source: /tmp/mgrsrc/lib/RCS/sin.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/lib/RCS/sin.c,v $$Revision: 4.1 $";

/*	sine and cosine routines
 *	input:	degrees (integer) 
 *	output:	sine/cosine * 1024
 */

/*	sin table 0-90 degrees * 1024 */

int sintab[] = {
	0, 18, 36, 54, 71, 89, 107, 125, 143, 160,
	178, 195, 213, 230, 248, 265, 282, 299, 316, 333,
	350, 367, 384, 400, 416, 433, 449, 465, 481, 496,
	512, 527, 543, 558, 573, 587, 602, 616, 630, 644,
	658, 672, 685, 698, 711, 724, 737, 749, 761, 773,
	784, 796, 807, 818, 828, 839, 849, 859, 868, 878,
	887, 896, 904, 912, 920, 928, 935, 943, 949, 956,
	962, 968, 974, 979, 984, 989, 994, 998, 1002, 1005,
	1008, 1011, 1014, 1016, 1018, 1020, 1022, 1023, 1023,
	1024, 1024,
	} ;

int
isin(n)
register int n;		/* angle in degrees */
   {
   if (n < 0)
      return(-isin(-n));

   while (n >= 360)
      n -= 360;

   if (n < 90)
      return( sintab[n]);
   else if (n < 180)
      return( sintab[180-n]);
   else if (n < 270)
      return( -sintab[n-180]);
   else
      return( -sintab[360-n]);
   }

int
icos(n)
register int n;
   {
   if (n < 0)
      n = -n;

   while (n >= 360)
      n -= 360;

   if (n < 90)
      return( sintab[90-n]);
   else if (n < 180)
      return( -sintab[n-90]);
   else if (n < 270)
      return( -sintab[270-n]);
   else
      return( sintab[n-270]);
   }
