/*
** SCCS ID:	@(#)c_io.c	1.13	03/08/10
**
** File:	c_io.c
**
** Author:	K. Reek
**
** Contributor:	Jon Coles, Warren R. Carithers
**
** Description:	Console I/O routines
**
**	This module implements a simple set of input and output routines
**	for the console screen and keyboard on the Soyatas in the DSL.
**	Refer to the printed documentation for complete details.
**
*/

#include "c_io.h"
#include "startup.h"
#include "support.h"
#include "x86arch.h"

/*
** Video parameters, and state variables
*/
#define	SCREEN_MIN_X	0
#define	SCREEN_MIN_Y	0
#define	SCREEN_X_SIZE	80
#define	SCREEN_Y_SIZE	25
#define	SCREEN_MAX_X	( SCREEN_X_SIZE - 1 )
#define	SCREEN_MAX_Y	( SCREEN_Y_SIZE - 1 )

unsigned int	scroll_min_x, scroll_min_y;
unsigned int	scroll_max_x, scroll_max_y;
unsigned int	curr_x, curr_y;
unsigned int	min_x, min_y;
unsigned int	max_x, max_y;

#ifdef	SA_DEBUG
#include <stdio.h>
#define	c_putchar	putchar
#define	c_puts(x)	fputs( x, stdout )
#endif

#define	VIDEO_ADDR(x,y)	( unsigned short * ) \
		( VIDEO_BASE_ADDR + 2 * ( (y) * SCREEN_X_SIZE + (x) ) )

/*
** Support routines.
**
** bound: confine an argument within given bounds
** __c_putchar_at: physical output to the video memory
** __c_setcursor: set the cursor location (screen coordinates)
** __c_strlen: compute the length of a string
*/
static unsigned int bound( unsigned int min, unsigned int value, unsigned int max ){
	if( value < min ){
		value = min;
	}
	if( value > max ){
		value = max;
	}
	return value;
}

static void __c_setcursor( void ){
	unsigned addr;
	unsigned int	y = curr_y;

	if( y > scroll_max_y ){
		y = scroll_max_y;
	}

	addr = (unsigned)( y * SCREEN_X_SIZE + curr_x );

	__outb( 0x3d4, 0xe );
	__outb( 0x3d5, ( addr >> 8 ) & 0xff );
	__outb( 0x3d4, 0xf );
	__outb( 0x3d5, addr & 0xff );
}

static unsigned int __c_strlen( char const *str ){
	unsigned int	len = 0;

	while( *str++ != '\0' ){
		len += 1;
	}
	return len;
}

static void __c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
	/*
	** If x or y is too big or small, don't do any output.
	*/
	if( x <= max_x && y <= max_y ){
		unsigned short *addr = VIDEO_ADDR( x, y );

		if( c > 0xff ) {
			/*
			** Use the given attributes
			*/
			*addr = (unsigned short)c;
		} else {
			/*
			** Use attributes 0000 0111 (white on black)
			*/
			*addr = (unsigned short)c | 0x0700;
		}
	}
}

void c_setscroll( unsigned int s_min_x, unsigned int s_min_y, unsigned int s_max_x, unsigned int s_max_y ){
	scroll_min_x = bound( min_x, s_min_x, max_x );
	scroll_min_y = bound( min_y, s_min_y, max_y );
	scroll_max_x = bound( scroll_min_x, s_max_x, max_x );
	scroll_max_y = bound( scroll_min_y, s_max_y, max_y );
	curr_x = scroll_min_x;
	curr_y = scroll_min_y;
	__c_setcursor();
}

/*
** Cursor movement in the scroll region
*/
void c_moveto( unsigned int x, unsigned int y ){
	curr_x = bound( scroll_min_x, x + scroll_min_x, scroll_max_x );
	curr_y = bound( scroll_min_y, y + scroll_min_y, scroll_max_y );
	__c_setcursor();
}

