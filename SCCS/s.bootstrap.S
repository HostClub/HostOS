h24427
s 00080/00053/00489
d D 1.7 11/01/29 20:58:43 wrc 7 6
c updated for DG41TX - enforces 446-byte boot sector limit
e
s 00059/00011/00483
d D 1.6 09/10/18 16:28:18 wrc 6 5
c updated for usb flash drive use
e
s 00001/00001/00493
d D 1.5 06/11/10 20:14:07 wrc 5 4
c fixed commend on code_seg access byte
e
s 00001/00001/00493
d D 1.4 05/03/15 15:34:16 wrc 4 3
c (wrc) global to globl for consistency
e
s 00001/00001/00493
d D 1.3 05/03/15 15:28:32 wrc 3 2
c (wrc) clarified a bit of documentation
e
s 00265/00307/00229
d D 1.2 02/05/16 15:14:50 kar 2 1
c Version 2: improvements based on my experience with 20013 506.
e
s 00536/00000/00000
d D 1.1 02/01/15 16:18:24 kar 1 0
c date and time created 02/01/15 16:18:24 by kar
e
u
U
f e 0
t
T
I 1
/*
D 2
** SCCS ID: %W%	%G%
E 2
I 2
** SCCS ID:	%W%	%G%
E 2
**
D 2
** File:		bootstrap.S
E 2
I 2
** File:	bootstrap.S
E 2
**
** Author:	Jon Coles
**		copyleft 1999 Jon Coles
**
D 2
** Contributors:	Warren R. Carithers
E 2
I 2
D 6
** Contributor:	Warren R. Carithers, K. Reek
E 6
I 6
** Contributor:	Warren R. Carithers, K. Reek, Garrett C. Smith
E 6
E 2
**
** Description:	Bootstrap routine.
**
** This bootstrap program is loaded by the PC BIOS into memory at
** location 0000:7C00.  It must be exactly 512 bytes long, and must
** end with the hex sequence AA55 at location 1FE.
**
** The bootstrap initially sets up a stack in low memory.  Next, it
D 2
** figures out how many additional sectors are to be loaded from the
** floppy disk, and loads them beginning at 0000:7E00 (immediately
** following the bootstrap).
E 2
I 2
** loads a second sector at 0000:7E00 (immediately following the
** boot block).  Then it loads the target program at TARGET_ADDRESS, 
** switches to protected mode, and branches to the target program.
E 2
**
D 2
** Note that this code is intended to be assembled with as, not as86.
E 2
I 2
** NOTE: This loader does NOT zero out the bss of any of the loaded
** programs.  However, a reset appears to set all memory locations
D 3
** from 0x10000 to 0x45d50 to 0, so there is no need to explicitly
E 3
I 3
** between 0x10000 and 0x45d50 to 0, so there is no need to explicitly
E 3
** zero the bss in modules loaded into that range.
E 2
**
D 2
**
** Must assemble as 16-bit code.
E 2
I 2
** Must assemble this as 16-bit code.
E 2
*/
	.code16

D 2
NSTARTSECTS = 2		/* number of sectors for the startup code */
E 2
I 2
#include "bootstrap.h"
E 2

D 2
GDT_SEG   = 0x0040	/* where we'll put the GDT */
IDT_SEG   = 0x0240	/* where we'll put the IDT */
BOOT_SEG  = 0x07C0	/* default BIOS addr to load boot sector */
START_SEG = 0x0000	/* where we'll put the startup code */
OS_SEG    = 0x1080	/* where we'll put the os itself */
E 2
I 2
BOOT_SEGMENT	= 0x07C0	/* default BIOS addr to load boot sector */
BOOT_ADDRESS 	= 0x00007C00
START_SEGMENT	= 0x0000	/* where we'll put the startup code */
START_OFFSET	= 0x00007E00
I 6
SECTOR_SIZE	= 0x200		/* typical sector size for floppy & HD */
OFFSET_LIMIT	= 65536 - SECTOR_SIZE
E 6
E 2

D 2
GDT_PADDR   = 0x00000400	/* physical addresses for these segments */
IDT_PADDR   = 0x00002400
BOOT_PADDR  = 0x00007C00
START_PADDR = 0x00007E00
OS_PADDR    = 0x00010800

