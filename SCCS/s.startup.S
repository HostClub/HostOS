h29920
s 00002/00000/00220
d D 1.9 11/03/31 18:04:48 wrc 9 8
c added a cli at the protected mode entry point
e
s 00010/00005/00210
d D 1.8 07/04/03 14:20:18 wrc 8 7
c moved init calls into ifdef structure
e
s 00023/00023/00192
d D 1.7 07/02/01 09:32:32 wrc 7 6
c corrected comments in __in* and __out* routines
e
s 00002/00004/00213
d D 1.6 06/10/14 13:55:10 wrc 6 5
c sp1/sp2 compatability tweaks
e
s 00045/00004/00172
d D 1.5 06/10/14 13:10:25 wrc 5 4
c added __pause, mods for sp1/sp2 compatability
e
s 00013/00013/00163
d D 1.4 03/09/30 11:27:01 kar 4 3
c Update for installation of latest OS & utilities
e
s 00086/00030/00090
d D 1.3 02/05/16 15:14:53 kar 3 2
c Version 2: improvements based on my experience with 20013 506.
e
s 00021/00008/00099
d D 1.2 02/01/22 16:41:04 kar 2 1
c Add interrupt stuff, clean up code
e
s 00107/00000/00000
d D 1.1 02/01/15 16:18:25 kar 1 0
c date and time created 02/01/15 16:18:25 by kar
e
u
U
f e 0
t
T
I 1
/*
D 3
** SCCS ID:		%W%	%G%
E 3
I 3
** SCCS ID:	%W%	%G%
E 3
**
D 3
** File:		startup.S
E 3
I 3
** File:	startup.S
E 3
**
D 3
** Author:		Jon Coles
E 3
I 3
** Author:	Jon Coles
E 3
**
D 3
** Contributors:	Warren R. Carithers, K. Reek
E 3
I 3
** Contributor:	Warren R. Carithers, K. Reek
E 3
**
D 6
** Description:	Startup routine.
E 6
I 6
** Description:	SP1/SP2 startup code.
E 6
**
** This code prepares the various registers for execution of
** the program.  It sets up all the segment registers and the
** runtime stack.  By the time this code is running, we're in
** protected mode already.
**
** This code is intended to be assembled with as, not as86.
*/

I 3
#include "bootstrap.h"

E 3
/*
I 5
D 6
** Configuration definitions
E 6
I 6
** Configuration options - define in Makefile
E 6
**
**	CLEAR_BSS_SEGMENT	include code to clear all BSS space
**	SP2_CONFIG		enable SP2-specific startup variations
*/
D 6
#define	CLEAR_BSS_SEGMENT
// #define	SP2_CONFIG
E 6

/*
E 5
** A symbol for locating the beginning of the code.
*/
D 4
	.global begtext
E 4
I 4
	.globl begtext
E 4

	.text
begtext:

/*
** The entry point.
*/
D 4
	.global	_start
I 3
	.global	__bss_start,_end
E 4
I 4
	.globl	_start
D 5
	.globl	__bss_start,_end
E 5
E 4

E 3
_start:
I 9
	cli
E 9
D 3
	xorl	%edi, %edi	/* clear out SI and DI */
	xorl	%esi, %esi
E 3
I 3
	movb	$0x00, %al	/* re-enable NMIs (bootstrap */
D 4
	outb	%al, $0x70	/*   turned them off) */
E 4
I 4
	outb	$0x70		/*   turned them off) */
E 4
E 3

I 9

E 9
/*
D 3
** Set registers to point to the data segment.
E 3
I 3
** Set the data and stack segment registers (code segment register
** was set by the long jump that switched us into protected mode).
E 3
*/
	xorl	%eax, %eax	/* clear EAX */
D 3
	movw	$0x18, %ax	/* GDT entry #3 - data segment */
E 3
I 3
	movw	$GDT_DATA, %ax	/* GDT entry #3 - data segment */
E 3
	movw	%ax, %ds	/* for all four data segment registers */
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

D 3
	movw	$0x20, %ax	/* entry #4 is the stack segment */
E 3
I 3
	movw	$GDT_STACK, %ax	/* entry #4 is the stack segment */
