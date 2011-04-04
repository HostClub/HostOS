/*
** SCCS ID:	@(#)x86arch.h	1.5	03/29/11
**
** File:	x86arch.h
**
** Author:	Warren R. Carithers
**
** Contributor:	K. Reek
**
** Description:	Definitions of constants and macros for use
**		with the x86 architecture and registers.
**
*/

#ifndef _X86ARCH_H_
#define	_X86ARCH_H_

/*
** Video stuff
*/
#define	VIDEO_BASE_ADDR		0xB8000

/*
** Memory management
*/
#define	SEG_PRESENT	0x80
#define	SEG_PL_0	0x00
#define	SEG_PL_1	0x20
#define	SEG_PL_2	0x40
#define	SEG_PL_3	0x50
#define	SEG_SYSTEM	0x00
#define	SEG_NON_SYSTEM	0x10
#define	SEG_32BIT	0x04
#define	DESC_IGATE	0x06

/*
** Exceptions
*/
#define	N_EXCEPTIONS              256

/*
** Bit definitions in registers
**
** See IA-32 Intel Architecture SW Dev. Manual, Volume 3: System
** Programming Guide, page 2-8.
*/

/*
** EFLAGS
*/
#define	EFLAGS_RSVD	0xffc00000	/* reserved */
#define	EFLAGS_MB0	0x00008020	/* must be zero */
#define	EFLAGS_MB1	0x00000002	/* must be 1 */

#define	EFLAGS_ID	0x00200000
#define	EFLAGS_VIP	0x00100000
#define	EFLAGS_VIF	0x00080000
#define	EFLAGS_AC	0x00040000
#define	EFLAGS_VM	0x00020000
#define	EFLAGS_RF	0x00010000
#define	EFLAGS_NT	0x00004000
#define	EFLAGS_IOPL	0x00003000
#define	EFLAGS_OF	0x00000800
#define	EFLAGS_DF	0x00000400
#define	EFLAGS_IF	0x00000200
#define	EFLAGS_TF	0x00000100
#define	EFLAGS_SF	0x00000080
#define	EFLAGS_ZF	0x00000040
#define	EFLAGS_AF	0x00000010
#define	EFLAGS_PF	0x00000004
#define	EFLAGS_CF	0x00000001

/*
** CR0, CR1, CR2, CR3, CR4
**
** IA-32 V3, page 2-12.
*/
#define	CR0_RSVD	0x1ffaffc0
#define CR0_PG		0x80000000
#define CR0_CD		0x40000000
#define CR0_NW		0x20000000
#define CR0_AM		0x00040000
#define CR0_WP		0x00010000
#define CR0_NE		0x00000020
#define CR0_ET		0x00000010
#define CR0_TS		0x00000008
#define CR0_EM		0x00000004
#define CR0_MP		0x00000002
#define CR0_PE		0x00000001
#define	CR1_RSVD	0xffffffff
#define	CR2_RSVD	0x00000000
#define	CR2_PF_LIN_ADDR	0xffffffff
#define	CR3_RSVD	0x00000fe7
#define CR3_PD_BASE	0xfffff000
#define CR3_PCD		0x00000010
#define CR3_PWT		0x00000008
#define	CR4_RSVD	0xfffff800
#define CR4_OSXMMEXCPT	0x00000400
#define CR4_OSFXSR	0x00000200
#define CR4_PCE		0x00000100
#define CR4_PGE		0x00000080
#define CR4_MCE		0x00000040
#define CR4_PAE		0x00000020
#define CR4_PSE		0x00000010
#define CR4_DE		0x00000008
#define CR4_TSD		0x00000004
#define CR4_PVI		0x00000002
#define CR4_VME		0x00000001

/*
** PMode segment selectors
**
** IA-32 V3, page 3-8.
*/
#define	SEG_SEL_IX	0xfff8
#define	SEG_SEL_TI	0x0004
#define	SEG_SEL_RPL	0x0003

/*
** Segment descriptor bytes
**
** IA-32 V3, page 3-10.
*/
	/* byte 5 - access control */
