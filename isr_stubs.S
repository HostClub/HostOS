/*
** SCCS ID:	@(#)isr_stubs.S	1.7	03/31/11
**
** File:	isr_stubs.S
**
** Author:	K. Reek
**
** Contributor:	Jon Coles, Warren R. Carithers, Margaret Reek, and
**		numerous 4003-506 classes.
**
** Description:	Stubs for ISRs.
**
**	This module provides the stubs needed for interrupts to save
**	the machine state before calling the ISR.  All interrupts have
**	their own stub which pushes the interrupt number on the stack.
**	This makes it possible for a common ISR to determine which
**	interrupted occurred.
*/

#include "bootstrap.h"

/*
** Configuration options - define in Makefile
**
**	ISR_DEBUGGING_CODE	include context restore debugging code
*/

	.text

/*
** Macros for the isr stubs.  Some interrupts push an error code on
** the stack and others don't; for those that don't we simply push
** a zero so that cleaning up from either type is identical.
*/

#define	ISR(vector)			\
	.globl	__isr_##vector		; \
__isr_##vector:				; \
	pushl	$0			; \
	pushl	$vector			; \
	jmp	isr_save

#define	ERR_ISR(vector)		\
	.globl	__isr_##vector		; \
__isr_##vector:				; \
	pushl	$vector			; \
	jmp	isr_save

	.globl	__isr_table
	.globl	__isr_restore

/*
** This routine saves the machine state, calls the ISR, and then
** restores the machine state and returns from the interrupt.
**
********************************************************************
********************************************************************
** NOTE:  this code is highly application-specific, and will most **
** probably require modification to tailor it.                    **
**                                                                **
** Examples of mods:  switch to/from user stack, context switch   **
** changes, etc.                                                  **
********************************************************************
********************************************************************
*/

isr_save:

/*
** Begin by saving the state
**
** At this point, the stack looks like this:
**
**  esp ->  vector #		saved by the entry macro
**	    error code, or 0	saved by the hardware, or the entry macro
**	    saved EIP		saved by the hardware
**	    saved CS		saved by the hardware
**	    saved EFLAGS	saved by the hardware
*/
	pusha			// save E*X, ESP, EBP, ESI, EDI
	pushl	%ds		// save segment registers
	pushl	%es
	pushl	%fs
	pushl	%gs
	pushl	%ss

/*
** Stack contents (all 32-bit longwords) and offsets from ESP:
**
**   SS GS FS ES DS EDI ESI EBP ESP EBX EDX ECX EAX vec cod EIP CS EFL
**   0  4  8  12 16 20  24  28  32  36  40  44  48  52  56  60  64 68
**
** Note that the saved ESP is the contents before the PUSHA.
**
** Set up parameters for the ISR call.
*/

/*
** MODS for 20103 SP2
*/
	.globl	_current
	.globl	_system_esp
	.globl	_system_time

	movl	_current, %ebx	// save context pointer for current proc
	movl	%esp, (%ebx)	// (must be first field in PCB)

	movl	52(%esp),%eax	// get vector number and error code
	movl	56(%esp),%ebx

	movl	_system_esp, %esp	// switch to OS stack

	pushl	%ebx		// put them on the top of the stack ...
	pushl	%eax		// ... as parameters for the ISR

/*
** END MODS for 20103 SP2
*/

/*
** Call the ISR
*/
	movl	__isr_table(,%eax,4),%ebx
	call	*%ebx
	addl	$8,%esp		// pop the two parameters

/*
** Context restore begins here
*/

__isr_restore:

/*
** MODS for 20103 SP2
*/

	movl	_current, %ebx	// restore ESP for (new?) process
	movl	(%ebx), %esp

/*
** END MODS for 20103 SP2
*/

#ifdef ISR_DEBUGGING_CODE
/*
** DEBUGGING CODE PART 1
**
** This code will execute during each context restore, and 
** should be modified to print out whatever debugging information
** is desired.
**
** By default, it prints out the CPU context being restored; it
** relies on the standard save sequence (see above).
*/

/*
** MODS for 20103 SP2
*/

	xorl	%eax, %eax	// clear upper 24 bits
	movb	25(%ebx), %al	// priority of current process
	pushl	%eax
	movl	8(%ebx), %eax	// pid of current process
	pushl	%eax
	movl	_system_time, %eax	// current time
	pushl	%eax
	pushl	$fmt2
	pushl	$5
	pushl	$0
	call	c_printf_at
	addl	$24, %esp

/*
** END MODS for 20103 SP2
*/

	pushl	$fmt
	pushl	$1
	pushl	$0
	call	c_printf_at
	addl	$12,%esp