E 2
/*
** Symbol for locating the beginning of the code.
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
** Entry point.	Begin by setting up a runtime stack.
*/
D 2
	movw	$BOOT_SEG, %ax	/* get our data seg */
	movw	$0x4000-12, %di
E 2
I 2
	movw	$BOOT_SEGMENT, %ax	/* get our data seg */
E 2
	movw	%ax, %ds
D 2
	movw	%ax, %ss	/* put stack at INITSEG:0x4000-12. */
	movw	%di, %sp
E 2
I 2
	movw	%ax, %ss	/* stack segment starts at BOOT_SEGMENT */
	movw	$0x4000, %ax	/* and the stack starts 0x4000 beyond that */
	movw	%ax, %sp
E 2

/*
** Next, verify that the disk is there and working.
*/
	movb	$0x01, %ah	/* test the disk status and make sure */
D 6
	movb	$0, %dl		/* it's safe to proceed */
E 6
I 6
	movb	drive, %dl	/* it's safe to proceed */
E 6
	int	$0x13
	jnc	diskok

D 2
/*
** Oops - something went wrong.
*/
	movw	$err_diskstatus, %si
	call	dispMsg
f0:	jmp	f0
E 2
I 2
	movw	$err_diskstatus, %si /* Something went wrong; print a message */
	call	dispMsg		/* and freeze. */
	jmp	.
E 2

D 2
/*
** The disk is OK, so we now need to load the startup code.
** It must immediately follow the boot sector on the disk,
** and must sit between the boot sector and the OS on the disk.
*/
E 2
diskok:
D 2
	movw	$3, %cx		/* on error retry this read 3 times */
	
read_disk:
	pushw	%cx
E 2
I 2
D 6
	movw	$0,%ax		/* Reset the floppy disk */
	movb	$0,%dl
	int	$0X13
E 6
I 6
	movw	$0,%ax		/* Reset the disk */
	movb	drive,%dl
	int	$0x13
E 6
E 2

I 6
	/* get drive parameters to determine number of heads and sectors/track */
	xorw	%ax, %ax	/* set ES:DI = 0000:0000 in case of BIOS bugs */
	movw	%ax, %es
	movw	%ax, %di
	movb	$0x08, %ah	/* get drive parameters */
	movb	drive, %dl	/* hard disk or floppy */
	int	$0x13

	/* store (max + 1) - CL[5:0] = maximum head, DH = maximum head */
	andb	$0x3F, %cl
	incb	%cl
	incb	%dh

	movb	%cl, max_sec
	movb	%dh, max_head

E 6
D 2
	movw	$0x00, %ax	/* reset disk */
	movb	$0x00, %dl
	int	$0x13

	movw	$START_SEG, %ax	/* point ES to mem just after boot prog */
	movw	%ax, %es
	movw	$START_PADDR, %bx

	movb	$0x02, %ah	/* read sectors */
	movb	$0x01, %al	/* how many to read */
	movw	$0x0002, %cx	/* track=0, sector=1 */
	movw	$0, %dx		/* drive=0, head=0 */
	int	$0x13

	popw	%cx

	jnc	load_os		/* no C = OK */
	loop	read_disk	/* else try again */

E 2
/*
D 2
** Tried three times and didn't succeed - report the error and freeze.
E 2
I 2
** The disk is OK, so we now need to load the second page of the bootstrap.
** It must immediately follow the boot sector on the disk,
** and the target program(s) must immediately follow.
E 2
*/
D 2
	movw	$err_diskread, %si
E 2
I 2
	movw	$msg_loading,%si /* Print the Loading message */
E 2
	call	dispMsg
D 2
f1:	jmp	f1
E 2

I 2
	movw	$1,%ax		/* sector count = 1 */
	movw	$START_SEGMENT,%bx /* read this into memory that */
	movw	%bx,%es		/* immediately follows this code. */
	movw	$START_OFFSET,%bx
	call	readprog

E 2
/*
D 2
** OK, we got the startup routine into memory at 0000:7E00.
** Now, read the rest of the OS into memory at 0001:0800.
E 2
I 2
** We've got the second block of the bootstrap program in memory. Now
** read all of the user's program blocks.  Use %di to point to the
** count field for the next block to load.
E 2
*/
D 2
load_os:
E 2
I 2
	movw	$firstcount,%di