/*
** The putchar family
*/
void c_putchar_at( unsigned int x, unsigned int y, unsigned int c ){
	if( ( c & 0x7f ) == '\n' ){
		unsigned int	limit;

		/*
		** If we're in the scroll region, don't let this loop
		** leave it.  If we're not in the scroll region, don't
		** let this loop enter it.
		*/
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
void c_putchar( unsigned int c ){
	/*
	** If we're off the bottom of the screen, scroll the window.
	*/
	if( curr_y > scroll_max_y ){
		c_scroll( curr_y - scroll_max_y );
		curr_y = scroll_max_y;
	}

	switch( c & 0xff ){
	case '\n':
		/*
		** Erase to the end of the line, then move to new line
		** (actual scroll is delayed until next output appears).
		*/
		while( curr_x <= scroll_max_x ){
			__c_putchar_at( curr_x, curr_y, ' ' );
			curr_x += 1;
		}
		curr_x = scroll_min_x;
		curr_y += 1;
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
		}
		break;
	}
	__c_setcursor();
}
#endif

/*
** The puts family
*/
void c_puts_at( unsigned int x, unsigned int y, char *str ){
	unsigned int	ch;

	while( (ch = *str++) != '\0' && x <= max_x ){
		c_putchar_at( x, y, ch );
		x += 1;
	}
}

#ifndef SA_DEBUG
void c_puts( char *str ){
	unsigned int	ch;

	while( (ch = *str++) != '\0' ){
		c_putchar( ch );
	}
}
#endif

void c_clearscroll( void ){
	unsigned int	nchars = scroll_max_x - scroll_min_x + 1;
	unsigned int	l;
	unsigned int	c;

	for( l = scroll_min_y; l <= scroll_max_y; l += 1 ){
		unsigned short *to = VIDEO_ADDR( scroll_min_x, l );

		for( c = 0; c < nchars; c += 1 ){
			*to++ = ' ' | 0x0700;
		}
	}
}

void c_clearscreen( void ){
	unsigned short *to = VIDEO_ADDR( min_x, min_y );
	unsigned int	nchars = ( max_y - min_y + 1 ) * ( max_x - min_x + 1 );

	while( nchars > 0 ){
		*to++ = ' ' | 0x0700;
		nchars -= 1;
	}
}


void c_scroll( unsigned int lines ){
	unsigned short *from;
	unsigned short *to;
	int	nchars = scroll_max_x - scroll_min_x + 1;
	int	line, c;

	/*
	** If # of lines is the whole scrolling region or more, just clear.
	*/
	if( lines > scroll_max_y - scroll_min_y ){
		c_clearscroll();
		curr_x = scroll_min_x;
		curr_y = scroll_min_y;
		__c_setcursor();
		return;
	}

	/*
	** Must copy it line by line.
	*/
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
		if( i == 7 || val != 0 || chars_stored ){
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
		len = __c_strlen( str );
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
	int	*ap;
	char	buf[ 12 ];
	char	ch;
	char	*str;
	int	leftadjust;
	int	width;
	int	len;
	int	padchar;

	/*
	** Get characters from the format string and process them
	*/
	ap = (int *)( f + 1 );
	while( (ch = *fmt++) != '\0' ){
		/*
		** Is it the start of a format code?
		*/
		if( ch == '%' ){
			/*
			** Yes, get the padding and width options (if there).
			** Alignment must come at the beginning, then fill,
			** then width.
			*/
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

			/*
			** What data type do we have?
			*/
			switch( ch ){
			case 'c':
				// ch = *( (int *)ap )++;
				ch = *ap++;
				buf[ 0 ] = ch;
				buf[ 1 ] = '\0';
				x = padstr( x, y, buf, 1, width, leftadjust, padchar );
				break;

			case 'd':
				// len = cvtdec( buf, *( (int *)ap )++ );
				len = cvtdec( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 's':
				// str = *( (char **)ap )++;
				str = (char *) (*ap++);
				x = padstr( x, y, str, -1, width, leftadjust, padchar );
				break;

			case 'x':
				// len = cvthex( buf, *( (int *)ap )++ );
				len = cvthex( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			case 'o':
				// len = cvtoct( buf, *( (int *)ap )++ );
				len = cvtoct( buf, *ap++ );
				x = padstr( x, y, buf, len, width, leftadjust, padchar );
				break;

			}
		}
		else {
			if( x != -1 || y != -1 ){
				c_putchar_at( x, y, ch );
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
			}
			else {
				c_putchar( ch );
			}
		}
	}
}

void c_printf_at( unsigned int x, unsigned int y, char *fmt, ... ){
	__c_do_printf( x, y, &fmt );
}

void c_printf( char *fmt, ... ){
	__c_do_printf( -1, -1, &fmt );
}

unsigned char scan_code[ 2 ][ 128 ] = {
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
	static	int	shift = 0;
	static	int	ctrl_mask = 0xff;

	/*
	** Do the shift processing
	*/
	code &= 0xff;
	switch( code ){
	case 0x2a:
	case 0x36:
		shift = 1;
		break;

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
		/*
		** Process ordinary characters only on the press
		** (to handle autorepeat).
		** Ignore undefined scan codes.
		*/
		if( ( code & 0x80 ) == 0 ){
			code = scan_code[ shift ][ (int)code ];
			if( code != '\377' ){
				volatile char	*next = __c_increment( __c_next_space );

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

static void __c_keyboard_isr( int vector, int code ){
	__c_input_scan_code( __inb( KEYBOARD_DATA ) );
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

int c_getchar( void ){
	char	c;
	int	interrupts_enabled = __get_flags() & EFLAGS_IF;

	while( __c_next_char == __c_next_space ){
		if( !interrupts_enabled ){
			/*
			** Must read the next keystroke ourselves.
			*/
			while( ( __inb( KEYBOARD_STATUS ) & READY ) == 0 ){
				;
			}
			__c_input_scan_code( __inb( KEYBOARD_DATA ) );
		}
	}

	c = *__c_next_char & 0xff;
	__c_next_char = __c_increment( __c_next_char );
	if( c != EOT ){
		c_putchar( c );
	}
	return c;
}

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