#define	SEG_ACCESS_TYPE_MASK	0x0f
#	define		SEG_DATA_A_BIT		0x1
#	define		SEG_DATA_W_BIT		0x2
#	define		SEG_DATA_E_BIT		0x4
#	define		SEG_CODE_A_BIT		0x1
#	define		SEG_CODE_R_BIT		0x2
#	define		SEG_CODE_C_BIT		0x4
#		define	SEG_DATA_RO		0x0
#		define	SEG_DATA_ROA		0x1
#		define	SEG_DATA_RW		0x2
#		define	SEG_DATA_RWA		0x3
#		define	SEG_DATA_RO_XD		0x4
#		define	SEG_DATA_RO_XDA		0x5
#		define	SEG_DATA_RW_XW		0x6
#		define	SEG_DATA_RW_XWA		0x7
#		define	SEG_CODE_XO		0x8
#		define	SEG_CODE_XOA		0x9
#		define	SEG_CODE_XR		0xa
#		define	SEG_CODE_XRA		0xb
#		define	SEG_CODE_XO_C		0xc
#		define	SEG_CODE_XO_CA		0xd
#		define	SEG_CODE_XR_C		0xe
#		define	SEG_CODE_XR_CA		0xf

#define	SEG_ACCESS_S_BIT	0x10
#	define		SEG_S_SYSTEM		0x00
#	define		SEG_S_NON_SYSTEM	0x10

#define	SEG_ACCESS_DPL_MASK	0x60
#	define		SEG_DPL_0		0x00
#	define		SEG_DPL_1		0x20
#	define		SEG_DPL_2		0x40
#	define		SEG_DPL_3		0x60

#define	SEG_ACCESS_P_BIT	0x80

	/* byte 6 - sizes */
#define	SEG_SIZE_LIM_19_16	0x0f

#define	SEG_SIZE_AVL_BIT	0x10

#define	SEG_SIZE_D_B_BIT	0x40
#	define		SEG_DB_16BIT		0x00
#	define		SEG_DB_32BIT		0x40

#define	SEG_SIZE_G_BIT		0x80
#	define		SEG_GRAN_BYTE		0x00
#	define		SEG_GRAN_4KBYTE		0x80

/*
** System-segment and gate-descriptor types
**
** IA-32 V3, page 3-15.
*/
	/* type 0 reserved */
#define	SEG_SYS_16BIT_TSS_AVAIL		0x1
#define	SEG_SYS_LDT			0x2
#define	SEG_SYS_16BIT_TSS_BUSY		0x3
#define	SEG_SYS_16BIT_CALL_GATE		0x4
#define	SEG_SYS_TASK_GATE		0x5
#define	SEG_SYS_16BIT_INT_GATE		0x6
#define	SEG_SYS_16BIT_TRAP_GATE		0x7
	/* type 8 reserved */
#define	SEG_SYS_32BIT_TSS_AVAIL		0x9
	/* type A reserved */
#define	SEG_SYS_32BIT_TSS_BUSY		0xb
#define	SEG_SYS_32BIT_CALL_GATE		0xc
	/* type D reserved */
#define	SEG_SYS_32BIT_INT_GATE		0xe
#define	SEG_SYS_32BIT_TRAP_GATE		0xf

/*
** IDT Descriptors
** 
** IA-32 V3, page 5-13.
**
** All have a segment selector in bytes 2 and 3; Task Gate descriptors
** have bytes 0, 1, 4, 6, and 7 reserved; others have bytes 0, 1, 6,
** and 7 devoted to the 16 bits of the Offset, with the low nybble of
** byte 4 reserved.
*/
#define	IDT_PRESENT		0x8000
#define	IDT_DPL_MASK		0x6000
#	define		IDT_DPL_0	0x0000
#	define		IDT_DPL_1	0x2000
#	define		IDT_DPL_2	0x4000
#	define		IDT_DPL_3	0x6000
#define	IDT_GATE_TYPE		0x0f00
#	define		IDT_TASK_GATE	0x0500
#	define		IDT_INT16_GATE	0x0600
#	define		IDT_INT32_GATE	0x0e00
#	define		IDT_TRAP16_GATE	0x0700
#	define		IDT_TRAP32_GATE	0x0f00

/*
** Interrupt vectors
*/
#define	INT_VEC_DIVIDE_ERROR		0x00
#define	INT_VEC_DEBUG_EXCEPTION		0x01
#define	INT_VEC_NMI_INTERRUPT		0x02
#define	INT_VEC_BREAKPOINT		0x03
#define	INT_VEC_INTO_DETECTED_OVERFLOW	0x04
#define	INT_VEC_BOUND_RANGE_EXCEEDED	0x05
#define	INT_VEC_INVALID_OPCODE		0x06
#define	INT_VEC_DEVICE_NOT_AVAILABLE	0x07
#define	INT_VEC_DOUBLE_EXCEPTION	0x08
#define	INT_VEC_COPROCESSOR_OVERRUN	0x09
#define	INT_VEC_INVALID_TSS		0x0a
#define	INT_VEC_SEGMENT_NOT_PRESENT	0x0b
#define	INT_VEC_STACK_FAULT		0x0c
#define	INT_VEC_GENERAL_PROTECTION	0x0d
#define	INT_VEC_PAGE_FAULT		0x0e