/*
** END OF DEBUGGING CODE PART 1
*/
#endif

/*
** Restore the context.
*/
	popl	%ss		// restore the segment registers
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popa			// restore others
	addl	$8, %esp	// discard the error code and vector
	iret			// and return

#ifdef ISR_DEBUGGING_CODE
/*
** DEBUGGING CODE PART 2
*/
fmt:	.ascii	" ss=%08x  gs=%08x  fs=%08x  es=%08x  ds=%08x\n"
	.ascii	"edi=%08x esi=%08x ebp=%08x esp=%08x ebx=%08x\n"
	.ascii	"edx=%08x ecx=%08x eax=%08x vec=%08x cod=%08x\n"
	.string	"eip=%08x  cs=%08x efl=%08x\n"

/*
** MODS for 20103 SP2
*/

fmt2:	.string	"time %08x  pid %3d  priority %02x\n"

/*
** END MODS for 20103 SP2
*/

/*
** END OF DEBUGGING CODE PART 2
*/
#endif

/*
** Here we generate the individual stubs for each interrupt.
*/
ISR(0x00);	ISR(0x01);	ISR(0x02);	ISR(0x03);
ISR(0x04);	ISR(0x05);	ISR(0x06);	ISR(0x07);
ERR_ISR(0x08);	ISR(0x09);	ERR_ISR(0x0a);	ERR_ISR(0x0b);
ERR_ISR(0x0c);	ERR_ISR(0x0d);	ERR_ISR(0x0e);	ISR(0x0f);
ISR(0x10);	ERR_ISR(0x11);	ISR(0x12);	ISR(0x13);
ISR(0x14);	ISR(0x15);	ISR(0x16);	ISR(0x17);
ISR(0x18);	ISR(0x19);	ISR(0x1a);	ISR(0x1b);
ISR(0x1c);	ISR(0x1d);	ISR(0x1e);	ISR(0x1f);
ISR(0x20);	ISR(0x21);	ISR(0x22);	ISR(0x23);
ISR(0x24);	ISR(0x25);	ISR(0x26);	ISR(0x27);
ISR(0x28);	ISR(0x29);	ISR(0x2a);	ISR(0x2b);
ISR(0x2c);	ISR(0x2d);	ISR(0x2e);	ISR(0x2f);
ISR(0x30);	ISR(0x31);	ISR(0x32);	ISR(0x33);
ISR(0x34);	ISR(0x35);	ISR(0x36);	ISR(0x37);
ISR(0x38);	ISR(0x39);	ISR(0x3a);	ISR(0x3b);
ISR(0x3c);	ISR(0x3d);	ISR(0x3e);	ISR(0x3f);
ISR(0x40);	ISR(0x41);	ISR(0x42);	ISR(0x43);
ISR(0x44);	ISR(0x45);	ISR(0x46);	ISR(0x47);
ISR(0x48);	ISR(0x49);	ISR(0x4a);	ISR(0x4b);
ISR(0x4c);	ISR(0x4d);	ISR(0x4e);	ISR(0x4f);
ISR(0x50);	ISR(0x51);	ISR(0x52);	ISR(0x53);
ISR(0x54);	ISR(0x55);	ISR(0x56);	ISR(0x57);
ISR(0x58);	ISR(0x59);	ISR(0x5a);	ISR(0x5b);
ISR(0x5c);	ISR(0x5d);	ISR(0x5e);	ISR(0x5f);
ISR(0x60);	ISR(0x61);	ISR(0x62);	ISR(0x63);
ISR(0x64);	ISR(0x65);	ISR(0x66);	ISR(0x67);
ISR(0x68);	ISR(0x69);	ISR(0x6a);	ISR(0x6b);
ISR(0x6c);	ISR(0x6d);	ISR(0x6e);	ISR(0x6f);
ISR(0x70);	ISR(0x71);	ISR(0x72);	ISR(0x73);
ISR(0x74);	ISR(0x75);	ISR(0x76);	ISR(0x77);
ISR(0x78);	ISR(0x79);	ISR(0x7a);	ISR(0x7b);
ISR(0x7c);	ISR(0x7d);	ISR(0x7e);	ISR(0x7f);
ISR(0x80);	ISR(0x81);	ISR(0x82);	ISR(0x83);
ISR(0x84);	ISR(0x85);	ISR(0x86);	ISR(0x87);
ISR(0x88);	ISR(0x89);	ISR(0x8a);	ISR(0x8b);
ISR(0x8c);	ISR(0x8d);	ISR(0x8e);	ISR(0x8f);
ISR(0x90);	ISR(0x91);	ISR(0x92);	ISR(0x93);
ISR(0x94);	ISR(0x95);	ISR(0x96);	ISR(0x97);
ISR(0x98);	ISR(0x99);	ISR(0x9a);	ISR(0x9b);
ISR(0x9c);	ISR(0x9d);	ISR(0x9e);	ISR(0x9f);
ISR(0xa0);	ISR(0xa1);	ISR(0xa2);	ISR(0xa3);
ISR(0xa4);	ISR(0xa5);	ISR(0xa6);	ISR(0xa7);
ISR(0xa8);	ISR(0xa9);	ISR(0xaa);	ISR(0xab);
ISR(0xac);	ISR(0xad);	ISR(0xae);	ISR(0xaf);
ISR(0xb0);	ISR(0xb1);	ISR(0xb2);	ISR(0xb3);
ISR(0xb4);	ISR(0xb5);	ISR(0xb6);	ISR(0xb7);
ISR(0xb8);	ISR(0xb9);	ISR(0xba);	ISR(0xbb);
ISR(0xbc);	ISR(0xbd);	ISR(0xbe);	ISR(0xbf);
ISR(0xc0);	ISR(0xc1);	ISR(0xc2);	ISR(0xc3);
ISR(0xc4);	ISR(0xc5);	ISR(0xc6);	ISR(0xc7);
ISR(0xc8);	ISR(0xc9);	ISR(0xca);	ISR(0xcb);
ISR(0xcc);	ISR(0xcd);	ISR(0xce);	ISR(0xcf);
ISR(0xd0);	ISR(0xd1);	ISR(0xd2);	ISR(0xd3);
ISR(0xd4);	ISR(0xd5);	ISR(0xd6);	ISR(0xd7);
ISR(0xd8);	ISR(0xd9);	ISR(0xda);	ISR(0xdb);
ISR(0xdc);	ISR(0xdd);	ISR(0xde);	ISR(0xdf);
ISR(0xe0);	ISR(0xe1);	ISR(0xe2);	ISR(0xe3);
ISR(0xe4);	ISR(0xe5);	ISR(0xe6);	ISR(0xe7);
ISR(0xe8);	ISR(0xe9);	ISR(0xea);	ISR(0xeb);
ISR(0xec);	ISR(0xed);	ISR(0xee);	ISR(0xef);
ISR(0xf0);	ISR(0xf1);	ISR(0xf2);	ISR(0xf3);
ISR(0xf4);	ISR(0xf5);	ISR(0xf6);	ISR(0xf7);
ISR(0xf8);	ISR(0xf9);	ISR(0xfa);	ISR(0xfb);
ISR(0xfc);	ISR(0xfd);	ISR(0xfe);	ISR(0xff);

	.data