E 2

I 6
	pushw	%ds
	movw	(%di), %bx
	movw	$MMAP_SEGMENT, %ax
	movw	%ax, %ds
	movw	%bx, MMAP_SECTORS	/* store kernel image size */
	popw	%ds

E 6
D 2
	movw	$msg_loading, %si	/* status message */
	call	dispMsg
E 2
I 2
nextblock:
	movw	(%di),%ax	/* get the # of sectors */
	testw	%ax,%ax		/* is it zero? */
	jz	done_loading	/*   yes, nothing more to load. */
E 2

D 2
	movw	$3, %cx		/* on error retry this read 3 times */
	
read_disk2:
	pushw	%cx
E 2
I 2
	subw	$2,%di
	movw	(%di),%bx	/* get the segment value */
	movw	%bx,%es		/*   and copy it to %es */
	subw	$2,%di
	movw	(%di),%bx	/* get the address offset */
	subw	$2,%di
	pushw	%di		/* save di */
	call	readprog	/* read this program block, */
	popw	%di		/* and restore di */
	jmp	nextblock	/*   then go back and read the next one. */
E 2

D 2
	movw	$0x00, %ax	/* reset disk */
	movb	$0x00, %dl
	int	$0x13

	movw	$OS_SEG, %ax	/* point ES to destination */
	movw	%ax, %es
	movw	$0x0000, %bx

	movw	os_len, %ax	/* grab os length (see below) */
	movw	$0x0003, %cx	/* track=0, sector=2 */
	movw	$0x0000, %dx	/* head=0, drive=0 */

E 2
/*
D 2
** Read the next sector from the disk into memory.
E 2
I 2
** Read one complete program block into memory.
**
**	ax: number of sectors to read
**	es:bx = starting address for the block
E 2
*/
D 2
next_sect:
	pushw	%ax		/* save os_len */
	pushw	%bx
E 2
I 2
readprog:
	pushw	%ax		/* save sector count */
E 2

I 2
	movw	$3,%cx		/* initial retry count is 3 */
E 2
retry:
D 2
	movw	$0x0201, %ax	/* read the next sector */
	int	$0x13
E 2
I 2
	pushw	%cx		/* push the retry count on the stack. */
E 2

D 2
	jnc	continue	/* OK? */
E 2
I 2
	movw	sec,%cx		/* get sector number */
	movw	head,%dx	/* get head number */
I 6
	movb	drive, %dl
E 6
E 2

D 2
	movw	$err_disktrouble, %si 	/* no - report this */
	call	dispMsg

	pushw	%dx		/* and try again - */
	movw	$0x00, %ax	/* reset disk */
	movb	$0x00, %dl
E 2
I 2
	movw	$0x0201,%ax	/* read 1 sector */
E 2
	int	$0x13
D 2
	popw	%dx
	jmp	retry		/* and loop */
E 2
I 2
	jnc	readcont	/* jmp if it worked ok */
E 2

D 2
/*
** OK, we got the sector.  Adjust our pointer.
*/
continue:
E 2
I 2
	movw	$err_diskread,%si /* report the error */
	call	dispMsg
	popw	%cx		/* get the retry count back */
	loop	retry		/*   and go try again. */
	movw	$err_diskfail,%si /* can't proceed, */
	call	dispMsg		/* print message and freeze. */
	jmp	.
E 2

D 2
	popw	%bx
	addw	$512, %bx	/* adjust memory offset */
E 2
I 2
readcont:
	movw	$msg_dot,%si	/* print status: a dot */
	call	dispMsg
D 6
	cmpw	$65536-512,%bx	/* have we reached the limit of the offset? */
E 6
I 6
D 7
	cmpw	$OFFSET_LIMIT,%bx	/* have we reached the limit of the offset? */
E 7
I 7
	cmpw	$OFFSET_LIMIT,%bx  /* have we reached the offset limit? */
E 7
E 6
	je	adjust		/* Yes--must adjust the es register */
D 6
	addw	$512,%bx	/* No--just adjust the block size to */
E 6
I 6
	addw	$SECTOR_SIZE,%bx	/* No--just adjust the block size to */
