/*                        Copyright (c) 1988 Bellcore
 *                            All Rights Reserved
 *       Permission is granted to copy or use this program, EXCEPT that it
 *       may not be sold for profit, the copyright notice must be reproduced
 *       on copies, and credit should be given to Bellcore where it is due.
 *       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.
 */
/*	$Header: ckmgrterm.c,v 4.1 88/06/21 13:40:34 bianchi Exp $
	$Source: /tmp/mgrsrc/lib/RCS/ckmgrterm.c,v $
*/
static char	RCSid_[] = "$Source: /tmp/mgrsrc/lib/RCS/ckmgrterm.c,v $$Revision: 4.1 $";

#include	<stdio.h>


/*	Check to see if the TERM environment variable says this is an "mgr"
	terminal.
	If it is not, print a message, optionally preceed by the given text,
	and exit with exit code 1.
	Otherwise, return.
	Absence of a TERM environment variable is considered OK, so that
	these commands can be run within a remote shell.
*/

void
ckmgrterm( text )
char	*text;
{
	char	*getenv();
	char	*term;

	term = getenv( "TERM" );
	/*	If TERM is defined, has a non-null value,
		and TERM != mgr, complain and exit.
	*/
	if( term  &&  *term  &&  strcmp( term, "mgr" ) ) {
		if( text  &&  *text ) {
			fputs( text, stderr );
			fputs( ":  ", stderr );
		}
		fputs( "only runs on mgr terminals\n", stderr );
		exit( 1 );
	}
}