E 3
	movw	%ax, %ss

D 3
	movl	$0x10000, %ebx	/* set up the system SP */
	movl	%ebx, %esp
E 3
I 3
	movl	$TARGET_STACK, %ebp	/* set up the system frame pointer */
	movl	%ebp, %esp	/* and stack pointer */
E 3

I 5
#ifdef CLEAR_BSS_SEGMENT

E 5
/*
D 3
** Flush the pre-fetch cache.  The MOV is here to prevent the
** jump from being optimized out.
E 3
I 3
** Zero the BSS segment
I 5
**
** These symbols are defined automatically by the linker.
E 5
E 3
*/
I 5
	.globl	__bss_start,_end

E 5
D 3
	stc
	jc	flush
	mov	%ax, %ax
flush:
	clc
E 3
I 3
	movl	$__bss_start,%edi
clearbss:
	movl	$0,(%edi)
	addl	$4,%edi
	cmpl	$_end,%edi
	jb	clearbss
E 3

I 5
#endif

I 8
#ifdef SP2_CONFIG

E 8
E 5
D 2
	call	c_io_init	// Initialize the console I/O system
E 2
I 2
D 3
	call	init		// Initialize the console I/O system
E 2
	call	main		// Call the user's main program
E 3
I 3
/*
D 5
** Initialize some things
E 5
I 5
D 8
** Initialize various devices
E 8
I 8
** Initialize devices
E 8
E 5
*/
D 5
	call	init_interrupts	/* set up the IDT and initialize the PIC */
E 5
I 5
D 8
	call	__init_interrupts	/* initialize the interrupt system */
E 5
	call	c_io_init	/* initialize the console I/O routines */
E 8
I 8
	call	_init		// initialize the OS
E 8
E 3

I 5
D 8
#ifdef SP2_CONFIG

E 8
E 5
I 3
/*
I 5
** Restore the first user process context
**
** WARNING:  must set things up so that the stack looks like
** we're returning to the context restore routine after handling
** an interrupt!!!
*/
	jmp	__isr_restore   // defined in isr_stubs.S

#else

/*
I 8
** Initialize various devices
*/
	call	__init_interrupts	/* initialize the interrupt system */
	call	c_io_init	/* initialize the console I/O routines */

/*
E 8
E 5
** Call the user's main program
*/
	call	main

/*
** Returned from the main function!  Stop execution.
*/
E 3
	cli			// Disable interrupts
	pushl	$message	// Print the message saying that
	pushl	$24		//   the main function returned.
	movl	$80,%eax	//   This goes to the lower right
	subl	$messagelen,%eax //  corner of the screen.
	pushl	%eax
	call	c_puts_at
die:	hlt			// Stop.
	jmp	die

message: .string "     ***** Returned from main! *****     "
messagelen = . - message - 1

I 5
#endif
E 5
I 3

E 3
/*
** Support routines needed for boot up and the console.
*/
ARG1	= 8			// Offset to 1st argument
ARG2	= 12			// Offset to 2nd argument

/*
D 2
** inport: read a single byte from the specified input port
**	int inport( int port );
E 2
I 2
D 3
** __inport: read a single byte from the specified input port
**	int __inport( int port );
E 3
I 3
** Name:	__inb, __inw, __inl
E 3
E 2
**
D 2
** outport: write a single byte to the specified output port
**	void outport( int port, int value );
E 2
I 2
D 3
** __outport: write a single byte to the specified output port
**	void __outport( int port, int value );
E 3
I 3
** Description: read a single byte, word, or longword from the specified
**		input port
** Arguments:	the port number
** Returns:	the byte, word, or longword value from the port (unused
**		high-order bytes are zeroed)
E 3
E 2
*/
D 2
	.globl	inport, outport
E 2
I 2
D 3
	.globl	__inport, __outport
E 3
I 3
D 4
	.global	__inb, __inw, __inl
E 4
I 4
	.globl	__inb, __inw, __inl
E 4
E 3
E 2

D 2
inport:
E 2
I 2
D 3
__inport:
E 3
I 3
__inb:
E 3
E 2
	enter	$0,$0