E 6
	jmp	readcont2	/*    the offset and continue. */
E 2

D 2
	cmpw	$0xffff, %bx
	jnz	continue2

	movw	$0x0000, %bx	/* oops - too long */
E 2
I 2
adjust:
	movw	$0, %bx		/* start offset over again */
E 2
	movw	%es, %ax
D 2
	incw	%ax
E 2
I 2
	addw	$0x1000,%ax	/* move segment pointer to next chunk */
E 2
	movw	%ax, %es

D 2
	movw	$err_diskstatus, %si 	/* report the error */
	call	dispMsg
f2:	jmp	f2		/* and freeze */
	
continue2:
	popw	%ax		/* grab os_len */
	decw	%ax		/* count down */

#ifdef DUMP_SECTORS_READ
	pushw	%ax		/* print the number of sectors read */
	pushw	%bx
	pushw	%cx
	movw	%ax, %cx	/* remaining count */
	movw	os_len, %ax	/* total count */
	subw	%cx, %ax	/* ax <- number written */
	orb	$0x30, %al	/* turn into a character */
	movb	$0x0E, %ah	/* write and advance */
	movw	$0x07, %bx	/* video page 0, white on black, no blink */
	movw	$1, %cx		/* one character */
	int	$0x10
	popw	%cx
	popw	%bx
	popw	%ax
#endif

	cmpw	$0, %ax		/* if we've finished reading the OS */
	jle	load_startup	/* we can run the startup code */

E 2
I 2
readcont2:
E 2
	incb	%cl		/* not done - move to the next sector */
D 6
	cmpb	$19, %cl	/* only 18 per track - see if we need */
E 6
I 6
	cmpb	max_sec, %cl	/* only 18 per track - see if we need */
E 6
D 2
	jnz	next_sect	/* to switch heads or tracks */
E 2
I 2
	jnz	save_sector	/* to switch heads or tracks */
E 2

	movb	$1, %cl		/* reset sector number */
	incb	%dh		/* first, switch heads */
D 6
	cmpb	$2, %dh		/* there are only two - if we've already */
E 6
I 6
	cmpb	max_head, %dh	/* there are only two - if we've already */
E 6
D 2
	jnz	next_sect	/* used both, we need to switch tracks */
E 2
I 2
	jnz	save_sector	/* used both, we need to switch tracks */
E 2

D 2
	movw	$msg_dot, %si	/* track switch - give a load status dot */
	call	dispMsg

E 2
	xorb	%dh, %dh	/* reset to head $0 */
	incb	%ch		/* inc track number */
	cmpb	$80, %ch	/* 80 tracks per side - have we read all? */
D 2
	jnz	next_sect	/* read another track */
E 2
I 2
	jnz	save_sector	/* read another track */
E 2

D 2
error:				/* error - we've read all 80x2 tracks! */
	popw	%ax		/* clear the stack */
	popw	%cx
	movw	$err_diskread, %si 	/* report the error */
E 2
I 2
	movw	$err_toobig, %si 	/* report the error */
E 2
	call	dispMsg
D 2
f3:	jmp	f3		/* and freeze */
E 2
I 2
	jmp	.		/* and freeze */
E 2

I 2
save_sector:
	movw	%cx,sec		/* save sector number */
	movw	%dx,head	/*   and head number */

	popw	%ax		/* discard the retry count */
	popw	%ax		/* get the sector count from the stack */
	decw	%ax		/*   and decrement it. */
	jg	readprog	/* If it is zero, we're done reading. */

readdone:
	movw	$msg_bar,%si	/* print message saying this block is done */
	call	dispMsg
	ret			/* and return to the caller */

E 2
/*
D 2
** We've loaded the whole OS into memory, so it's time to transfer to
** the startup code.
E 2
I 2
** We've loaded the whole target program into memory,
** so it's time to transfer to the startup code.
E 2
*/
D 2
load_startup:
E 2
I 2
done_loading:
E 2
	movw	$msg_go, %si	/* last status message */
	call	dispMsg

	jmp	switch		/* move to the next phase */
	
/*
** Support routine - display a message byte by byte to the monitor.
*/
dispMsg:	
	pushw	%ax
	pushw	%bx
