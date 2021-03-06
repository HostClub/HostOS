.code16
#include "bootstrap.h"
#include "trampoline.h"

BSP_BOOT_SEGMENT	= 0x07C0	/* default BIOS addr to load boot sector */
BSP_BOOT_ADDRESS 	= 0x00007C00
AP_BOOT_SEGMENT =	 0x0800
AP_BOOT_ADDRESS =	0x00008000


/*
 ** Symbol for locating the beginning of the code.
 */
.globl begtextt

.text
begtextt:

cli
xor %eax, %eax
movl %eax, %cr0

movb $5, 0x9000

movw $AP_BOOT_SEGMENT, %ax
movw %ax, %ds
movw %ax, %ss
movw $0x1000, %ax
movw %ax, %sp

movb $6, 0x1000

/*push $0x1000
call mutex_test_and_set
add $4, %esp*/

jmp switch

/*
 ** Enable the A20 gate for full memory access.
 */
enable_A20t:
movb $81, 0x1000
call	a20waitt
movb $82, 0x1000
movb	$0xad, %al
outb	%al, $0x64

call	a20waitt
movb	$0xd0, %al
outb	%al, $0x64

call	a20wait2t
inb	$0x60, %al
pushl	%eax

call	a20waitt
movb	$0xd1, %al
outb	%al, $0x64

call	a20waitt
popl	%eax
orb	$2, %al
outb	%al, $0x60

call	a20waitt
mov	$0xae, %al
out	%al, $0x64

call	a20waitt
ret





a20waitt:	/* wait until bit 1 of the device register is clear */
movl    $65536, %ecx	/* loop a lot if need be */
wait_loop: 
inb     $0x64, %al	/* grab the byte */
test    $2, %al		/* is the bit clear? */
jz      wait_exit	/* yes */
loop    wait_loop	/* no, so loop */
jmp     a20waitt		/* if still not clear, go again */
wait_exit:
ret

a20wait2t:	/* like a20wait, but waits until bit 0 is set. */
mov     $65536, %ecx
wait2_loop:
in      $0x64, %al
test    $1, %al
jnz     wait2_exit
loop    wait2_loop
jmp     a20wait2t
wait2_exit:
ret

ldksfja:
hlt
jmp ldksfja

/*
 ** Startup code.
 **
 ** This code configures the GDT, enters protected mode, and then
 ** transfers to the OS entry point.
 */

/*
 ** The GDTR and IDTR contents.
 */
gdt_48t:
.word	0x2000		/* 1024 GDT entries x 8 bytes/entry = 8192 */
.quad	GDT_ADDRESS

idt_48t:
.word	0x0800		/* 256 interrupts */
.quad	IDT_ADDRESS



switch:
movb $7, 0x1000

cli
movb	$0x80, %al	/* disable NMIs */
outb	%al, $0x70


movb $8, 0x1000

call	enable_A20t
movw	%cs, %si
movw	%si, %ds

movb $9, 0x1000



/*
 ** The IDTR and GDTR are loaded relative to this segment, so we must
 ** use the full offsets from the beginning of the segment (0000:0000);
 ** however, we were loaded at 0000:8000, so we need to add that in.
 */
lidt	idt_48t + AP_BOOT_ADDRESS
lgdt	gdt_48t + AP_BOOT_ADDRESS

/*lidtw 0x7db8
lgdtw 0x7dae*/

movb $10, 0x1000


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

//jmp .

movl	%cr0, %eax	/* get current CR0 */
orl	$1, %eax	/* set the PE bit */
movl	%eax, %cr0	/* and store it back. */


.byte	0x66	/* 32-bit mode prefix */
.code32
ljmp	$GDT_CODE, $TARGET_ADDRESS
/*ljmp	$GDT_CODE, $(AP_BOOT_ADDRESS + target)*/


target:
movw $40, 0x9000
hlt
jmp target
