h54779
s 00025/00006/00035
d D 1.4 09/10/18 16:28:19 wrc 4 3
c updated for usb flash drive use
e
s 00006/00006/00035
d D 1.3 09/03/21 15:32:02 wrc 3 2
c moved GDT to 0x500 to avoid bios mem map; moved IDT to 0x2500
e
s 00003/00003/00038
d D 1.2 05/03/15 15:29:50 wrc 2 1
c (wrc) fixed defined symbol to match file name
e
s 00041/00000/00000
d D 1.1 02/05/16 15:10:15 kar 1 0
c date and time created 02/05/16 15:10:15 by kar
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
** File:	bootstrap.h
**
** Author:	K. Reek
**
D 2
** Contributor:
E 2
I 2
D 4
** Contributor:	Warren R. Carithers
E 4
I 4
** Contributor:	Warren R. Carithers, Garrett C. Smith
E 4
E 2
**
** Description:	Addresses where various stuff goes in memory.
*/

D 2
#ifndef	_MEMORY_H
#define	_MEMORY_H
E 2
I 2
#ifndef	_BOOTSTRAP_H
#define	_BOOTSTRAP_H
E 2

D 4

E 4
/*
** The target program itself
*/
#define TARGET_SEGMENT	0x00001000	/* 0001:0000 */
#define TARGET_ADDRESS	0x00010000	/* and upward */
#define	TARGET_STACK	0x00010000	/* and downward */

/*
D 3
** The Global Descriptor Table (0000:0400 - 0000:2400)
E 3
I 3
** The Global Descriptor Table (0000:0500 - 0000:2500)
E 3
*/
D 3
#define	GDT_SEGMENT	0x00000040
#define	GDT_ADDRESS	0x00000400
E 3
I 3
#define	GDT_SEGMENT	0x00000050
#define	GDT_ADDRESS	0x00000500
E 3

D 4
#define	GDT_LINEAR	0x08		/* All of memory, R/W */
#define	GDT_CODE	0x10		/* All of memory, R/E */
#define	GDT_DATA	0x18		/* All of memory, R/W */
#define	GDT_STACK	0x20		/* All of memory, R/W */
E 4
I 4
	/* segment register values */
#define	GDT_LINEAR	0x0008		/* All of memory, R/W */
#define	GDT_CODE	0x0010		/* All of memory, R/E */
#define	GDT_DATA	0x0018		/* All of memory, R/W */
#define	GDT_STACK	0x0020		/* All of memory, R/W */
E 4

/*
D 3
** The Interrupt Descriptor Table (0000:2400 - 0000:2C00)
E 3
I 3
** The Interrupt Descriptor Table (0000:2500 - 0000:2D00)
E 3
*/
D 3
#define	IDT_SEGMENT	0x00000240
#define IDT_ADDRESS	0x00002400
E 3
I 3
#define	IDT_SEGMENT	0x00000250
#define IDT_ADDRESS	0x00002500
E 3

I 4
/*
** Physical Memory Map Table (0000:2D00 - 0000:2D08)
*/
#define	MMAP_SEGMENT	0x000002D0
#define	MMAP_ADDRESS	0x00002D00
#define	MMAP_EXT_LO	0x00	/* extended memory - low register */
#define	MMAP_EXT_HI	0x02	/* extended memory - high register */
#define	MMAP_CFG_LO	0x04	/* configured memory - low register */
#define	MMAP_CFG_HI	0x06	/* configured memory - high register */
#define	MMAP_PROGRAMS	0x08	/* # of programs loaded from disk (+ kernel) */
#define	MMAP_SECTORS	0x0a	/* table of sector counts for each program */

/*
** Real Mode Program(s) Text Area (0000:3000 - 0x7c00)
*/

#define	RMTEXT_SEGMENT	0x00000300
#define	RMTEXT_ADDRESS	0x00003000

E 4
#endif
E 1