repeat:
	lodsb			/* grab next character */

	movb	$0x0e, %ah	/* write and advance cursor */
	movw	$0x07, %bx	/* page 0, white on blank, no blink */
	orb	%al, %al	/* AL is character to write */
	jz	getOut		/* if we've reached the NUL, get out */

	int	$0x10		/* otherwise, print and repeat */
	jmp	repeat	

getOut:				/* we're done, so return */
	popw	%bx
	popw	%ax
	ret

I 2
#if 0
E 2
/*
D 2
** DATA AREAS.
E 2
I 2
** Debugging routine.  This lives in the 1st block of the bootstrap
** so it can be called from there as well as from the 2nd block.
E 2
**
I 2
**	movw	$'x',%di	/* a single character to print */
**	movw	value,%ax	/* a 16-bit value to print in hex */
**	call	pnum
*/
pnum:
	pushw	%ax
	pushw	%bx
	movw	%di,%ax
	movb	$0xe,%ah
	movw	$7,%bx
	int	$0x10

	call	pdigit
	call	pdigit
	call	pdigit
	call	pdigit

	popw	%bx
	popw	%ax
	ret

pdigit:	movw	%si,%ax
	shl	$4,%si
	shr	$12,%ax
	cmpw	$9,%ax
	jle	pdd
	addw	$'A'-10,%ax
	jmp	prt
pdd:	addw	$'0',%ax
prt:	movb	$0xe,%ah
	movw	$7,%bx
	int	$0x10
	ret
#endif

/*
** Move the GDT entries from where they are to location 0000:0000
E 2
**
D 2
** Status messages.
E 2
I 2
** As with the IDTR and GDTR loads, we need the offset for the GDT
** data from the beginning of the segment (0000:0000).
E 2
*/
I 2
move_gdt:
	movw	%cs, %si
	movw	%si, %ds
	movw	$start_gdt + BOOT_ADDRESS, %si
	movw	$GDT_SEGMENT, %di
	movw	%di, %es
	xorw	%di, %di
	movl	$gdt_len, %ecx
	cld
	rep	movsb
	ret

I 6
/*
** Determine the amount of physical memory available to the system.
*/
check_mem:
	pushw	%ax
	pushw	%ds
	movw	$MMAP_SEGMENT, %ax
D 7
	movw	%ax, %ds		/* Write relative to the memory map start */
	movw	$0xE801, %ax		/* Get memory size for >64M configurations */
E 7
I 7
	movw	%ax, %ds	  /* Write relative to the memory map start */
	movw	$0xE801, %ax	  /* Get memory size for >64M configurations */
E 7
	int	$0x15
D 7
	movw	%ax, MMAP_EXT_LO	/* extended memory between 1M - 16M in K */
	movw	%bx, MMAP_EXT_HI	/* extended memory > 16M in 64K blocks */
	movw	%cx, MMAP_CFG_LO	/* configured memory between 1M - 16M in K */
	movw	%dx, MMAP_CFG_HI	/* configured memory > 16M in 64K blocks */
E 7
I 7
	movw	%ax, MMAP_EXT_LO  /* extended memory between 1M - 16M in K */
	movw	%bx, MMAP_EXT_HI  /* extended memory > 16M in 64K blocks */
	movw	%cx, MMAP_CFG_LO  /* configured memory between 1M - 16M in K */
	movw	%dx, MMAP_CFG_HI  /* configured memory > 16M in 64K blocks */
E 7
	popw	%ds
	popw	%ax
	ret
E 6

/*
** DATA AREAS.
**
** Next sector number and head number to read from.
*/
sec:	.word	2	/* cylinder=0, sector=1 */
head:	.word	0	/* head=0 */
I 6
max_sec:	.byte	19	/* up to 18 sectors per floppy track */
max_head:	.byte	2	/* only two r/w heads per floppy drive */
E 6

/*
** Status and error messages.
*/
E 2
msg_loading:
D 2
	.asciz "Loading..."
E 2
I 2
	.asciz "Loading"
E 2
msg_dot:
	.asciz "."
msg_go:
	.asciz "done."
I 2
msg_bar:
	.asciz	"|"
E 2

/*
** Error messages.
*/
err_diskstatus:
D 2
	.asciz "error: Disk not ready. Reinsert disk and reboot."