D 7
	xorl	%eax,%eax	// Clear the high order bytes of eax
	movl	ARG1(%ebp),%edx	// Move port number to edx
D 3
	inb	(%dx)		// Get a byte from the port in al (low
E 3
I 3
D 4
	inb	%dx,%al		// Get a byte from the port into al (low
E 4
I 4
	inb	(%dx)		// Get a byte from the port into al (low
E 4
E 3
	leave			//   byte of eax)
E 7
I 7
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inb	(%dx)		// Get a byte from the port into %al (low
	leave			//   byte of %eax)
E 7
	ret
I 3
__inw:
	enter	$0,$0
D 7
	xorl	%eax,%eax	// Clear the high order bytes of eax
	movl	ARG1(%ebp),%edx	// Move port number to edx
D 4
	inw	%dx,%ax		// Get a byte from the port into al (low
E 4
I 4
	inw	(%dx)		// Get a byte from the port into al (low
E 4
	leave			//   byte of eax)
E 7
I 7
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inw	(%dx)		// Get a word from the port into %ax (low
	leave			//   word of %eax)
E 7
	ret
__inl:
	enter	$0,$0
D 7
	xorl	%eax,%eax	// Clear the high order bytes of eax
	movl	ARG1(%ebp),%edx	// Move port number to edx
D 4
	inl	%dx,%eax	// Get a byte from the port into al (low
E 4
I 4
	inl	(%dx)		// Get a byte from the port into al (low
E 4
	leave			//   byte of eax)
E 7
I 7
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inl	(%dx)		// Get a longword from the port into %eax
	leave
E 7
	ret
E 3
	
D 2
outport:
E 2
I 2
D 3
__outport:
E 3
I 3
/*
** Name:	__outb, __outw, __outl
**
** Description: write a single byte, word, or longword to the specified
**		output port
** Arguments:	the port number, the value to write to it (unused high-order
**		bytes are ignored)
*/
D 4
	.global	__outb, __outw, __outl
E 4
I 4
	.globl	__outb, __outw, __outl
E 4
__outb:
E 3
E 2
	enter	$0,$0
D 7
	movl	ARG1(%ebp),%edx	// Get the port number into edx,
	movl	ARG2(%ebp),%eax	//   and the value into eax.
D 3
	outb	(%dx)		// Output that byte to the port.
E 3
I 3
D 4
	outb	%al,%dx		// Output that byte to the port.
E 4
I 4
	outb	(%dx)		// Output that byte to the port.
E 4
E 3
	leave
E 7
I 7
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outb	(%dx)		// Output that byte to the port
	leave			//   (only %al is sent)
E 7
	ret
I 3
__outw:
	enter	$0,$0
D 7
	movl	ARG1(%ebp),%edx	// Get the port number into edx,
	movl	ARG2(%ebp),%eax	//   and the value into eax.
D 4
	outw	%ax,%dx		// Output that byte to the port.
E 4
I 4
	outw	(%dx)		// Output that byte to the port.
E 4
	leave
E 7
I 7
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outw	(%dx)		// Output that word to the port.
	leave			//   (only %ax is sent)
E 7
	ret
__outl:
	enter	$0,$0
D 7
	movl	ARG1(%ebp),%edx	// Get the port number into edx,
	movl	ARG2(%ebp),%eax	//   and the value into eax.
D 4
	outl	%eax,%dx	// Output that byte to the port.
E 4
I 4
	outl	(%dx)		// Output that byte to the port.
E 7
I 7
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outl	(%dx)		// Output that longword to the port.
E 7
E 4
	leave
	ret
E 3
I 2

/*
** __get_flags: return the current processor flags
**	int __get_flags( void );
*/
D 3
	.globl	__get_flags
E 3
I 3
D 4
	.global	__get_flags
E 4
I 4
	.globl	__get_flags
E 4
E 3

__get_flags:
D 5
	enter	$0,$0
E 5
	pushfl			// Push flags on the stack,
	popl	%eax		//   and pop them into eax.
I 5
	ret

/*
** __pause: halt until something happens
**      void __pause( void );
*/
	.globl	__pause

__pause:
	enter	$0,$0
	sti
	hlt
E 5
	leave
	ret
E 2
E 1
