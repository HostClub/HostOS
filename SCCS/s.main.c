h42699
s 00004/00004/00013
d D 1.3 05/03/15 15:32:56 wrc 3 2
c (wrc) minor tweaks
e
s 00001/00001/00016
d D 1.2 03/09/30 11:26:43 kar 2 1
c 
e
s 00017/00000/00000
d D 1.1 02/05/16 15:13:21 kar 1 0
c date and time created 02/05/16 15:13:21 by kar
e
u
U
f e 0
t
T
I 1
/*
** SCCS ID:	%W%	%G%
**
** File:	main.c
**
** Author:	K. Reek
**
D 3
** Contributor:
E 3
I 3
** Contributor:	Warren R. Carithers
E 3
**
** Description:	Dummy main program
*/
#include "c_io.h"

D 3
int main ( void ) {
D 2
	c_printf( "Hello world!\n", cp );
E 2
I 2
	c_puts( "Hello world!\n" );
E 2
	return 0;
E 3
I 3
int main( void ) {
	c_puts( "Hello, world!\n" );
	return( 0 );
E 3
}
E 1
