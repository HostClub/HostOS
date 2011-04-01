/*
** SCCS ID:	@(#)bootstrap.S	1.7	01/29/11
**
** File:	bootstrap.S
**
** Author:	Jon Coles
**		copyleft 1999 Jon Coles
**
** Contributor:	Warren R. Carithers, K. Reek, Garrett C. Smith
**
** Description:	Bootstrap routine.
**
** This bootstrap program is loaded by the PC BIOS into memory at
** location 0000:7C00.  It must be exactly 512 bytes long, and must
** end with the hex sequence AA55 at location 1FE.
**
** The bootstrap initially sets up a stack in low memory.  Next, it
** loads a second sector at 0000:7E00 (immediately following the
** boot block).  Then it loads the target program at TARGET_ADDRESS, 
** switches to protected mode, and branches to the target program.
**
** NOTE: This loader does NOT zero out the bss of any of the loaded
** programs.  However, a reset appears to set all memory locations
** between 0x10000 and 0x45d50 to 0, so there is no need to explicitly
** zero the bss in modules loaded into that range.
**
** Must assemble this as 16-bit code.
*/
	.code16

#include "bootstrap.h"

BOOT_SEGMENT	= 0x07C0	/* default BIOS addr to load boot sector */
BOOT_ADDRESS 	= 0x00007C00
START_SEGMENT	= 0x0000	/* where we'll put the startup code */
START_OFFSET	= 0x00007E00
SECTOR_SIZE	= 0x200		/* typical sector size for floppy & HD */
OFFSET_LIMIT	= 65536 - SECTOR_SIZE

/*
** Symbol for locating the beginning of the code.
*/
	.globl begtext

	.text
begtext:

/*
** Entry point.	Begin by setting up a runtime stack.
*/
	movw	$BOOT_SEGMENT, %ax	/* get our data seg */
	movw	%ax, %ds
	movw	%ax, %ss	/* stack segment starts at BOOT_SEGMENT */
	movw	$0x4000, %ax	/* and the stack starts 0x4000 beyond that */
	movw	%ax, %sp

/*
** Next, verify that the disk is there and working.
*/
	movb	$0x01, %ah	/* test the disk status and make sure */
	movb	drive, %dl	/* it's safe to proceed */
	int	$0x13
	jnc	diskok

	movw	$err_diskstatus, %si /* Something went wrong; print a message */
	call	dispMsg		/* and freeze. */
	jmp	.

diskok:
	movw	$0,%ax		/* Reset the disk */
	movb	drive,%dl
	int	$0x13

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

/*
** The disk is OK, so we now need to load the second page of the bootstrap.
** It must immediately follow the boot sector on the disk,
** and the target program(s) must immediately follow.
*/
	movw	$msg_loading,%si /* Print the Loading message */
	call	dispMsg

	movw	$1,%ax		/* sector count = 1 */
	movw	$START_SEGMENT,%bx /* read this into memory that */
	movw	%bx,%es		/* immediately follows this code. */
	movw	$START_OFFSET,%bx
	call	readprog

/*
** We've got the second block of the bootstrap program in memory. Now
** read all of the user's program blocks.  Use %di to point to the
** count field for the next block to load.
*/
	movw	$firstcount,%di

	pushw	%ds
	movw	(%di), %bx
	movw	$MMAP_SEGMENT, %ax
	movw	%ax, %ds
	movw	%bx, MMAP_SECTORS	/* store kernel image size */
	popw	%ds

nextblock:
	movw	(%di),%ax	/* get the # of sectors */
	testw	%ax,%ax		/* is it zero? */
	jz	done_loading	/*   yes, nothing more to load. */

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

/*
** Read one complete program block into memory.
**
**	ax: number of sectors to read
**	es:bx = starting address for the block
*/
readprog:
	pushw	%ax		/* save sector count */

	movw	$3,%cx		/* initial retry count is 3 */
retry:
	pushw	%cx		/* push the retry count on the stack. */

	movw	sec,%cx		/* get sector number */
	movw	head,%dx	/* get head number */
	movb	drive, %dl

	movw	$0x0201,%ax	/* read 1 sector */
	int	$0x13
	jnc	readcont	/* jmp if it worked ok */

	movw	$err_diskread,%si /* report the error */
	call	dispMsg
	popw	%cx		/* get the retry count back */
	loop	retry		/*   and go try again. */
	movw	$err_diskfail,%si /* can't proceed, */
	call	dispMsg		/* print message and freeze. */
	jmp	.

readcont:
	movw	$msg_dot,%si	/* print status: a dot */
	call	dispMsg
	cmpw	$OFFSET_LIMIT,%bx  /* have we reached the offset limit? */
	je	adjust		/* Yes--must adjust the es register */
	addw	$SECTOR_SIZE,%bx	/* No--just adjust the block size to */
	jmp	readcont2	/*    the offset and continue. */

