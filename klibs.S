/*
** sccs id:	@(#)klibs.S	1.1	03/31/11
**
** File:	klibs.S
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description: Assembly implementations of some kernel-level library routines
*/

#define	__ASM__20103__

/*
** _get_ebp - return current contents of EBP at the time of the call
**
** Primarily used only by _kpanic
*/

	.globl	_get_ebp
_get_ebp:
	movl	%ebp, %eax
	ret
