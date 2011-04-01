h11275
s 00005/00000/00243
d D 1.8 11/01/31 18:20:21 wrc 8 7
c added REPORT_MYSTERY_INTS to control int 0x27 messages
e
s 00001/00001/00242
d D 1.7 11/01/20 12:33:01 wrc 7 6
c changed default_unexp to report unexpected vs. unhandled
e
s 00019/00001/00224
d D 1.6 10/11/21 15:18:55 wrc 6 5
c (wrc) added handler for "mystery" interrupt 0x27
e
s 00004/00004/00221
d D 1.5 07/04/03 14:54:36 wrc 5 4
c changed panic to __panic for consistency
e
s 00009/00013/00216
d D 1.4 06/10/14 13:12:27 wrc 4 3
c updated for sp1/sp2 compatability
e
s 00055/00042/00174
d D 1.3 05/03/15 17:30:32 wrc 3 2
c (wrc) reorganized - locals, then globally-available functions
e
s 00085/00054/00131
d D 1.2 02/05/16 15:14:54 kar 2 1
c Version 2: improvements based on my experience with 20013 506.
e
s 00185/00000/00000
d D 1.1 02/01/22 16:41:17 kar 1 0
c date and time created 02/01/22 16:41:17 by kar
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
** File:	support.c
**
D 2
** Author:	0603-506 20003
E 2
I 2
** Author:	4003-506 class of 20003
E 2
**
** Contributor:	K. Reek
**
D 2
** Description:	Initialization of the global descriptor table,
**		interrupt descriptor table, and console I/O system.
E 2
I 2
** Description:	Initialization of the global descriptor table and
**		interrupt descriptor table.  Also some support routines.
E 2
**
*/

#include "startup.h"
#include "support.h"
#include "c_io.h"
#include "x86arch.h"
I 2
#include "bootstrap.h"
E 2

