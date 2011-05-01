.code16

#include "bootstrap.h"

BSP_BOOT_SEGMENT	= 0x07C0	/* default BIOS addr to load boot sector */
BSP_BOOT_ADDRESS 	= 0x00007C00

/*
 ** Symbol for locating the beginning of the code.
 */
.globl begtext

.text
begtext:

movb $'h', 0x9000
movb $'e', 0x9001
movb $'l', 0x9002
movb $'l', 0x9003
movb $'o', 0x9004
movb $0, 0x9005

aldksfja:
hlt
jmp aldksfja

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

call	enable_A20

/*
 ** The IDTR and GDTR are loaded relative to this segment, so we must
 ** use the full offsets from the beginning of the segment (0000:0000);
 ** however, we were loaded at 0000:7c00, so we need to add that in.
 */
lidt	idt_48 + BSP_BOOT_ADDRESS
lgdt	gdt_48 + BSP_BOOT_ADDRESS

movl	%cr0, %eax	/* get current CR0 */
orl	$1, %eax	/* set the PE bit */
movl	%eax, %cr0	/* and store it back. */




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


#if 0
/* Check to see if this is the BSP */
movl	$IA32_APIC_BASE, %eax
andl	$BSP_MASK, %eax
jz ap_target
#endif

bsp_target:
.byte	0x66	/* 32-bit mode prefix */
.code32
ljmp	$GDT_CODE, $target

.long 0x78656C41
.long 0x0

target:

movb $'h', 0x9000
movb $'e', 0x9001
movb $'l', 0x9002
movb $'l', 0x9003
movb $'o', 0x9004
movb $0, 0x9005

baldksfja:
hlt
jmp baldksfja