E 2
I 2
	.asciz "Disk not ready.\n\r"
E 2
err_diskread:
D 2
	.asciz "error: Unable to read from boot disk. Disk may be corrupt."
err_disktrouble:
	.asciz "error: Trouble reading the disk. Will retry until success."
E 2
I 2
	.asciz "Read failed\n\r"
err_toobig:
	.asciz	"Too big\n\r"
err_diskfail:
	.asciz	"Can't proceed\n\r"
E 2

/*
D 2
** End of the boot program.  This must be at the very end of the 512-byte
** boot sector.  The last two bytes must be AA55; we'll use the two bytes
** before that to hold our OS length, to control the read loop above.
E 2
I 2
** Data areas.
D 7
**
**
** The GDT.  This cannot be created in C because the bootstrap is not
** linked with that code.
E 7
E 2
*/
D 2
	.org	508
/*	.org	0x7DFC */
E 2
I 2
D 7
start_gdt:
	.word	0,0,0,0		/* first GDT entry is always null */
E 7
E 2

D 2
os_len:		/* this must be calculated and plugged in at this point */
	.word 0x0000
E 2
I 2
D 7
linear_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF	/* limit[15:0] */
	.word	0x0000	/* base[15:0] */
	.byte	0x00	/* base[23:16] */
	.byte	0x92	/* access byte */
	.byte	0xCF	/* granularity */
	.byte	0x00	/* base[31:24] */
E 2

D 2
boot_sig:	/* boot sector signature bytes - must be at 1FE and 1FF. */
E 2
I 2
code_seg:	/* limit FFFFF, base 0, R/E code seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
D 5
	.byte	0x9A	/* 1 00 1 0010: present, prio 0, C/D, R/E code */
E 5
I 5
	.byte	0x9A	/* 1 00 1 1010: present, prio 0, C/D, R/E code */
E 5
	.byte	0xCF	/* 1 1 00 1111: 4K, 32-bit, 0, 0, limit[19:16] */
	.byte	0x00

data_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92	/* 1 00 1 0010: present, prio 0, C/D, R/W data */
	.byte	0xCF
	.byte	0x00

stack_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xCF
	.byte	0x00

end_gdt:
gdt_len = end_gdt - start_gdt

E 7
/*
D 7
** Next, the GDTR and IDTR contents.
E 7
I 7
** The GDTR and IDTR contents.
E 7
*/
gdt_48:
	.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
	.quad	GDT_ADDRESS

idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_ADDRESS
I 7

E 7
/*
I 7
** Originally, the GDT contents were here.  When booting from a floppy
** disk, that's not a problem, as all 510 available bytes of the boot
** sector can be used.  However, when booting from a hard drive, only
** the first 446 bytes (0x000-0x1bd) can be used, and including the GDT
** here pushed this part of the bootstrap over that limit.  The older
** machines in the lab (Intel D867PERL motherboards) didn't enforce
** this when booting from a flash drive; however, the current machines
** (Intel DG41TX motherboards) do, so the GDT contents are now in the
** second sector of the bootstrap program.
*/

/*
E 7
** End of the first sector of the boot program.  The last two bytes
** of this sector must be AA55 in order for the disk to be recognized
** by the BIOS as bootable.
*/
D 6
	.org	510
E 6
I 6
	.org	SECTOR_SIZE-4
E 6

I 6
drive:	.word	0	/* 0x00 = floppy, 0x80 = usb */

E 6
boot_sig:
E 2
	.word 0xAA55

/*
** Startup code.
**
** This code configures the GDT, enters protected mode, and then
** transfers to the OS entry point.
*/

switch:
	cli
	movb	$0x80, %al	/* disable NMIs */
	outb	%al, $0x70

	call	floppy_off
	call	enable_A20
	call	move_gdt
I 6
	call	check_mem
E 6

/*
** The IDTR and GDTR are loaded relative to this segment, so we must
** use the full offsets from the beginning of the segment (0000:0000);
** however, we were loaded at 0000:7c00, so we need to add that in.
*/
D 2
	lidt	idt_48 + 0x7c00
	lgdt	gdt_48 + 0x7c00
