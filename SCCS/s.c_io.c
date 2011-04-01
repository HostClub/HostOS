h02161
s 00012/00007/00730
d D 1.13 10/03/08 13:28:12 wrc 13 12
c (wrc) updated __c_do_printf() use of 'ap' variable to eliminate use of cast as lvalue
e
s 00013/00013/00724
d D 1.12 02/05/16 15:14:51 kar 12 11
c Version 2: improvements based on my experience with 20013 506.
e
s 00012/00001/00725
d D 1.11 02/04/04 09:22:54 kar 11 10
c Add return and newline processing to printf
e
s 00247/00136/00479
d D 1.10 02/01/22 16:41:03 kar 10 9
c Add interrupt stuff, clean up code
e
s 00062/00043/00553
d D 1.9 02/01/15 16:13:03 kar 9 8
c Remove special types
e
s 00001/00001/00595
d D 1.8 02/01/04 17:26:07 kar 8 7
c fixed hex output of value 0
e
s 00001/00001/00595
d D 1.7 02/01/04 13:28:59 kar 7 6
c fix warning
e
s 00002/00002/00594
d D 1.6 02/01/04 13:22:27 kar 6 5
c replace __inb with inport calls
e
s 00093/00001/00503
d D 1.5 02/01/02 14:20:19 kar 5 4
c Add keyboard input
e
s 00014/00005/00490
d D 1.4 02/01/02 09:50:19 kar 4 3
c Fixed cursor & clear screen in scroll region
e
s 00027/00013/00468
d D 1.3 02/01/02 09:22:53 kar 3 2
c scrolling completed
e
s 00076/00060/00405
d D 1.2 02/01/02 08:11:49 kar 2 1
c ?
e
s 00465/00000/00000
d D 1.1 01/12/22 07:58:41 kar 1 0
c date and time created 01/12/22 07:58:41 by kar
e
u
U
f e 0
t
T
I 1
D 9
//
// File:	c_io.c
// Author:	K. Reek
// Contributors: Jon Coles, Warren R. Carithers
// Description:	Console I/O routines
// SCCS ID:	%W%	%G%
//
E 9
I 9
/*
D 10
** sccs id: %W%        %G%
E 10
I 10
D 12
** SCCS ID:		%W%	%G%
E 12
I 12
** SCCS ID:	%W%	%G%
E 12
E 10
**
D 12
** File:		c_io.c
E 12
I 12
** File:	c_io.c
E 12
**
D 12
** Author:		K. Reek
E 12
I 12
** Author:	K. Reek
E 12
**
D 12
** Contributors:	Jon Coles, Warren R. Carithers
E 12
I 12
** Contributor:	Jon Coles, Warren R. Carithers
E 12
**
D 12
** Description:		Console I/O routines
E 12
I 12
** Description:	Console I/O routines
E 12
**
**	This module implements a simple set of input and output routines
**	for the console screen and keyboard on the Soyatas in the DSL.
**	Refer to the printed documentation for complete details.
**
*/
E 9

D 9
#include "defs.h"
#include "libc.h"
#include "types.h"
#include "globals.h"
#include "protos.h"
E 9
#include "c_io.h"
I 9
#include "startup.h"
I 10
#include "support.h"
#include "x86arch.h"
E 10
E 9

I 9
/*
** Video parameters, and state variables
*/
D 10
#define	VIDEO_BASE_ADDR		0xB8000

E 10
E 9
#define	SCREEN_MIN_X	0
#define	SCREEN_MIN_Y	0
#define	SCREEN_X_SIZE	80
#define	SCREEN_Y_SIZE	25
#define	SCREEN_MAX_X	( SCREEN_X_SIZE - 1 )
#define	SCREEN_MAX_Y	( SCREEN_Y_SIZE - 1 )

I 9
unsigned int	scroll_min_x, scroll_min_y;
unsigned int	scroll_max_x, scroll_max_y;
unsigned int	curr_x, curr_y;
unsigned int	min_x, min_y;
unsigned int	max_x, max_y;

E 9
#ifdef	SA_DEBUG
#include <stdio.h>
#define	c_putchar	putchar
#define	c_puts(x)	fputs( x, stdout )
#endif

D 9
#define	VIDEO_ADDR(x,y)	( ushort_t * ) \
E 9
I 9
#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
E 9
D 3
		( VIDEO_BASE_ADDR + 2 * ( y * SCREEN_X_SIZE + x ) )
