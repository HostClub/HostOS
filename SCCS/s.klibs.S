h36811
s 00024/00000/00000
d D 1.1 11/03/31 18:06:25 wrc 1 0
c date and time created 11/03/31 18:06:25 by wrc
e
u
U
f e 0
t
T
I 1
/*
** sccs id:	%W%	%G%
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
E 1