adjust:
	movw	$0, %bx		/* start offset over again */
	movw	%es, %ax
	addw	$0x1000,%ax	/* move segment pointer to next chunk */
	movw	%ax, %es

readcont2:
	incb	%cl		/* not done - move to the next sector */
	cmpb	max_sec, %cl	/* only 18 per track - see if we need */
	jnz	save_sector	/* to switch heads or tracks */

	movb	$1, %cl		/* reset sector number */
	incb	%dh		/* first, switch heads */
	cmpb	max_head, %dh	/* there are only two - if we've already */
	jnz	save_sector	/* used both, we need to switch tracks */

	xorb	%dh, %dh	/* reset to head $0 */
	incb	%ch		/* inc track number */
	cmpb	$80, %ch	/* 80 tracks per side - have we read all? */
	jnz	save_sector	/* read another track */

	movw	$err_toobig, %si 	/* report the error */
	call	dispMsg
	jmp	.		/* and freeze */

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

/*
** We've loaded the whole target program into memory,
** so it's time to transfer to the startup code.
*/
done_loading:
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

#if 0
/*
** Debugging routine.  This lives in the 1st block of the bootstrap
** so it can be called from there as well as from the 2nd block.
**
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
**
** As with the IDTR and GDTR loads, we need the offset for the GDT
** data from the beginning of the segment (0000:0000).
*/
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

/*
** Determine the amount of physical memory available to the system.
*/
check_mem:
	pushw	%ax
	pushw	%ds
	movw	$MMAP_SEGMENT, %ax
	movw	%ax, %ds	  /* Write relative to the memory map start */
	movw	$0xE801, %ax	  /* Get memory size for >64M configurations */
	int	$0x15
	movw	%ax, MMAP_EXT_LO  /* extended memory between 1M - 16M in K */
	movw	%bx, MMAP_EXT_HI  /* extended memory > 16M in 64K blocks */
	movw	%cx, MMAP_CFG_LO  /* configured memory between 1M - 16M in K */
	movw	%dx, MMAP_CFG_HI  /* configured memory > 16M in 64K blocks */
	popw	%ds
	popw	%ax
	ret

/*
** DATA AREAS.
**
** Next sector number and head number to read from.
*/
sec:	.word	2	/* cylinder=0, sector=1 */
head:	.word	0	/* head=0 */
max_sec:	.byte	19	/* up to 18 sectors per floppy track */
max_head:	.byte	2	/* only two r/w heads per floppy drive */

/*
** Status and error messages.
*/
msg_loading:
	.asciz "Loading"
msg_dot:
	.asciz "."
msg_go:
	.asciz "done."
msg_bar:
	.asciz	"|"

/*
** Error messages.
*/
err_diskstatus:
	.asciz "Disk not ready.\n\r"
err_diskread:
	.asciz "Read failed\n\r"
err_toobig:
	.asciz	"Too big\n\r"
err_diskfail:
	.asciz	"Can't proceed\n\r"

/*
** Data areas.
*/

/*
** The GDTR and IDTR contents.
*/
gdt_48:
	.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
	.quad	GDT_ADDRESS

idt_48:
	.word	0x0800		/* 256 interrupts */
	.quad	IDT_ADDRESS

/*
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
** End of the first sector of the boot program.  The last two bytes
** of this sector must be AA55 in order for the disk to be recognized
** by the BIOS as bootable.
*/
	.org	SECTOR_SIZE-4

drive:	.word	0	/* 0x00 = floppy, 0x80 = usb */

boot_sig:
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
	call	check_mem

/*
** The IDTR and GDTR are loaded relative to this segment, so we must
** use the full offsets from the beginning of the segment (0000:0000);
** however, we were loaded at 0000:7c00, so we need to add that in.
*/
	lidt	idt_48 + BOOT_ADDRESS
	lgdt	gdt_48 + BOOT_ADDRESS

	movl	%cr0, %eax	/* get current CR0 */
	orl	$1, %eax	/* set the PE bit */
	movl	%eax, %cr0	/* and store it back. */
	
	/*
	** We'll be in protected mode at the start of the user's code
	** right after this jump executes.
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
	*/

	.byte	0x66	/* 32-bit mode prefix */
	.code32
	ljmp	$GDT_CODE, $TARGET_ADDRESS
	.code16

/*
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

/*
** Enable the A20 gate for full memory access.
*/
enable_A20:
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
** The end of this program will contain a list of the sizes and load
** addresses of all of the blocks to be loaded.  THese values are
** inserted here by the Modify program, which checks that there are
** not so many blocks that the IDT would be overwritten.  The layout
** of the data is:
**
**	offset
**	segment
**	# of sectors
**
** with the # of sectors for the first block appearing at firstcount, and
** the other values appearing just before it.  If additional blocks are
** to be loaded, their values appear just before the previous set.
*/

	.org	1024-2
firstcount:
	.word	0	/* n_sectors for 1st module will go here */