E 2
I 2
	lidt	idt_48 + BOOT_ADDRESS
	lgdt	gdt_48 + BOOT_ADDRESS
E 2

D 2
	cli
E 2
	movl	%cr0, %eax	/* get current CR0 */
	orl	$1, %eax	/* set the PE bit */
D 2
	movl	%eax, %cr0	/* now we're in protected mode */
E 2
I 2
D 7
	movl	%eax, %cr0	/* and store it back.
E 7
I 7
	movl	%eax, %cr0	/* and store it back. */
E 7
	
	/*
	** We'll be in protected mode at the start of the user's code
	** right after this jump executes.
I 7
	**
	** First, a byte to force 32-bit mode execution, followed by
	** a 32-bit long jump.  The long ("far") jump loads both EIP
	** and CS with the proper values so that when we land at the
	** destination address in protected mode, the next instruction
	** fetch doesn't cause a fault.
	**
	** The old code for this:
	**
	**	.byte	06xx, 0xEA
	**	.long	TARGET_ADDRESS
	**	.word	GDT_CODE
E 7
	*/
E 2

D 2
/*
** This strange code is required in order to flush the prefetch cache.
** The MOV is there to make sure the jump isn't optimized out.
*/
	stc
	jc	flush
	mov	%ax, %ax
flush:
	clc
	//movb	$0x00, %al	/* enable NMIs? */
	//outb	%al, $0x70
E 2
I 2
D 7
	.byte	0x66, 0xEA	/* 32-bit mode, jmp */
	.long	TARGET_ADDRESS	/* target address */
D 6
	.word	0x0010		/* descriptor #2, GDT, RPL=0 */
E 6
I 6
	.word	GDT_CODE		/* descriptor #2, GDT, RPL=0 */
E 7
I 7
	.byte	0x66	/* 32-bit mode prefix */
	.code32
	ljmp	$GDT_CODE, $TARGET_ADDRESS
	.code16
E 7
E 6
E 2

/*
D 2
** All set - transfer to the OS entry point.  Note that the OS must be
** in memory at location 0001:0800.
*/
	.byte	0x66, 0xEA
	.long	0x10800
	.word	0x0010

/*
** If we come back, freeze up here.
*/
finish:
	jmp	finish

/*
E 2
** Supporting code.
**
** Turn off the motor on the floppy disk drive.
*/
floppy_off:
	push	%dx
	movw	$0x3f2, %dx
	xorb	%al, %al
	outb	%al, %dx
	pop	%dx
	ret

D 2
clear_a20:
g5:
	inb	$0x64, %al
	test	$2, %al
	jnz	g5
	movb	$0xD1, %al
	outb	%al, $0x64
g6:
	inb	$0x54, %al
	test	$2, %al
	jnz	g6
	mov	$0xDF,%al
	out	%al, $0x60
	ret

E 2
/*
** Enable the A20 gate for full memory access.
*/
enable_A20:
D 2
	cli

E 2
	call	a20wait
	movb	$0xad, %al
	outb	%al, $0x64

	call	a20wait
	movb	$0xd0, %al
	outb	%al, $0x64

	call	a20wait2
	inb	$0x60, %al
	pushl	%eax

	call	a20wait
	movb	$0xd1, %al
	outb	%al, $0x64

	call	a20wait
	popl	%eax
	orb	$2, %al
	outb	%al, $0x60

	call	a20wait
	mov	$0xae, %al
	out	%al, $0x64

	call	a20wait
	ret

a20wait:	/* wait until bit 1 of the device register is clear */
	movl    $65536, %ecx	/* loop a lot if need be */
wait_loop: 
	inb     $0x64, %al	/* grab the byte */
	test    $2, %al		/* is the bit clear? */
	jz      wait_exit	/* yes */
	loop    wait_loop	/* no, so loop */
	jmp     a20wait		/* if still not clear, go again */
wait_exit:    
	ret

a20wait2:	/* like a20wait, but waits until bit 0 is set. */
	mov     $65536, %ecx
wait2_loop:
	in      $0x64, %al
	test    $1, %al
	jnz     wait2_exit
	loop    wait2_loop
	jmp     a20wait2
wait2_exit:
	ret

