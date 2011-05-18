/*
** SCCS ID:	@(#)startup.S	1.9	03/31/11
**
** File:	startup.S
**
** Author:	Jon Coles
**
** Contributor:	Warren R. Carithers, K. Reek
**
** Description:	SP1/SP2 startup code.
**
** This code prepares the various registers for execution of
** the program.  It sets up all the segment registers and the
** runtime stack.  By the time this code is running, we're in
** protected mode already.
**
** This code is intended to be assembled with as, not as86.
*/

#include "bootstrap.h"

/*
** Configuration options - define in Makefile
**
**	CLEAR_BSS_SEGMENT	include code to clear all BSS space
**	SP2_CONFIG		enable SP2-specific startup variations
*/

/*
** A symbol for locating the beginning of the code.
*/
	.globl begtext

	.text
begtext:

/*
** The entry point.
*/
	.globl	_start

_start:
	cli
	movb	$0x00, %al	/* re-enable NMIs (bootstrap */
	outb	$0x70		/*   turned them off) */

/* Check to see if this is the BSP */
	movl (IA32_APIC_BASE), %eax
	andl $BSP_MASK, %eax
	jnz bsp_startup


	xorl	%eax, %eax	/* clear EAX */
	movw	$GDT_DATA, %ax	/* GDT entry #3 - data segment */
	movw	%ax, %ds	/* for all four data segment registers */
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	movw	$GDT_STACK, %ax	/* entry #4 is the stack segment */
	movw	%ax, %ss

	movl	0x20000, %ebp	/* set up the system frame pointer */
	movl	%ebp, %esp	/* and stack pointer */


	//call _ap_init
	movl $22, 0x9000
	jmp .


bsp_startup:

/*
** Set the data and stack segment registers (code segment register
** was set by the long jump that switched us into protected mode).
*/
	xorl	%eax, %eax	/* clear EAX */
	movw	$GDT_DATA, %ax	/* GDT entry #3 - data segment */
	movw	%ax, %ds	/* for all four data segment registers */
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	movw	$GDT_STACK, %ax	/* entry #4 is the stack segment */
	movw	%ax, %ss

	movl	$TARGET_STACK, %ebp	/* set up the system frame pointer */
	movl	%ebp, %esp	/* and stack pointer */


	movl $20, 0x9000


#ifdef CLEAR_BSS_SEGMENT

/*
** Zero the BSS segment
**
** These symbols are defined automatically by the linker.
*/
	.globl	__bss_start,_end

	movl	$__bss_start,%edi
clearbss:
	movl	$0,(%edi)
	addl	$4,%edi
	cmpl	$_end,%edi
	jb	clearbss

#endif

#ifdef SP2_CONFIG

/*
** Initialize devices
*/
	call	_init		// initialize the OS

/*
** Restore the first user process context
**
** WARNING:  must set things up so that the stack looks like
** we're returning to the context restore routine after handling
** an interrupt!!!
*/
	jmp	__isr_restore   // defined in isr_stubs.S

#else

/*
** Initialize various devices
*/
	call	__init_interrupts	/* initialize the interrupt system */
	call	c_io_init	/* initialize the console I/O routines */

/*
** Call the user's main program
*/
	call	main

/*
** Returned from the main function!  Stop execution.
*/
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

#endif

/*
** Support routines needed for boot up and the console.
*/
ARG1	= 8			// Offset to 1st argument
ARG2	= 12			// Offset to 2nd argument

/*
** Name:	__inb, __inw, __inl
**
** Description: read a single byte, word, or longword from the specified
**		input port
** Arguments:	the port number
** Returns:	the byte, word, or longword value from the port (unused
**		high-order bytes are zeroed)
*/
	.globl	__inb, __inw, __inl

__inb:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inb	(%dx)		// Get a byte from the port into %al (low
	leave			//   byte of %eax)
	ret
__inw:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inw	(%dx)		// Get a word from the port into %ax (low
	leave			//   word of %eax)
	ret
__inl:
	enter	$0,$0
	xorl	%eax,%eax	// Clear the high order bytes of %eax
	movl	ARG1(%ebp),%edx	// Move port number to %edx
	inl	(%dx)		// Get a longword from the port into %eax
	leave
	ret
	
/*
** Name:	__outb, __outw, __outl
**
** Description: write a single byte, word, or longword to the specified
**		output port
** Arguments:	the port number, the value to write to it (unused high-order
**		bytes are ignored)
*/
	.globl	__outb, __outw, __outl
__outb:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outb	(%dx)		// Output that byte to the port
	leave			//   (only %al is sent)
	ret
__outw:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outw	(%dx)		// Output that word to the port.
	leave			//   (only %ax is sent)
	ret
__outl:
	enter	$0,$0
	movl	ARG1(%ebp),%edx	// Get the port number into %edx,
	movl	ARG2(%ebp),%eax	//   and the value into %eax
	outl	(%dx)		// Output that longword to the port.
	leave
	ret

/*
** __get_flags: return the current processor flags
**	int __get_flags( void );
*/
	.globl	__get_flags

__get_flags:
	pushfl			// Push flags on the stack,
	popl	%eax		//   and pop them into eax.
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
	leave
	ret