E 3
I 3
		( VIDEO_BASE_ADDR + 2 * ( (y) * SCREEN_X_SIZE + (x) ) )
E 3

I 2
D 10
//
// Support routines.
//
// bound: confine an argument within given bounds
// __c_putchar_at: physical output to the video memory
// __c_setcursor: set the cursor location (screen coordinates)
I 9
// __c_strlen: compute the length of a string
E 9
//
E 10
I 10
/*
** Support routines.
**
** bound: confine an argument within given bounds
** __c_putchar_at: physical output to the video memory
** __c_setcursor: set the cursor location (screen coordinates)
** __c_strlen: compute the length of a string
*/
E 10
D 9
static uint_t bound( uint_t min, uint_t value, uint_t max ){
E 9
I 9
static unsigned int bound( unsigned int min, unsigned int value, unsigned int max ){
E 9
	if( value < min ){
		value = min;
	}
	if( value > max ){
		value = max;
	}
	return value;
}

D 5
/*static*/ void __c_setcursor( void ){
E 5
I 5
static void __c_setcursor( void ){
E 5
	unsigned addr;
I 4
D 9
	uint_t	y = curr_y;
E 9
I 9
	unsigned int	y = curr_y;
E 9
E 4

D 4
	addr = (unsigned)( curr_y * SCREEN_X_SIZE + curr_x );
E 4
I 4
	if( y > scroll_max_y ){
		y = scroll_max_y;
	}
E 4

D 4
	outport( 0x3d5, 0xe );
E 4
I 4
	addr = (unsigned)( y * SCREEN_X_SIZE + curr_x );

D 10
	outport( 0x3d4, 0xe );
E 4
	outport( 0x3d5, ( addr >> 8 ) & 0xff );
D 4
	outport( 0x3d5, 0xf );
E 4
I 4
	outport( 0x3d4, 0xf );
E 4
	outport( 0x3d5, addr & 0xff );
E 10
I 10
D 12
	__outport( 0x3d4, 0xe );
	__outport( 0x3d5, ( addr >> 8 ) & 0xff );
	__outport( 0x3d4, 0xf );
	__outport( 0x3d5, addr & 0xff );
E 12
I 12
	__outb( 0x3d4, 0xe );
	__outb( 0x3d5, ( addr >> 8 ) & 0xff );
	__outb( 0x3d4, 0xf );
	__outb( 0x3d5, addr & 0xff );
E 12
E 10
}

D 3
static void __c_putchar_at( uint_t x, uint_t y, ushort_t c ){
E 3
I 3
D 9
static void __c_putchar_at( uint_t x, uint_t y, uint_t c ){
E 9
I 9
static unsigned int __c_strlen( char const *str ){
	unsigned int	len = 0;

	while( *str++ != '\0' ){
		len += 1;
	}
	return len;
}

static void __c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
E 9
E 3
D 10
	//
	// If x or y is too big or small, don't do any output.
	//
E 10
I 10
	/*
	** If x or y is too big or small, don't do any output.
	*/
E 10
D 3
	if( x >= 0 && x <= max_x && y >= 0 && y <= max_y ){
E 3
I 3
	if( x <= max_x && y <= max_y ){
E 3
D 9
		ushort_t *addr = VIDEO_ADDR( x, y );
E 9
I 9
		unsigned short *addr = VIDEO_ADDR( x, y );
E 9

		if( c > 0xff ) {
D 3
			*addr = c;		// use specified attributes
E 3
I 3
D 9
			*addr = (ushort_t)c;	// use specified attributes
E 9
I 9
D 10
			*addr = (unsigned short)c;	// use specified attributes
E 10
I 10
			/*
			** Use the given attributes
			*/
			*addr = (unsigned short)c;
E 10
E 9
E 3
		} else {
D 3
			*addr = c | 0x0700;	// 0 000 0111 = white on black
E 3
I 3
D 9
			*addr = (ushort_t)c | 0x0700;	// 0 000 0111 = white on black
E 9
I 9
D 10
			*addr = (unsigned short)c | 0x0700;	// 0 000 0111 = white on black
E 10
I 10
			/*
			** Use attributes 0000 0111 (white on black)
			*/
			*addr = (unsigned short)c | 0x0700;
E 10
E 9
E 3
		}
	}
}

D 10
//
// Initialization routines
//
E 2
void c_io_init( void ){
	//
	// Screen dimensions
	//
	min_x  = SCREEN_MIN_X;	
	min_y  = SCREEN_MIN_Y;
	max_x  = SCREEN_MAX_X;
	max_y  = SCREEN_MAX_Y;

	//
	// Scrolling region
	//
	scroll_min_x = SCREEN_MIN_X;
	scroll_min_y = SCREEN_MIN_Y;
	scroll_max_x = SCREEN_MAX_X;
	scroll_max_y = SCREEN_MAX_Y;

	//
	// Initial cursor location
	//
	curr_y = min_y;
	curr_x = min_x;
I 4
	__c_setcursor();
E 4
D 9

	//
	// The (as yet unimplemented) input buffer
	//
	in_len = 0;	/* no characters in the input buffer */
	in_curr = 0;	/* next available character in the buffer */
	in_next = 0;	/* next available position in the buffer */
E 9
}

E 10
D 2
int bound( int min, int value, int max ){
	if( value < min ){
		value = min;
	}
	if( value > max ){
		value = max;
	}
	return value;
E 2
I 2
D 9
void c_setscroll( uint_t s_min_x, uint_t s_min_y, uint_t s_max_x, uint_t s_max_y ){
E 9
I 9
void c_setscroll( unsigned int s_min_x, unsigned int s_min_y, unsigned int s_max_x, unsigned int s_max_y ){
E 9
	scroll_min_x = bound( min_x, s_min_x, max_x );
	scroll_min_y = bound( min_y, s_min_y, max_y );
	scroll_max_x = bound( scroll_min_x, s_max_x, max_x );
	scroll_max_y = bound( scroll_min_y, s_max_y, max_y );
I 3
	curr_x = scroll_min_x;
	curr_y = scroll_min_y;
I 4
	__c_setcursor();
E 4
E 3
E 2
}

D 2
void c_moveto( int x, int y ){
E 2
I 2
D 10
//
// Cursor movement in the scroll region
//
E 10
I 10
/*
** Cursor movement in the scroll region
*/
E 10
D 9
void c_moveto( uint_t x, uint_t y ){
E 9
I 9
void c_moveto( unsigned int x, unsigned int y ){
E 9
E 2
	curr_x = bound( scroll_min_x, x + scroll_min_x, scroll_max_x );
	curr_y = bound( scroll_min_y, y + scroll_min_y, scroll_max_y );
I 4
	__c_setcursor();
E 4
}

D 2
void __c_setcursor( void ){
	unsigned addr;

	addr = (unsigned)( curr_y * SCREEN_X_SIZE + curr_x );

	outport( 0x3d5, 0xe );
	outport( 0x3d5, ( addr >> 8 ) & 0xff );
	outport( 0x3d5, 0xf );
	outport( 0x3d5, addr & 0xff );
}

/*
** Print a character to the specified spot on the display.
*/
static void __c_putchar_at( int x, int y, int c ){
	//
	// If x or y is too big or small, don't do any output.
	//
	if( x >= 0 && x <= max_x && y >= 0 && y <= max_y ){
		ushort_t *addr = VIDEO_ADDR( x, y );

		if( c > 0xff ) {
			*addr = c;		// use specified attributes
		} else {
			*addr = c | 0x0700;	// 0 000 0111 = white on black
		}
	}
}

void c_putchar_at( int x, int y, int c ){
E 2
I 2
D 10
//
// The putchar family
//
E 10
I 10
/*
** The putchar family
*/
E 10
D 3
void c_putchar_at( uint_t x, uint_t y, ushort_t c ){
E 3
I 3
D 9
void c_putchar_at( uint_t x, uint_t y, uint_t c ){
E 9
I 9
void c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
E 9
E 3
E 2
	if( ( c & 0x7f ) == '\n' ){
D 2
		int	limit;
E 2
I 2
D 9
		uint_t	limit;
E 9
I 9
		unsigned int	limit;
E 9
E 2

D 10
		//
		// If we're in the scroll region, don't let this loop
		// leave it.  If we're not in the scroll region, don't
		// let this loop enter it.
		//
E 10
I 10
		/*
		** If we're in the scroll region, don't let this loop
		** leave it.  If we're not in the scroll region, don't
		** let this loop enter it.
		*/
E 10
		if( x > scroll_max_x ){
			limit = max_x;
		}
		else if( x >= scroll_min_x ){
			limit = scroll_max_x;
		}
		else {
			limit = scroll_min_x - 1;
		}
		while( x <= limit ){
			__c_putchar_at( x, y, ' ' );
			x += 1;
		}
	}
	else {
		__c_putchar_at( x, y, c );
	}
}

#ifndef SA_DEBUG
D 2
void c_putchar( int c ){
E 2
I 2
D 3
void c_putchar( ushort_t c ){
E 3
I 3
D 9
void c_putchar( uint_t c ){
E 9
I 9
void c_putchar( unsigned int c ){
E 9
D 10
	//
	// If we're off the bottom of the screen, scroll the window.
	//
E 10
I 10
	/*
	** If we're off the bottom of the screen, scroll the window.
	*/
E 10
	if( curr_y > scroll_max_y ){
		c_scroll( curr_y - scroll_max_y );
		curr_y = scroll_max_y;
	}

E 3
E 2
	switch( c & 0xff ){
D 10
	case '\n': // Erase to the end of the line, then scroll
E 10
I 10
	case '\n':
		/*
		** Erase to the end of the line, then move to new line
		** (actual scroll is delayed until next output appears).
		*/
E 10
		while( curr_x <= scroll_max_x ){
			__c_putchar_at( curr_x, curr_y, ' ' );
			curr_x += 1;
		}
		curr_x = scroll_min_x;
		curr_y += 1;
I 3
D 10
#if 0
E 3
		if( curr_y > scroll_max_y ){
			c_scroll( 1 );
			curr_y -= 1;
		}
I 3
#endif
E 10
E 3
		break;

	case '\r':
		curr_x = scroll_min_x;
		break;

	default:
		__c_putchar_at( curr_x, curr_y, c );
		curr_x += 1;
		if( curr_x > scroll_max_x ){
			curr_x = scroll_min_x;
			curr_y += 1;
I 3
D 10
#if 0
E 3
			if( curr_y > scroll_max_y ){
				c_scroll( 1 );
				curr_y -= 1;
			}
I 3
#endif
E 10
E 3
		}
		break;
	}
D 3
	__c_setcursor();
E 3
I 3
D 4
	//__c_setcursor();
E 4
I 4
	__c_setcursor();
E 4
E 3
}
#endif

D 2
void c_puts_at( int x, int y, char *str ){
	unsigned int	ch;
E 2
I 2
D 10
//
// The puts family
//
E 10
I 10
/*
** The puts family
*/
E 10
D 9
void c_puts_at( uint_t x, uint_t y, char *str ){
D 3
	ushort_t	ch;
E 3
I 3
	uint_t	ch;
E 9
I 9
void c_puts_at( unsigned int x, unsigned int y, char *str ){
	unsigned int	ch;
E 9
E 3
E 2

	while( (ch = *str++) != '\0' && x <= max_x ){
D 2
		c_putchar_at( x, y, (int)ch );
E 2
I 2
		c_putchar_at( x, y, ch );
E 2
		x += 1;
	}
}

#ifndef SA_DEBUG
void c_puts( char *str ){
D 2
	unsigned int	ch;
E 2
I 2
D 3
	ushort_t	ch;
E 3
I 3
D 9
	uint_t	ch;
E 9
I 9
	unsigned int	ch;
E 9
E 3
E 2

	while( (ch = *str++) != '\0' ){
D 2
		c_putchar( (int)ch );
E 2
I 2
		c_putchar( ch );
E 2
	}
}
#endif

D 2
void c_clearscreen( void ){
	int	nchars = scroll_max_x - scroll_min_x + 1;
	int	l;
	int	c;
E 2
I 2
void c_clearscroll( void ){
D 9
	uint_t	nchars = scroll_max_x - scroll_min_x + 1;
	uint_t	l;
	uint_t	c;
E 9
I 9
	unsigned int	nchars = scroll_max_x - scroll_min_x + 1;
	unsigned int	l;
	unsigned int	c;
E 9
E 2

	for( l = scroll_min_y; l <= scroll_max_y; l += 1 ){
		unsigned short *to = VIDEO_ADDR( scroll_min_x, l );

		for( c = 0; c < nchars; c += 1 ){
			*to++ = ' ' | 0x0700;
		}
	}
}

D 2
void c_clearwholescreen( void ){
E 2
I 2
void c_clearscreen( void ){
E 2
	unsigned short *to = VIDEO_ADDR( min_x, min_y );
D 2
	int	nchars = ( max_y - min_y + 1 ) * ( max_x - min_x + 1 );
E 2
I 2
D 9
	uint_t	nchars = ( max_y - min_y + 1 ) * ( max_x - min_x + 1 );
E 9
I 9
	unsigned int	nchars = ( max_y - min_y + 1 ) * ( max_x - min_x + 1 );
E 9
E 2

	while( nchars > 0 ){
		*to++ = ' ' | 0x0700;
		nchars -= 1;
	}
}

D 2
void c_setscroll( int s_min_x, int s_min_y, int s_max_x, int s_max_y ){
	scroll_min_x = bound( min_x, s_min_x, max_x );
	scroll_min_y = bound( min_y, s_min_y, max_y );
	scroll_max_x = bound( scroll_min_x, s_max_x, max_x );
	scroll_max_y = bound( scroll_min_y, s_max_y, max_y );
}
E 2

D 2

void c_scroll( int lines ){
E 2
I 2
D 9
void c_scroll( uint_t lines ){
E 2
D 3
	unsigned short *from = VIDEO_ADDR( scroll_min_x, scroll_min_y + lines );
	unsigned short *to = VIDEO_ADDR( scroll_min_x, scroll_min_y );
E 3
I 3
	ushort_t *from;
	ushort_t *to;
E 9
I 9
void c_scroll( unsigned int lines ){
	unsigned short *from;
	unsigned short *to;
E 9
E 3
	int	nchars = scroll_max_x - scroll_min_x + 1;
	int	line, c;

D 10
	//
	// If # of lines is the whole scrolling region or more, just clear.
	//
E 10
I 10
	/*
	** If # of lines is the whole scrolling region or more, just clear.
	*/
E 10
	if( lines > scroll_max_y - scroll_min_y ){
D 4
		c_clearscreen();
E 4
I 4
		c_clearscroll();
E 4
		curr_x = scroll_min_x;
		curr_y = scroll_min_y;
I 4
		__c_setcursor();
E 4
		return;
	}

D 10
	//
	// Must copy it line by line.
	//
E 10
I 10
	/*
	** Must copy it line by line.
	*/
E 10
	for( line = scroll_min_y; line <= scroll_max_y - lines; line += 1 ){
		from = VIDEO_ADDR( scroll_min_x, line + lines );
		to = VIDEO_ADDR( scroll_min_x, line );
		for( c = 0; c < nchars; c += 1 ){
			*to++ = *from++;
		}
	}

	for( ; line <= scroll_max_y; line += 1 ){
		to = VIDEO_ADDR( scroll_min_x, line );
		for( c = 0; c < nchars; c += 1 ){
			*to++ = ' ' | 0x0700;
		}
	}
}

char * cvtdec0( char *buf, int value ){
	int	quotient;

	quotient = value / 10;
	if( quotient < 0 ){
		quotient = 214748364;
		value = 8;
	}
	if( quotient != 0 ){
		buf = cvtdec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

int cvtdec( char *buf, int value ){
	char	*bp = buf;

	if( value < 0 ){
		*bp++ = '-';
		value = -value;
	}
	bp = cvtdec0( bp, value );
	*bp = '\0';

	return bp - buf;
}

char hexdigits[] = "0123456789ABCDEF";

int cvthex( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;

	for( i = 0; i < 8; i += 1 ){
		int	val;

		val = ( value & 0xf0000000 );
D 8
		if( val != 0 || chars_stored ){
E 8
I 8
		if( i == 7 || val != 0 || chars_stored ){
E 8
			chars_stored = 1;
			val >>= 28;
			val &= 0xf;
			*bp++ = hexdigits[ val ];
		}
		value <<= 4;
	}
	*bp = '\0';

	return bp - buf;
}

I 10
int cvtoct( char *buf, int value ){
	int	i;
	int	chars_stored = 0;
	char	*bp = buf;
	int	val;

	val = ( value & 0xc0000000 );
	val >>= 30;
	for( i = 0; i < 11; i += 1 ){

		if( i == 10 || val != 0 || chars_stored ){
			chars_stored = 1;
			val &= 0x7;
			*bp++ = hexdigits[ val ];
		}
		value <<= 3;
		val = ( value & 0xe0000000 );
		val >>= 29;
	}
	*bp = '\0';

	return bp - buf;
}

E 10
static int pad( int x, int y, int extra, int padchar ){
	while( extra > 0 ){
		if( x != -1 || y != -1 ){
			c_putchar_at( x, y, padchar );
			x += 1;
		}
		else {
			c_putchar( padchar );
		}
		extra -= 1;
	}
	return x;
}

static int padstr( int x, int y, char *str, int len, int width, int leftadjust, int padchar ){
	int	extra;

	if( len < 0 ){
D 9
		len = strlen( str );
E 9
I 9
		len = __c_strlen( str );
E 9
	}
	extra = width - len;
	if( extra > 0 && !leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	if( x != -1 || y != -1 ){
		c_puts_at( x, y, str );
		x += len;
	}
	else {
		c_puts( str );
	}
	if( extra > 0 && leftadjust ){
		x = pad( x, y, extra, padchar );
	}
	return x;
}

static void __c_do_printf( int x, int y, char **f ){
	char	*fmt = *f;
D 13
	char	*ap;
E 13
I 13
	int	*ap;
E 13
	char	buf[ 12 ];
	char	ch;
	char	*str;
	int	leftadjust;
	int	width;
	int	len;
	int	padchar;

D 10
	//
	// Get characters from the format string and process them
	//
E 10
I 10
	/*
	** Get characters from the format string and process them
	*/
E 10
D 13
	ap = (char *)( f + 1 );
E 13
I 13
	ap = (int *)( f + 1 );
E 13
	while( (ch = *fmt++) != '\0' ){
D 10
		//
		// Is it the start of a format code?
		//
E 10
I 10
		/*
		** Is it the start of a format code?
		*/
E 10
		if( ch == '%' ){
D 10
			//
			// Yes, get the padding and width options (if there).
			// Alignment must come at the beginning, then fill,
			// then width.
			//
E 10
I 10
			/*
			** Yes, get the padding and width options (if there).
			** Alignment must come at the beginning, then fill,
			** then width.
			*/
E 10
			leftadjust = 0;
			padchar = ' ';
			width = 0;
			ch = *fmt++;
			if( ch == '-' ){
				leftadjust = 1;
				ch = *fmt++;
			}
			if( ch == '0' ){
				padchar = '0';
				ch = *fmt++;
			}
			while( ch >= '0' && ch <= '9' ){
				width *= 10;
				width += ch - '0';
				ch = *fmt++;
			}

D 10
			//
			// What data type do we have?
			//
E 10
I 10
			/*
			** What data type do we have?
			*/
E 10
			switch( ch ){
			case 'c':
D 13
				ch = *( (int *)ap )++;
E 13
I 13
				// ch = *( (int *)ap )++;
				ch = *ap++;
E 13
				buf[ 0 ] = ch;
				buf[ 1 ] = '\0';
				x = padstr( x, y, buf, 1, width, leftadjust, padchar );
				break;

			case 'd':
D 13
				len = cvtdec( buf, *( (int *)ap )++ );
E 13
I 13
				// len = cvtdec( buf, *( (int *)ap )++ );
				len = cvtdec( buf, *ap++ );
E 13
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 's':
D 13
				str = *( (char **)ap )++;
E 13
I 13
				// str = *( (char **)ap )++;
				str = (char *) (*ap++);
E 13
				x = padstr( x, y, str, -1, width, leftadjust, padchar );
				break;

			case 'x':
D 13
				len = cvthex( buf, *( (int *)ap )++ );
E 13
I 13
				// len = cvthex( buf, *( (int *)ap )++ );
				len = cvthex( buf, *ap++ );
E 13
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;
I 10

			case 'o':
D 13
				len = cvtoct( buf, *( (int *)ap )++ );
E 13
I 13
				// len = cvtoct( buf, *( (int *)ap )++ );
				len = cvtoct( buf, *ap++ );
E 13
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

E 10
			}
		}
		else {
			if( x != -1 || y != -1 ){
				c_putchar_at( x, y, ch );
D 11
				x += 1;
E 11
I 11
				switch( ch ){
				case '\n':
					y += 1;
					/* FALL THRU */

				case '\r':
					x = scroll_min_x;
					break;

				default:
					x += 1;
				}
E 11
			}
			else {
				c_putchar( ch );
			}
		}
	}
}

D 3
void c_printf_at( int x, int y, char *fmt, ... ){
E 3
I 3
D 9
void c_printf_at( uint_t x, uint_t y, char *fmt, ... ){
E 9
I 9
void c_printf_at( unsigned int x, unsigned int y, char *fmt, ... ){
E 9
E 3
	__c_do_printf( x, y, &fmt );
}

void c_printf( char *fmt, ... ){
	__c_do_printf( -1, -1, &fmt );
}

I 5
D 10
char scan_code[ 2 ][ 128 ] = {
E 10
I 10
unsigned char scan_code[ 2 ][ 128 ] = {
E 10
	{
/* 00-07 */	'\377',	'\033',	'1',	'2',	'3',	'4',	'5',	'6',
/* 08-0f */	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10-17 */	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 18-1f */	'o',	'p',	'[',	']',	'\n',	'\377',	'a',	's',
/* 20-27 */	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 28-2f */	'\'',	'`',	'\377',	'\\',	'z',	'x',	'c',	'v',
/* 30-37 */	'b',	'n',	'm',	',',	'.',	'/',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	},

	{
/* 00-07 */	'\377',	'\033',	'!',	'@',	'#',	'$',	'%',	'^',
/* 08-0f */	'&',	'*',	'(',	')',	'_',	'+',	'\b',	'\t',
/* 10-17 */	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 18-1f */	'O',	'P',	'{',	'}',	'\n',	'\377',	'A',	'S',
/* 20-27 */	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
/* 28-2f */	'"',	'~',	'\377',	'|',	'Z',	'X',	'C',	'V',
/* 30-37 */	'B',	'N',	'M',	'<',	'>',	'?',	'\377',	'*',
/* 38-3f */	'\377',	' ',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 40-47 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'7',
/* 48-4f */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50-57 */	'2',	'3',	'0',	'.',	'\377',	'\377',	'\377',	'\377',
/* 58-5f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 60-67 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 68-6f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 70-77 */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',
/* 78-7f */	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377',	'\377'
	}
};

D 10
int c_getchar(){
	char	c;
E 10
I 10
#define	C_BUFSIZE	200
#define	KEYBOARD_DATA	0x60
#define	KEYBOARD_STATUS	0x64
#define	READY		0x1
#define	EOT		'\04'

/*
** Circular buffer for input characters.  Characters are inserted at
** __c_next_space, and are removed at __c_next_char.  Buffer is empty if
** these are equal.
*/
static	char	__c_input_buffer[ C_BUFSIZE ];
static	volatile char	*__c_next_char = __c_input_buffer;
static	volatile char	*__c_next_space = __c_input_buffer;

static	volatile char *__c_increment( volatile char *pointer ){
	if( ++pointer >= __c_input_buffer + C_BUFSIZE ){
		pointer = __c_input_buffer;
	}
	return pointer;
}

static void __c_input_scan_code( int code ){
E 10
	static	int	shift = 0;
	static	int	ctrl_mask = 0xff;

D 10
	for(;;){
		/*
		** Wait for a keystroke, then get the scan code
		*/
D 6
		while( ( __inb( 0x64 ) & 0x1 ) == 0 ){
E 6
I 6
		while( ( inport( 0x64 ) & 0x1 ) == 0 ){
E 6
			;
		}
D 6
		c = __inb( 0x60 );
E 6
I 6
		c = inport( 0x60 );
E 10
I 10
	/*
	** Do the shift processing
	*/
	code &= 0xff;
	switch( code ){
	case 0x2a:
	case 0x36:
		shift = 1;
		break;
E 10
E 6

I 10
	case 0xaa:
	case 0xb6:
		shift = 0;
		break;

	case 0x1d:
		ctrl_mask = 0x1f;
		break;

	case 0x9d:
		ctrl_mask = 0xff;
		break;

	default:
E 10
		/*
D 10
		** Do the shift processing
E 10
I 10
		** Process ordinary characters only on the press
		** (to handle autorepeat).
		** Ignore undefined scan codes.
E 10
		*/
D 10
		switch( c ){
		case '\x2a':
		case '\x36':
			shift = 1;
			break;
E 10
I 10
		if( ( code & 0x80 ) == 0 ){
			code = scan_code[ shift ][ (int)code ];
			if( code != '\377' ){
				volatile char	*next = __c_increment( __c_next_space );
E 10

D 10
		case '\xaa':
		case '\xb6':
			shift = 0;
			break;
E 10
I 10
				/*
				** Store character only if there's room
				*/
				if( next != __c_next_char ){
					*__c_next_space = code & ctrl_mask;
					__c_next_space = next;
				}
			}
		}
	}
}
E 10

D 10
		case '\x1d':
			ctrl_mask = 0x1f;
			break;
E 10
I 10
static void __c_keyboard_isr( int vector, int code ){
D 12
	__c_input_scan_code( __inport( KEYBOARD_DATA ) );
	__outport( PIC_MASTER_CMD_PORT, PIC_EOI );
E 12
I 12
	__c_input_scan_code( __inb( KEYBOARD_DATA ) );
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
E 12
}
E 10

D 10
		case '\x9d':
			ctrl_mask = 0xff;
			break;
E 10
I 10
int c_getchar( void ){
	char	c;
	int	interrupts_enabled = __get_flags() & EFLAGS_IF;
E 10

D 10
		default:
			//
			// Process ordinary characters only on the press
			// (to handle autorepeat).
			// Ignore undefined scan codes.
			//
			if( ( c & 0x80 ) == 0 ){
D 7
				c = scan_code[ shift ][ c ];
E 7
I 7
				c = scan_code[ shift ][ (int)c ];
E 7
				if( c != '\377' ){
					return c & ctrl_mask;
				}
E 10
I 10
	while( __c_next_char == __c_next_space ){
		if( !interrupts_enabled ){
			/*
			** Must read the next keystroke ourselves.
			*/
D 12
			while( ( __inport( KEYBOARD_STATUS ) & READY ) == 0 ){
E 12
I 12
			while( ( __inb( KEYBOARD_STATUS ) & READY ) == 0 ){
E 12
				;
E 10
			}
I 10
D 12
			__c_input_scan_code( __inport( KEYBOARD_DATA ) );
E 12
I 12
			__c_input_scan_code( __inb( KEYBOARD_DATA ) );
E 12
E 10
		}
	}
I 10

	c = *__c_next_char & 0xff;
	__c_next_char = __c_increment( __c_next_char );
	if( c != EOT ){
		c_putchar( c );
	}
	return c;
E 10
}

I 10
int c_gets( char *buffer, unsigned int size ){
	char	ch;
	int	count = 0;

	while( size > 1 ){
		ch = c_getchar();
		if( ch == EOT ){
			break;
		}
		*buffer++ = ch;
		count += 1;
		size -= 1;
		if( ch == '\n' ){
			break;
		}
	}
	*buffer = '\0';
	return count;
}

int c_input_queue( void ){
	int	n_chars = __c_next_space - __c_next_char;

	if( n_chars < 0 ){
		n_chars += C_BUFSIZE;
	}
	return n_chars;
}

/*
** Initialization routines
*/
void c_io_init( void ){
	/*
	** Screen dimensions
	*/
	min_x  = SCREEN_MIN_X;	
	min_y  = SCREEN_MIN_Y;
	max_x  = SCREEN_MAX_X;
	max_y  = SCREEN_MAX_Y;

	/*
	** Scrolling region
	*/
	scroll_min_x = SCREEN_MIN_X;
	scroll_min_y = SCREEN_MIN_Y;
	scroll_max_x = SCREEN_MAX_X;
	scroll_max_y = SCREEN_MAX_Y;

	/*
	** Initial cursor location
	*/
	curr_y = min_y;
	curr_x = min_x;
	__c_setcursor();

	/*
	** Set up the interrupt handler for the keyboard
	*/
	__install_isr( INT_VEC_KEYBOARD, __c_keyboard_isr );
}

E 10
E 5
#ifdef SA_DEBUG
int main(){
	c_printf( "%d\n", 123 );
	c_printf( "%d\n", -123 );
	c_printf( "%d\n", 0x7fffffff );
	c_printf( "%d\n", 0x80000001 );
	c_printf( "%d\n", 0x80000000 );
	c_printf( "x%14dy\n", 0x80000000 );
	c_printf( "x%-14dy\n", 0x80000000 );
	c_printf( "x%014dy\n", 0x80000000 );
	c_printf( "x%-014dy\n", 0x80000000 );
	c_printf( "%s\n", "xyz" );
	c_printf( "|%10s|\n", "xyz" );
	c_printf( "|%-10s|\n", "xyz" );
	c_printf( "%c\n", 'x' );
	c_printf( "|%4c|\n", 'y' );
	c_printf( "|%-4c|\n", 'y' );
	c_printf( "|%04c|\n", 'y' );
	c_printf( "|%-04c|\n", 'y' );
	c_printf( "|%3d|\n", 5 );
	c_printf( "|%3d|\n", 54321 );
	c_printf( "%x\n", 0x123abc );
	c_printf( "|%04x|\n", 20 );
	c_printf( "|%012x|\n", 0xfedcba98 );
	c_printf( "|%-012x|\n", 0x76543210 );
}

int curr_x, curr_y, max_x, max_y;
#endif
E 1