/*
I 7
** The GDT.  This cannot be created in C because the bootstrap is not
** linked with that code.
*/
start_gdt:
	.word	0,0,0,0		/* first GDT entry is always null */

linear_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF	/* limit[15:0] */
	.word	0x0000	/* base[15:0] */
	.byte	0x00	/* base[23:16] */
	.byte	0x92	/* access byte */
	.byte	0xCF	/* granularity */
	.byte	0x00	/* base[31:24] */

code_seg:	/* limit FFFFF, base 0, R/E code seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9A	/* 1 00 1 1010: present, prio 0, C/D, R/E code */
	.byte	0xCF	/* 1 1 00 1111: 4K, 32-bit, 0, 0, limit[19:16] */
	.byte	0x00

data_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92	/* 1 00 1 0010: present, prio 0, C/D, R/W data */
	.byte	0xCF
	.byte	0x00

stack_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xCF
	.byte	0x00

end_gdt:
gdt_len = end_gdt - start_gdt

/*
E 7
D 2
** Move the GDT entries from where they are to location 0040:0000
E 2
I 2
** The end of this program will contain a list of the sizes and load
** addresses of all of the blocks to be loaded.  THese values are
** inserted here by the Modify program, which checks that there are
** not so many blocks that the IDT would be overwritten.  The layout
** of the data is:
E 2
**
D 2
** As with the IDTR and GDTR loads, we need the offset for the GDT
** data from the beginning of the segment (0000:0000).
*/
move_gdt:
	movw	%cs, %si
	movw	%si, %ds
	movw	$start_gdt + 0x7c00, %si
	movw	$GDT_SEG, %di
	movw	%di, %es
	xorw	%di, %di
	movl	$gdt_len, %ecx
	cld
	rep	movsb
	ret

/*
** Enable the video device.
*/
enable_video:
	movw	$0x4F01, %ax
	movw	$0x0101, %cx	/* 640x480x256 if i remember correctly...  */
	movw	$0x1080, %si	/* we want to write the returned table */
	movw	%si, %es	/* to the beginning of the os */
	movw	$5, %bx
	movw	%bx, %di
	int	$0x10		/* load the data */

	movw	$0x4F02, %ax
	movw	$0x4101, %bx	/* 640x480x256 use linear frame */
	int	$0x10

	cmp	$0x00, %ah
	je	good_card	/* video card must support linear FB */

/*	mov	si, $msg_bad_card */
/*	call	dispMsg */
f4:	jmp	f4		/* error - freeze here */

good_card:
	ret

/*
** Data areas.
E 2
I 2
**	offset
**	segment
**	# of sectors
E 2
**
D 2
**
** The GDT.
E 2
I 2
** with the # of sectors for the first block appearing at firstcount, and
** the other values appearing just before it.  If additional blocks are
** to be loaded, their values appear just before the previous set.
E 2
*/
D 2
start_gdt:
	.word	0,0,0,0		/* first GDT entry is always null */
E 2

D 2
linear_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF	/* limit[15:0] */
	.word	0x0000	/* base[15:0] */
	.byte	0x00	/* base[23:16] */
	.byte	0x92	/* access byte */
	.byte	0xCF	/* granularity */
	.byte	0x00	/* base[31:24] */

code_seg:	/* limit FFFFF, base 0, R/E code seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9A	/* 1 00 1 0010: present, prio 0, C/D, R/E code */
	.byte	0xCF	/* 1 1 00 1111: 4K, 32-bit, 0, 0, limit[19:16] */
	.byte	0x00

data_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92	/* 1 00 1 0010: present, prio 0, C/D, R/W data */
	.byte	0xCF
	.byte	0x00

stack_seg:	/* limit FFFFF, base 0, R/W data seg, 32-bit 4K */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xCF
	.byte	0x00
end_gdt:
gdt_len = end_gdt - start_gdt

/*
** Next, the IDTR and GDTR contents.
*/
idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_PADDR	/* at 0000:2400 */

gdt_48:
	.word	0x2000		/* gdt 8192 GDT entries */
	.quad	GDT_PADDR	/* at 0000:0400 */

	.org	1024
/*	.org	0x8000 */
E 2
I 2
	.org	1024-2
firstcount:
	.word	0	/* n_sectors for 1st module will go here */
E 2
E 1