#define	INT_VEC_COPROCESSOR_ERROR	0x10
#define	INT_VEC_ALIGNMENT_CHECK		0x11
#define	INT_VEC_MACHINE_CHECK		0x12
#define	INT_VEC_SIMD_FP_EXCEPTION	0x13

#define	INT_VEC_TIMER			0x20
#define	INT_VEC_KEYBOARD		0x21

#define	INT_VEC_SERIAL_PORT_2		0x23
#define	INT_VEC_SERIAL_PORT_1		0x24
#define	INT_VEC_PARALLEL_PORT		0x25
#define	INT_VEC_FLOPPY_DISK		0x26
#define	INT_VEC_MYSTERY			0x27
#define	INT_VEC_MOUSE			0x2c

/*
** Hardware timer (Intel 8254 Programmable Interval Timer)
*/
#define	TIMER_DEFAULT_TICKS_PER_SECOND	18	/* default ticks per second */
#define	TIMER_DEFAULT_MS_PER_TICK	(1000/TIMER_DEFAULT_TICKS_PER_SECOND)
#define	TIMER_FREQUENCY			1193182	/* clock cycles/sec  */
#define	TIMER_BASE_PORT			0x40	/* I/O port for the timer */
#define	TIMER_0_PORT			( TIMER_BASE_PORT )
#define	TIMER_1_PORT			( TIMER_BASE_PORT + 1 )
#define	TIMER_2_PORT			( TIMER_BASE_PORT + 2 )
#define	TIMER_CONTROL_PORT		( TIMER_BASE_PORT + 3 )

/* Timer 0 settings */
#define	TIMER_0_SELECT			0xc0	/* select timer 0 */
#define	TIMER_0_LOAD			0x30	/* load LSB, then MSB */
#define	TIMER_0_NDIV			0x04	/* divide-by-N counter */
#define	TIMER_0_SQUARE			0x06	/* square-wave mode */
#define	TIMER_0_ENDSIGNAL		0x00	/* assert OUT at end of count */

/* Timer 1 settings */
#define	TIMER_1_SELECT			0x40	/* select timer 1 */
#define	TIMER_1_READ			0x30	/* read/load LSB then MSB */
#define	TIMER_1_RATE			0x06	/* square-wave, for USART */

/*
** This section is based on parts of Sun's "sys/pic.h" include file.
** Some modifications have been made for readability (!?!?).
**
** Original headers follow.
*/

/*
 * Copyright (c) 1992-1998 by Sun Microsystems, Inc.
 * All rights reserved.
 */

/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF		*/
/*	UNIX System Laboratories, Inc.				*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* Definitions for 8259 Programmable Interrupt Controller */

#define	PIC_NEEDICW4	0x01		/* ICW4 needed */
#define	PIC_ICW1BASE	0x10		/* base for ICW1 */
#define	PIC_LTIM	0x08		/* level-triggered mode */
#define	PIC_86MODE	0x01		/* MCS 86 mode */
#define	PIC_AUTOEOI	0x02		/* do auto eoi's */
#define	PIC_SLAVEBUF	0x08		/* put slave in buffered mode */
#define	PIC_MASTERBUF	0x0C		/* put master in buffered mode */
#define	PIC_SPFMODE	0x10		/* special fully nested mode */
#define	PIC_READISR	0x0B		/* Read the ISR */
#define	PIC_READIRR	0x0A		/* Read the IRR */
#define	PIC_EOI		0x20		/* Non-specific EOI command */
#define	PIC_SEOI	0x60		/* specific EOI command */
#define	PIC_SEOI_LVL7	(PIC_SEOI | 0x7)	/* specific EOI for level 7 */

/*
 * Interrupt configuration information specific to a particular computer.
 * These constants are used to initialize tables in modules/pic/space.c.
 * NOTE: The master pic must always be pic zero.
 */

#define	NPIC		2		/* 2 PICs */
/*
** Port addresses for the command port and interrupt mask register port
** for both teh master and slave PICs.
*/
#define	PIC_MASTER_CMD_PORT	0x20	/* master command */
#define	PIC_MASTER_IMR_PORT	0x21	/* master intr mask register */
#define	PIC_SLAVE_CMD_PORT	0xA0	/* slave command */
#define	PIC_SLAVE_IMR_PORT	0xA1	/* slave intr mask register */
#define	PIC_MASTER_SLAVE_LINE	0x04	/* bit mask: slave id */
#define PIC_SLAVE_ID		0x02	/* integer: slave id */
#define	PIC_BUFFERED		0	/* PICs not in buffered mode */

#endif
