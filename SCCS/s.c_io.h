h01284
s 00033/00018/00208
d D 1.6 02/05/16 15:14:52 kar 6 5
c Version 2: improvements based on my experience with 20013 506.
e
s 00184/00018/00042
d D 1.5 02/01/22 16:41:04 kar 5 4
c Add interrupt stuff, clean up code
e
s 00049/00037/00011
d D 1.4 02/01/15 16:13:24 kar 4 3
c Remove special types, convert comments to C style
e
s 00005/00000/00043
d D 1.3 02/01/02 14:20:19 kar 3 2
c Add keyboard input
e
s 00003/00003/00040
d D 1.2 02/01/02 09:22:54 kar 2 1
c scrolling completed
e
s 00043/00000/00000
d D 1.1 01/12/22 07:57:32 kar 1 0
c date and time created 01/12/22 07:57:32 by kar
e
u
U
f e 0
t
T
I 1
D 4
//
// File:	c_io.h
// Author:	K. Reek
// Description:	Prototypes for console I/O routines
// SCCS ID:	%W%	%G%
//	
E 4
I 4
/*
D 5
** sccs id: %W%        %G%
E 5
I 5
D 6
** sccs id:		%W%	%G%
E 6
I 6
** SCCS ID:	%W%	%G%
E 6
E 5
**
D 6
** File:		c_io.h
E 6
I 6
** File:	c_io.h
E 6
**
D 6
** Author:		K. Reek
E 6
I 6
** Author:	K. Reek
E 6
**
D 6
** Contributors:	Jon Coles, Warren R. Carithers
E 6
I 6
** Contributor:	Jon Coles, Warren R. Carithers
E 6
**
D 5
** Description:		Declarations for console I/O routines
E 5
I 5
D 6
** Description:		Declarations and descriptions of console I/O routines
E 6
I 6
** Description:	Declarations and descriptions of console I/O routines
E 6
E 5
**
I 5
**	These routines provide a rudimentary capability for printing to
**	the screen and reading from the keyboard.  
**
** Screen output:
**	There are two families of functions.  The first provides a window
**	that behaves in the usual manner: writes extending beyond the right
**	edge of the window wrap around to the next line, the top line
**	scrolls off the window to make room for new lines at the bottom.
**	However, you may choose what part of the screen contains this
**	scrolling window.  This allows you to print some text at fixed
**	locations on the screen while the rest of the screen scrolls.
**
**	The second family allows for printing at fixed locations on the
**	screen.  No scrolling or line wrapping are done for these functions.
**	It is not intended that these functions be used to write in the
**	scrolling area of the screen.
**
**	In both sets of functions, the (x,y) coordinates are interpreted
**	as (column,row), with the upper left corner of the screen being
**	(0,0) and the lower right corner being (79,24).
**
**	The printf provided in both sets of functions has the same
**	conversion capabilities.  Format codes are of the form:
**
**		%-0WC
**
**	where "-", "0", and "W" are all optional:
**	  "-" is the left-adjust flag (default is right-adjust)
**	  "0" is the zero-fill flag (default is space-fill)
**	  "W" is a number specifying the minimum field width (default: 1 )
**	and "C" is the conversion type, which must be one of these:
**	  "c" print a single character
**	  "s" print a null-terminated string
**	  "d" print an integer as a decimal value
**	  "x" print an integer as a hexadecimal value
**	  "o" print an integer as a octal value
**
** Keyboard input:
**	Two functions are provided: getting a single character and getting
**	a newline-terminated line.  A third function returns a count of
**	the number of characters available for immediate reading. 
**	No conversions are provided (yet).
E 5
*/
E 4

D 4
//
// Initialize the screen driver
//
E 4
I 4
#ifndef _C_IO_H_
#define _C_IO_H_

/*
D 5
** Initialize the screen driver
E 5
I 5
** Name:	c_io_init
I 6
**
E 6
** Description:	Initializes the I/O routines.  This is called by the
**		standalone loader so you need not call it.
E 5
*/
E 4
void c_io_init( void );

I 5
/*****************************************************************************
**
** SCROLLING OUTPUT ROUTINES
**
**	Each operation begins at the current cursor position and advances
**	it.  If a newline is output, the reminder of that line is cleared.
**	Output extending past the end of the line is wrapped.  If the
**	cursor is moved below the scrolling region's bottom edge, scrolling
**	is delayed until the next output is produced.
*/