/*
D 2
** Default handler for unexpected interrupts: announce the problem
** and then just loop with interrupts disabled.
E 2
I 2
D 3
** Name:	panic
E 3
I 3
** Global variables and local data types.
E 3
E 2
*/
D 2
void __default_unexpected_handler( int vector, int code ) {
	c_printf( "\nPANIC: Unhandled interrupt: vector=0x%02x, code=%d\n",
	    vector, code );
	for(;;)
E 2
I 2
D 3
void panic( char *reason ){
	asm( "cli" );
	c_printf( "\nPANIC: %s\nHalting...", reason );
	for(;;){
E 2
		;
I 2
	}
E 2
}
E 3

/*
I 3
** Name:	__isr_table
**
** Description:	This is the table that contains pointers to the
**		C-language ISR for each interrupt.  These functions are
**		called from the isr stub based on the interrupt number.
*/
void ( *__isr_table[ 256 ] )( int vector, int code );

/*
** Name:	IDT_Gate
**
** Description:	Format of an IDT entry.
*/
typedef struct	{
	short	offset_15_0;
	short	segment_selector;
	short	flags;
	short	offset_31_16;
} IDT_Gate;

/*
** LOCAL ROUTINES - not intended to be used outside this module.
*/

/*
E 3
D 2
** Default handler for interrupts we expect may occur: just reset the pic.
E 2
I 2
D 4
** Name:	__default_unexpected_handler (should be static)
E 4
I 4
** Name:	__default_unexpected_handler
E 4
**
** Arguments:	The usual ISR arguments
**
** Returns:	Nothing; it never returns (though we must declare it
**		in the usual way to avoid compilation errors).
**
** Description:	This routine catches interrupts that we do not expect
**		to ever occur.  It handles them by calling panic.
E 2
*/
D 2
void __default_expected_handler( int vector, int code ) {
E 2
I 2
D 4
void __default_unexpected_handler( int vector, int code ){
E 4
I 4
static void __default_unexpected_handler( int vector, int code ){
E 4
	c_printf( "\nVector=0x%02x, code=%d\n", vector, code );
D 5
	panic( "Unhandled interrupt" );
E 5
I 5
D 7
	__panic( "Unhandled interrupt" );
E 7
I 7
	__panic( "Unexpected interrupt" );
E 7
E 5
}

/*
D 4
** Name:	__default_expected_handler (should be static)
E 4
I 4
** Name:	__default_expected_handler
E 4
**
** Arguments:	The usual ISR arguments
**
** Returns:	The usual ISR return value
**
** Description: Default handler for interrupts we expect may occur but
**		are not handling (yet).  Just reset the PIC and return.
*/
D 4
void __default_expected_handler( int vector, int code ){
E 4
I 4
static void __default_expected_handler( int vector, int code ){
E 4
E 2
	if( vector >= 0x20 && vector < 0x30 ){
D 2
		__outport( PIC_MASTER_CMD_PORT, PIC_EOI );
E 2
I 2
		__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
E 2
		if( vector > 0x28 ){
D 2
			__outport( PIC_SLAVE_CMD_PORT, PIC_EOI );
E 2
I 2
			__outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
E 2
		}
	}
	else {
		/*
		** All the "expected" interrupts will be handled by the
		** code above.  If we get down here, the isr table may
		** have been corrupted.  Print message and don't return.
		*/
D 2
		c_printf( "\nPANIC: Unexpected \"expected\" interrupt!" );
		__default_unexpected_handler( vector, code );
E 2
I 2
D 5
		panic( "Unexpected \"expected\" interrupt!" );
E 5
I 5
		__panic( "Unexpected \"expected\" interrupt!" );
E 5
E 2
	}
}

/*
I 6
** Name:	__default_mystery_handler
**
** Arguments:	The usual ISR arguments
**
** Returns:	The usual ISR return value
**
** Description: Default handler for the "mystery" interrupt that 
**		comes through vector 0x27.  This is a non-repeatable
**		interrupt whose source has not been identified.
*/
static void __default_mystery_handler( int vector, int code ){
I 8

#ifdef REPORT_MYSTERY_INTS
E 8
	c_printf( "\nMystery interrupt!\nVector=0x%02x, code=%d\n",
		  vector, code );
I 8
#endif

E 8
	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
I 8

E 8
}

/*
E 6
D 2
** Initialize the Programmable Interrupt Controller.
E 2
I 2
** Name:	init_pic
**
** Description:	Initialize the Programmable Interrupt Controller.
E 2
*/
static void init_pic( void ){
	/*
	** ICW1
	*/
D 2
	__outport( PIC_MASTER_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );
	__outport( PIC_SLAVE_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );
E 2
I 2
	__outb( PIC_MASTER_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );
	__outb( PIC_SLAVE_CMD_PORT, PIC_ICW1BASE | PIC_NEEDICW4 );
E 2

	/*
	** ICW2: master offset of 20 in the IDT, slave offset of 28
	*/
D 2
	__outport( PIC_MASTER_IMR_PORT, 0x20 );
	__outport( PIC_SLAVE_IMR_PORT, 0x28 );
E 2
I 2
	__outb( PIC_MASTER_IMR_PORT, 0x20 );
	__outb( PIC_SLAVE_IMR_PORT, 0x28 );
E 2

	/*
	** ICW3: slave attached to line 2 of master, bit mask is 00000100
	**	 slave id is 2
	*/
D 2
	__outport( PIC_MASTER_IMR_PORT, PIC_MASTER_SLAVE_LINE );
	__outport( PIC_SLAVE_IMR_PORT, PIC_SLAVE_ID );
E 2
I 2
	__outb( PIC_MASTER_IMR_PORT, PIC_MASTER_SLAVE_LINE );
	__outb( PIC_SLAVE_IMR_PORT, PIC_SLAVE_ID );
E 2

	/*
	** ICW4
	*/
D 2
	__outport( PIC_MASTER_IMR_PORT, PIC_86MODE );
	__outport( PIC_SLAVE_IMR_PORT, PIC_86MODE );
E 2
I 2
	__outb( PIC_MASTER_IMR_PORT, PIC_86MODE );
	__outb( PIC_SLAVE_IMR_PORT, PIC_86MODE );
E 2

	/*
	** OCW1: allow interrupts on all lines
	*/
D 2
	__outport( PIC_MASTER_IMR_PORT, 0x00 );
	__outport( PIC_SLAVE_IMR_PORT, 0x00 );
E 2
I 2
	__outb( PIC_MASTER_IMR_PORT, 0x00 );
	__outb( PIC_SLAVE_IMR_PORT, 0x00 );
E 2
}

/*
D 2
** Create an IDT entry.
E 2
I 2
D 3
** Name:	IDT_Gate
**
** Description:	Format of an IDT entry.
E 2
*/
typedef struct	{
	short	offset_15_0;
	short	segment_selector;
	short	flags;
	short	offset_31_16;
} IDT_Gate;

D 2
IDT_Gate *idt_base = (IDT_Gate *)IDT_PADDR;
E 2
I 2
/*
E 3
** Name:	set_idt_entry
**
** Description:	Construct an entry in the IDT
** Arguments:	The entry number (vector number), and a pointer to the
**		stub (NOT the ISR routine) that handles that interrupt.
*/
D 3
void set_idt_entry( int entry, void ( *handler )( void ) ){
E 3
I 3
static void set_idt_entry( int entry, void ( *handler )( void ) ){
E 3
	IDT_Gate *g = (IDT_Gate *)IDT_ADDRESS + entry;
E 2

D 2
static void set_idt_entry( int entry, void ( *handler )( void ) ) {
	IDT_Gate *g = idt_base + entry;

E 2
	g->offset_15_0 = (int)handler & 0xffff;
	g->segment_selector = 0x0010;
	g->flags = IDT_PRESENT | IDT_DPL_0 | IDT_INT32_GATE;
	g->offset_31_16 = (int)handler >> 16 & 0xffff;
}

D 3
/*
D 2
** This is the table that contains pointers to the C-language ISR
** for each interrupt.  These functions are called from the isr stub
** based on the interrupt number.
E 2
I 2
** Name:	__isr_table
**
** Description:	This is the table that contains pointers to the
**		C-language ISR for each interrupt.  These functions are
**		called from the isr stub based on the interrupt number.
E 2
*/
void ( *__isr_table[ 256 ] )( int vector, int code );
E 3

/*
D 2
** Install a new interrupt service routine for an interrupt.  A pointer
** to the previous routine is returned so you can revert to it later.
E 2
I 2
D 3
** Name:	__install_isr
E 2
*/
void ( *__install_isr( int vector, void ( *handler )( int vector, int code ) ) )( int vector, int code ){
	void	( *old_handler )( int vector, int code );

	old_handler = __isr_table[ vector ];
	__isr_table[ vector ] = handler;
	return old_handler;
}

/*
D 2
** Initialize the Interrupt Descriptor Table (IDT).  This makes
** each of the entries in the IDT point to the isr stub for that entry,
** and installs a default handler in the handler table.
E 2
I 2
** Name:	init_idt (should be static)
E 3
I 3
** Name:	init_idt
E 3
**
** Description: Initialize the Interrupt Descriptor Table (IDT).  This
**		makes each of the entries in the IDT point to the isr stub
**		for that entry, and installs a default handler in the
**		handler table.  Specific handlers are then installed for
**		those interrupts we may get before a real handler is set up.
E 2
*/
D 2
void init_idt( void ) {
E 2
I 2
D 3
void init_idt( void ){
E 3
I 3
static void init_idt( void ){
E 3
E 2
	int i;
	extern	void	( *__isr_stub_table[ 256 ] )( void );

	/*
	** Make each IDT entry point to the stub for that vector.
	** Also make each entry in the ISR table point to the default handler.
	*/
D 2
	for ( i=0; i < 256; i++ ) {
E 2
I 2
	for ( i=0; i < 256; i++ ){
E 2
		set_idt_entry( i, __isr_stub_table[ i ] );
		__install_isr( i, __default_unexpected_handler );
	}

	/*
	** Install the handlers for interrupts that have a specific handler.
	*/
	__install_isr( INT_VEC_KEYBOARD, __default_expected_handler );
D 6
	__install_isr( INT_VEC_TIMER, __default_expected_handler );
E 6
I 6
	__install_isr( INT_VEC_TIMER,    __default_expected_handler );
	__install_isr( INT_VEC_MYSTERY,  __default_mystery_handler );
E 6
}

/*
I 3
** END OF LOCAL ROUTINES.
*/

/*
D 5
** Name:	panic
E 5
I 5
** Name:	__panic
E 5
*/
D 5
void panic( char *reason ){
E 5
I 5
void __panic( char *reason ){
E 5
	asm( "cli" );
	c_printf( "\nPANIC: %s\nHalting...", reason );
	for(;;){
		;
	}
}

/*
E 3
D 2
** Initialize the interrupt system.  This includes initializing the
** IDT, the PIC, and the console I/O system.  It is up to the user
** to enable processor interrupts when they're ready.
E 2
I 2
D 4
** Name:	init_interrupts
E 4
I 4
** Name:	__init_interrupts
E 4
**
D 4
** Description:	Initialize the interrupt system.  This includes initializing
**		the IDT and the PIC.  It is up to the user to enable
**		processor interrupts when they're ready.
E 4
I 4
** Description:	(Re)initialize the interrupt system.  This includes
**		initializing the IDT and the PIC.  It is up to the
**		user to enable processor interrupts when they're ready.
E 4
E 2
*/
D 2
void init( void ) {
E 2
I 2
D 4
void init_interrupts( void ){
E 2
	/*
	** Initialize the interrupt descriptor table and the
	** PIC (programmable interrupt controller).
	*/
E 4
I 4
void __init_interrupts( void ){
E 4
	init_idt();
	init_pic();
D 2

	/*
	** Now initialize the console I/O system.
	*/
	c_io_init();
E 2
}

I 3
/*
** Name:	__install_isr
*/
void ( *__install_isr( int vector, void ( *handler )( int vector, int code ) ) )( int vector, int code ){
	void	( *old_handler )( int vector, int code );

	old_handler = __isr_table[ vector ];
	__isr_table[ vector ] = handler;
	return old_handler;
}

E 3
/*
D 2
** This function simply delays execution by counting for a while.
** The count was chosen to give approximately 1/10th of a second of delay.
** The interrupt state is not modified by this function: if interrupts
** are enabled, they will be serviced while the delay is going on.
E 2
I 2
** Name:	__delay
E 2
*/
void __delay( int tenths ){
	int	i;

	while( --tenths >= 0 ){
		for( i = 0; i < 10000000; i += 1 )
			;
	}
}
E 1
