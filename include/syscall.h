/*
** SCCS ID:	@(#)syscall.h	1.1	03/31/11
**
** File:	syscall.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	System call information
*/

#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#include <x86arch.h>

// system call codes

#define	SYS_exit	0
#define	SYS_fork	1
#define	SYS_exec	2
#define	SYS_wait	3
#define	SYS_kill	4
#define	SYS_sleep	5
#define	SYS_readc	6
#define	SYS_reads	7
#define	SYS_writec	8
#define	SYS_writes	9
#define	SYS_getpid	10
#define	SYS_getppid	11
#define	SYS_getprio	12
#define	SYS_gettime	13
#define	SYS_setprio	14
#define	SYS_settime	15

// number of "real" system calls

#define	N_SYSCALLS	16

// dummy system call code to test the syscall ISR

#define	SYS_bogus	0xbadc0de

// syscall interrupt vector number

#define	INT_VEC_SYSCALL	0x80

// default contents of the EFLAGS register

#define	DEFAULT_EFLAGS	(EFLAGS_MB1 | EFLAGS_IF)

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _isr_syscall()
**
** system call ISR
*/

void _isr_syscall( int vector, int code );

/*
** _syscall_init()
**
** initializes all syscall-related data structures
*/

void _syscall_init( void );

#endif

#endif