E 5
D 4
//
// Set the scrolling region (by default, it is the entire screen)
//
void c_setscroll( uint_t min_x, uint_t min_y, uint_t max_x, uint_t max_y );
E 4
I 4
/*
D 5
** Set the scrolling region (by default, it is the entire screen)
E 5
I 5
** Name:	c_setscroll
D 6
** Arguments:	coordinates of upper-left and lower-right corners of region
E 6
I 6
**
E 6
** Description:	This sets the scrolling region to be the area defined by
**		the arguments.  The remainder of the screen does not scroll
**		and may be used to display data you do not want to move.
**		By default, the scrolling region is the entire screen .
I 6
** Arguments:	coordinates of upper-left and lower-right corners of region
E 6
E 5
*/
void c_setscroll( unsigned int min_x, unsigned int min_y,
		  unsigned int max_x, unsigned int max_y );
E 4

D 4
//
// Routines to do output in the scrolling region.  Each output operation
// begins at the cursor and advances it.  Coordinates are relative to the
// scrolling region: 0,0 is the upper left corner of the region.
//
// c_moveto moves the cursor.
//
// c_scroll scrolls the region up by the given number of lines.
//
// c_clearscroll clears the scrolling region and sets the cursor at 0,0.
//
D 2
void c_putchar( ushort_t c );
E 2
I 2
void c_putchar( uint_t c );
E 4
I 4
/*
D 5
** Routines to do output in the scrolling region.  Each output operation
** begins at the cursor and advances it.  Coordinates are relative to the
** scrolling region: 0,0 is the upper left corner of the region.
**
** c_moveto moves the cursor.
**
** c_scroll scrolls the region up by the given number of lines.
**
** c_clearscroll clears the scrolling region and sets the cursor at 0,0.
E 5
I 5
** Name:	c_moveto
D 6
** Arguments:	desired cursor position
E 6
I 6
**
E 6
** Description:	Moves the cursor to the specified position. (0,0) indicates
**		the upper left corner of the scrolling region.  Subsequent
**		output will begin at the cursor position.
I 6
** Arguments:	desired cursor position
E 6
E 5
*/
I 5
void c_moveto( unsigned int x, unsigned int y );

/*
** Name:	c_putchar
D 6
** Arguments:	the character to be printed
E 6
I 6
**
E 6
** Description:	Prints a single character.
I 6
** Arguments:	the character to be printed
E 6
*/
E 5
void c_putchar( unsigned int c );
I 5

/*
** Name:	c_puts
D 6
** Arguments:	pointer to a null-terminated string
E 6
I 6
**
E 6
** Description:	Prints the characters in the string up to but not including
**		the terminating null byte.
I 6
** Arguments:	pointer to a null-terminated string
E 6
*/
E 5
E 4
E 2
void c_puts( char *str );
D 4
void c_moveto( uint_t x, uint_t y );
void c_scroll( uint_t lines );
E 4
I 4
D 5
void c_moveto( unsigned int x, unsigned int y );
void c_scroll( unsigned int lines );
E 5
I 5

/*
** Name:	c_printf
D 6
** Arguments:	printf-style format and optional values
E 6
I 6
**
E 6
** Description:	Limited form of printf (see the beginning of this file for
**		a list of what is implemented).
I 6
** Arguments:	printf-style format and optional values
E 6
*/
E 5
E 4
void c_printf( char *fmt, ... );
I 5

/*
** Name:	c_scroll
D 6
** Arguments:	number of lines
E 6
I 6
**
E 6
** Description:	Scroll the scrolling region up by the given number of lines.
**		The output routines scroll automatically so normally you
**		do not need to call this routine yourself.
I 6
** Arguments:	number of lines
E 6
*/
void c_scroll( unsigned int lines );

/*
** Name:	c_clearscroll
I 6
**
E 6
** Description:	Clears the entire scrolling region to blank spaces, and
**		moves the cursor to (0,0).
*/
E 5
void c_clearscroll( void );

I 5
/*****************************************************************************
**
** NON-SCROLLING OUTPUT ROUTINES
**
**	Coordinates are relative to the entire screen: (0,0) is the upper
**	left corner.  There is no line wrap or scrolling.
*/