/*
** This table contains the addresses where each of the preceding
** stubs begins.  This information is needed to initialize the
** Interrupt Descriptor Table in support.c
*/
	.globl	__isr_stub_table
__isr_stub_table:
	.long	__isr_0x00, __isr_0x01, __isr_0x02, __isr_0x03
	.long	__isr_0x04, __isr_0x05, __isr_0x06, __isr_0x07
	.long	__isr_0x08, __isr_0x09, __isr_0x0a, __isr_0x0b
	.long	__isr_0x0c, __isr_0x0d, __isr_0x0e, __isr_0x0f
	.long	__isr_0x10, __isr_0x11, __isr_0x12, __isr_0x13
	.long	__isr_0x14, __isr_0x15, __isr_0x16, __isr_0x17
	.long	__isr_0x18, __isr_0x19, __isr_0x1a, __isr_0x1b
	.long	__isr_0x1c, __isr_0x1d, __isr_0x1e, __isr_0x1f
	.long	__isr_0x20, __isr_0x21, __isr_0x22, __isr_0x23
	.long	__isr_0x24, __isr_0x25, __isr_0x26, __isr_0x27
	.long	__isr_0x28, __isr_0x29, __isr_0x2a, __isr_0x2b
	.long	__isr_0x2c, __isr_0x2d, __isr_0x2e, __isr_0x2f
	.long	__isr_0x30, __isr_0x31, __isr_0x32, __isr_0x33
	.long	__isr_0x34, __isr_0x35, __isr_0x36, __isr_0x37
	.long	__isr_0x38, __isr_0x39, __isr_0x3a, __isr_0x3b
	.long	__isr_0x3c, __isr_0x3d, __isr_0x3e, __isr_0x3f
	.long	__isr_0x40, __isr_0x41, __isr_0x42, __isr_0x43
	.long	__isr_0x44, __isr_0x45, __isr_0x46, __isr_0x47
	.long	__isr_0x48, __isr_0x49, __isr_0x4a, __isr_0x4b
	.long	__isr_0x4c, __isr_0x4d, __isr_0x4e, __isr_0x4f
	.long	__isr_0x50, __isr_0x51, __isr_0x52, __isr_0x53
	.long	__isr_0x54, __isr_0x55, __isr_0x56, __isr_0x57
	.long	__isr_0x58, __isr_0x59, __isr_0x5a, __isr_0x5b
	.long	__isr_0x5c, __isr_0x5d, __isr_0x5e, __isr_0x5f
	.long	__isr_0x60, __isr_0x61, __isr_0x62, __isr_0x63
	.long	__isr_0x64, __isr_0x65, __isr_0x66, __isr_0x67
	.long	__isr_0x68, __isr_0x69, __isr_0x6a, __isr_0x6b
	.long	__isr_0x6c, __isr_0x6d, __isr_0x6e, __isr_0x6f
	.long	__isr_0x70, __isr_0x71, __isr_0x72, __isr_0x73
	.long	__isr_0x74, __isr_0x75, __isr_0x76, __isr_0x77
	.long	__isr_0x78, __isr_0x79, __isr_0x7a, __isr_0x7b
	.long	__isr_0x7c, __isr_0x7d, __isr_0x7e, __isr_0x7f
	.long	__isr_0x80, __isr_0x81, __isr_0x82, __isr_0x83
	.long	__isr_0x84, __isr_0x85, __isr_0x86, __isr_0x87
	.long	__isr_0x88, __isr_0x89, __isr_0x8a, __isr_0x8b
	.long	__isr_0x8c, __isr_0x8d, __isr_0x8e, __isr_0x8f
	.long	__isr_0x90, __isr_0x91, __isr_0x92, __isr_0x93
	.long	__isr_0x94, __isr_0x95, __isr_0x96, __isr_0x97
	.long	__isr_0x98, __isr_0x99, __isr_0x9a, __isr_0x9b
	.long	__isr_0x9c, __isr_0x9d, __isr_0x9e, __isr_0x9f
	.long	__isr_0xa0, __isr_0xa1, __isr_0xa2, __isr_0xa3
	.long	__isr_0xa4, __isr_0xa5, __isr_0xa6, __isr_0xa7
	.long	__isr_0xa8, __isr_0xa9, __isr_0xaa, __isr_0xab
	.long	__isr_0xac, __isr_0xad, __isr_0xae, __isr_0xaf
	.long	__isr_0xb0, __isr_0xb1, __isr_0xb2, __isr_0xb3
	.long	__isr_0xb4, __isr_0xb5, __isr_0xb6, __isr_0xb7
	.long	__isr_0xb8, __isr_0xb9, __isr_0xba, __isr_0xbb
	.long	__isr_0xbc, __isr_0xbd, __isr_0xbe, __isr_0xbf
	.long	__isr_0xc0, __isr_0xc1, __isr_0xc2, __isr_0xc3
	.long	__isr_0xc4, __isr_0xc5, __isr_0xc6, __isr_0xc7
	.long	__isr_0xc8, __isr_0xc9, __isr_0xca, __isr_0xcb
	.long	__isr_0xcc, __isr_0xcd, __isr_0xce, __isr_0xcf
	.long	__isr_0xd0, __isr_0xd1, __isr_0xd2, __isr_0xd3
	.long	__isr_0xd4, __isr_0xd5, __isr_0xd6, __isr_0xd7
	.long	__isr_0xd8, __isr_0xd9, __isr_0xda, __isr_0xdb
	.long	__isr_0xdc, __isr_0xdd, __isr_0xde, __isr_0xdf
	.long	__isr_0xe0, __isr_0xe1, __isr_0xe2, __isr_0xe3
	.long	__isr_0xe4, __isr_0xe5, __isr_0xe6, __isr_0xe7
	.long	__isr_0xe8, __isr_0xe9, __isr_0xea, __isr_0xeb
	.long	__isr_0xec, __isr_0xed, __isr_0xee, __isr_0xef
	.long	__isr_0xf0, __isr_0xf1, __isr_0xf2, __isr_0xf3
	.long	__isr_0xf4, __isr_0xf5, __isr_0xf6, __isr_0xf7
	.long	__isr_0xf8, __isr_0xf9, __isr_0xfa, __isr_0xfb
	.long	__isr_0xfc, __isr_0xfd, __isr_0xfe, __isr_0xff