E 5
D 4
//
// Routines to do output to the entire screen.  Coordinates are relative to
// the entire screen: 0,0 is the upper left corner.
//
D 2
void c_putchar_at( uint_t x, uint_t y, ushort_t c );
void c_puts_at( ushort_t x, ushort_t y, char *str );
E 2
I 2
void c_putchar_at( uint_t x, uint_t y, uint_t c );
void c_puts_at( uint_t x, uint_t y, char *str );
E 2
void c_printf_at( uint_t x, uint_t y, char *fmt, ... );
E 4
I 4
/*
D 5
** Routines to do output to the entire screen.  Coordinates are relative to
** the entire screen: 0,0 is the upper left corner.
E 5
I 5
** Name:	c_putchar_at
D 6
** Arguments:	coordinates, the character to be printed
E 6
I 6
**
E 6
** Description:	Prints the given character.  If a newline is printed,
**		the rest of the line is cleared.  If this happens to the
**		left of the scrolling region, the clearing stops when the
**		region is reached.  If this happens inside the scrolling
**		region, the clearing stops when the edge of the region
**		is reached.
I 6
** Arguments:	coordinates, the character to be printed
E 6
E 5
*/
void c_putchar_at( unsigned int x, unsigned int y, unsigned int c );
I 5

/*
** Name:	c_puts_at
D 6
** Arguments:	coordinates, null-terminated string to be printed
E 6
I 6
**
E 6
** Description:	Prints the given string.  c_putchar_at is used to print
**		the individual characters; see that description for details.
I 6
** Arguments:	coordinates, null-terminated string to be printed
E 6
*/
E 5
void c_puts_at( unsigned int x, unsigned int y, char *str );
I 5

/*
** Name:	c_printf_at
D 6
** Arguments:	coordinates, printf-style format, optional values
E 6
I 6
**
E 6
** Description:	Limited form of printf (see the beginning of this file for
**		a list of what is implemented).
I 6
** Arguments:	coordinates, printf-style format, optional values
E 6
*/
E 5
void c_printf_at( unsigned int x, unsigned int y, char *fmt, ... );
I 5

/*
** Name:	c_clearscreen
I 6
**
E 6
** Description:	This function clears the entire screen, including the
**		scrolling region.
*/
E 5
E 4
void c_clearscreen( void );
I 3

I 5
/*****************************************************************************
**
** INPUT ROUTINES
**
**	When interrupts are enabled, a keyboard ISR collects keystrokes
**	and saves them until the program calls for them.  If the input
**	queue fills, additional characters are silently discarded.
**	When interrupts are not enabled, keystrokes made when no input
**	routines have been **	called are lost.  This can cause errors in
**	the input translation because the states of the Shift and Ctrl keys
**	may not be tracked accurately.  If interrupts are disabled, the user
**	is advised to refrain from typing anything except when the program is
**	waiting for input.
*/

E 5
D 4
//
// Routine to read a character from the keyboard.
//
E 4
I 4
/*
D 5
** Routine to read a character from the keyboard.
E 5
I 5
** Name:	c_getchar
D 6
** Returns:	The next character typed on the keyboard.
E 6
I 6
**
E 6
** Description:	If the character is not immediately available, the function
**		waits until the character arrives.
I 6
** Returns:	The next character typed on the keyboard.
E 6
E 5
*/
E 4
int c_getchar( void );
I 4

I 5
/*
** Name:	c_gets
D 6
** Arguments:	pointer to input buffer, size of buffer
** Returns:	count of the number of characters read
E 6
I 6
**
E 6
** Description:	This function reads a newline-terminated line from the
**		keyboard.  c_getchar is used to obtain the characters;
**		see that description for more details.  The function
**		returns when:
**			a newline is entered (this is stored in the buffer)
**			ctrl-D is entered (not stored in the buffer)
**			the buffer becomes full.
**		The buffer is null-terminated in all cases.
I 6
** Arguments:	pointer to input buffer, size of buffer
** Returns:	count of the number of characters read
E 6
*/
int c_gets( char *buffer, unsigned int size );

/*
** Name:	c_input_queue
D 6
** Returns:	number of characters in the input queue
E 6
I 6
**
E 6
** Description:	This function lets the program determine whether there
**		is input available.  This determines whether or not a call
**		to c_getchar would block.
I 6
** Returns:	number of characters in the input queue
E 6
*/
int c_input_queue( void );

E 5
#endif
E 4
E 3
E 1
