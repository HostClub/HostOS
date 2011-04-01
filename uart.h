/* File:	uart.h
** Author:	M. Reek
** Contributors: K. Reek, W. Carithers
**
** Description: Definitions for the 16540/16550 compatible UART1 on the
**		PCs in the DSL.  Mask definitions are taken from the
**		manual for the National Semiconductor PC87309 SuperI/O
**		Plug & Play Compatible Chip.
**
**		The naming convention is UAx_yyy_zzzzz.  "x" is either 4 or 5
**		(see below), "yyy" is the name of the register to which this
**		value applies, and "zzzzz" is the name of the value.
**
**		Note: the UA4 prefix denotes 16540 compatible functions,
**		and UA5 is for 16550 extensions (primarily the FIFOs).
**
**		For many items there are two names: one short one that
**		matches the name in the chip manual, and another that is more
**		readable to use in coding.
**
** SCCS ID:	@(#)uart.h	1.2	01/27/08
*/

#ifndef	_UART_H
#define _UART_H


/*
** Base port number assigned to the device
*/
#define UA4_COM1_IOADDR		0x3f8
#define UA4_COM2_IOADDR		0x2f8
#define UA4_COM3_IOADDR		0x3e8
#define UA4_COM4_IOADDR		0x2e8

#define	UA4_PORT		UA4_COM1_IOADDR

/*
*******************************************************************
** BANK 0 REGISTERS 
*******************************************************************
*/

/*
** Receiver data
*/
#define UA4_RXD			(UA4_PORT+0) 	/* receiver data port */
#define UA4_RX_DATA		UA4_RXD

/*
** Transmitter data
*/
#define UA4_TXD			(UA4_PORT+0) 	/* transmitter data port */
#define UA4_TX_DATA		UA4_TXD

/*
** Interrupt Enable
*/
#define UA4_IER			(UA4_PORT+1)  	/* interrupt enable register */
#define UA4_INT_ENABLE_REG	UA4_IER

#define UA4_IER_RXHDL_IE	0x01	/* Rcvr High-Data-Level Int Enable */
#define UA4_IER_RX_INT_ENABLE	UA4_IER_RXHDL_IE
#define UA4_IER_TXLDL_IE	0x02	/* Xmitter Low-data-level Int Enable */
#define UA4_IER_TX_INT_ENABLE	UA4_IER_TXLDL_IE
#define UA4_IER_LS_IE		0x04	/* Line Status Int Enable */
#define UA4_IER_LINE_STATUS_INT_ENABLE	UA4_IER_LS_IE
#define UA4_IER_MS_IE		0x08	/* Modem Status Int Enable */
#define UA4_IER_MODEM_STATUS_INT_ENABLE	UA4_IER_MS_IE


/*
** Event Identification
*/
#define UA4_EIR			(UA4_PORT+2)   	/* event ident register */
#define UA4_EVENT_ID		UA4_EIR

#define UA4_EIR_IPF		0x01	/* Interrupt Pending */
#define UA4_EIR_INT_PENDING	UA4_EIR_IPF
#define UA4_EIR_IPR0		0x02	/* Interrupt Priority bit 0 */
#define UA4_EIR_IPR1		0x04	/* Interrupt Priority bit 1 */
#define	UA4_EIR_PRI		(UA4_EIR_IPR0|UA4_EIR_IPR1)
#define UA5_EIR_RXFT		0x08	/* RX_FIFO Timeout */
#define UA5_EIR_RX_FIFO_TIMEOUT	UA5_EIR_RXFT
#define UA5_EIR_FEN0		0x40	/* FIFOs Enabled */
#define UA5_EIR_FIFO_ENABLED_0	UA5_EIR_FEN0
#define UA5_EIR_FEN1		0x80	/* FIFOs Enabled */
#define UA5_EIR_FIFO_ENABLED_1	UA5_EIR_FEN1

/* EIR interrupt priorities */

#define	UA4_EIR_INT_PRI_MASK	0x0f	/* Mask for extracting int priority */
#define UA4_EIR_NO_INT		0x01	/* no interrupt */
#define UA4_EIR_NO_INT_PENDING	UA4_EIR_NO_INT
#define UA4_EIR_LINE_STATUS	0x06	/* line status interrupt */
#define UA4_EIR_LINE_STATUS_INT_PENDING	UA4_EIR_LINE_STATUS
#define UA4_EIR_RX_HIGH		0x04	/* Receiver High Data Level event */
#define UA4_EIR_RX_INT_PENDING	UA4_EIR_RX_HIGH
#define UA5_EIR_RX_FIFO_TO	0x0c	/* Receiver FIFO timeout */
#define UA5_EIR_RX_FIFO_TIMEOUT_INT_PENDING UA5_EIR_RX_FIFO_TO
#define UA4_EIR_TX_LOW		0x02	/* Transmitter Lower Data level Event*/
#define UA4_EIR_TX_INT_PENDING	UA4_EIR_TX_LOW
#define UA4_EIR_MODEM_STATUS	0x00	/* Modem Status */
#define UA4_EIR_MODEM_STATUS_INT_PENDING UA4_EIR_MODEM_STATUS

/*
** FIFO Control
*/
#define UA4_FCR			(UA4_PORT+2)   	/* FIFO Control register - 16550*/
#define UA4_FIFO_CTL		UA4_FCR

#define UA5_FCR_FIFO_RESET	0x00	/* Reset the FIFO */
#define UA5_FCR_FIFO_EN		0x01	/* FIFO Enable */
#define UA5_FCR_FIFO_ENABLED	UA5_FCR_FIFO_EN
#define UA5_FCR_RXSR 		0x02	/* Receiver Soft Reset */
#define UA5_FCR_RX_SOFT_RESET	UA5_FCR_RXSR 
#define UA5_FCR_TXSR		0x04	/* Transmitter Soft Reset */
#define UA5_FCR_TX_SOFT_RESET	UA5_FCR_TXSR
#define UA5_FCR_TXFTH0		0x10	/* TX_FIFO threshold level */
#define UA5_FCR_TXFTH1		0x20	/* TX_FIFO threshold level */
#define	UA5_FCR_TXFTH		(UA5_FCR_TXFTH1|UA5_FCR_TXFTH0)
#define UA5_FCR_RXFTH0		0x40	/* RX_FIFO threshold level */
#define UA5_FCR_RXFTH1		0x80	/* RX_FIFO threshold level */
#define	UA5_FCR_RXFTH		(UA5_FCR_RXFH1|UA5_FCR_RXFH0)

/* FIFO thresholds */

/* Interrupt when transmitter FIFO gets this many characters */

#define UA5_FCR_TX_FIFO_1	0x00	/* TX_FIFO threshold = 1 char */
#define UA5_FCR_TX_FIFO_1_CHAR	UA5_FCR_TX_FIFO_1
#define UA5_FCR_TX_FIFO_3	0x10	/* TX_FIFO threshold = 3 char */
#define UA5_FCR_TX_FIFO_3_CHAR	UA5_FCR_TX_FIFO_3
#define UA5_FCR_TX_FIFO_9	0x20	/* TX_FIFO threshold = 9 char */
#define UA5_FCR_TX_FIFO_9_CHAR	UA5_FCR_TX_FIFO_9
#define UA5_FCR_TX_FIFO_13	0x30	/* TX_FIFO threshold = 13 char */
#define UA5_FCR_TX_FIFO_13_CHAR	UA5_FCR_TX_FIFO_13

/* Interrupt when receiver FIFO gets this many characters */

#define UA5_FCR_RX_FIFO_1	0x00	/* RX_FIFO threshold = 1 char */
#define UA5_FCR_RX_FIFO_1_CHAR	UA5_FCR_RX_FIFO_1
#define UA5_FCR_RX_FIFO_4	0x40	/* RX_FIFO threshold = 4 char */
#define UA5_FCR_RX_FIFO_4_CHAR	UA5_FCR_RX_FIFO_4
#define UA5_FCR_RX_FIFO_8	0x80	/* RX_FIFO threshold = 8 char */
#define UA5_FCR_RX_FIFO_8_CHAR	UA5_FCR_RX_FIFO_8
#define UA5_FCR_RX_FIFO_14	0xc0	/* RX_FIFO threshold = 14 char */
#define UA5_FCR_RX_FIFO_14_CHAR	UA5_FCR_RX_FIFO_14

/*
** Line Control (available in all banks)
*/
#define UA4_LCR			(UA4_PORT+3)   	/* line control register */
#define UA4_LINE_CTL		UA4_LCR

#define UA4_LCR_WLS0		0x01	/* Character Length select */
#define UA4_LCR_WLS1		0x02	/* Character Length select */
#define	UA4_LCR_WLS		(UA4_LCR_WLS0|UA4_LCR_WLS1)
#define UA4_LCR_STB		0x04	/* Stop Bits */
#define UA4_LCR_STOP_BITS	UA4_LCR_STB
#define UA4_LCR_PEN		0x08 	/* Parity Enable */
#define UA4_LCR_PARITY_ENABLE	UA4_LCR_PEN
#define UA4_LCR_EPS		0x10	/* Even Parity Select */
#define UA4_LCR_STKP		0x20	/* Sticky Parity */
#define UA4_LCR_SBRK		0x40	/* Set Break */
#define UA4_LCR_SET_BREAK	UA4_LCR_SBRK
#define UA4_LCR_BKSE		0x80	/* Bank Select Enable */
#define UA4_LCR_BANK_SELECT_ENABLE UA4_LCR_BKSE

#define UA4_LCR_BITS_5		0x00	/* 5 bits per char */
#define UA4_LCR_BITS_6		0x01	/* 6 bits per char */
#define UA4_LCR_BITS_7		0x02	/* 7 bits per char */
#define UA4_LCR_BITS_8		0x03	/* 8 bits per char */

#define UA4_LCR_1_STOP_BIT	0x00
#define UA4_LCR_2_STOP_BIT	0x01

#define UA4_LCR_NO_PARITY	0x00	
#define UA4_LCR_ODD_PARITY	UA4_LCR_PEN
#define UA4_LCR_EVEN_PARITY	(UA4_LCR_PEN|UA4_LCR_EPS)
#define UA4_LCR_PARITY_LOGIC_1	(UA4_LCR_PEN|UA4_LCR_STKP)
#define UA4_LCR_PARITY_LOGIC_0	(UA4_LCR_PEN|UA4_LCR_EPS|UA4_LCR_STKP)


/*
** Bank Select (available in all banks)
*/
#define UA4_BSR			(UA4_PORT+3)   	/* bank select register */
#define UA4_BANK_SELECT		UA4_BSR

#define UA4_BSR_BANK2		0xd0	/* Bank 2 selected, LCR not written */
#define UA4_BSR_BANK3		0xd4	/* Bank 2 selected, LCR not written */
#define UA4_LCR_BANK0		0x00	/* Bank 0 selected, LCR written */
#define UA4_LCR_BANK1		0x80	/* Bank 1 selected, LCR written */

/*
** Modem Control
*/
#define UA4_MCR			(UA4_PORT+4)   	/* modem control register */
#define UA4_MODEM_CTL		UA4_MCR

#define UA4_MCR_DTR		0x01	/* Data Terminal Ready */
#define UA4_MCR_DATA_TERMINAL_READY UA4_MCR_DTR
#define UA4_MCR_RTS		0x02	/* Ready to Send */
#define UA4_MCR_READY_TO_SEND	UA4_MCR_RTS
#define UA4_MCR_RILP		0x04	/* Loopback Interrupt Request */
#define UA4_MCR_LOOPBACK_INT_REQ UA4_MCR_RILP
#define UA4_MCR_ISEN		0x08	/* Interrupt Signal Enable */
#define UA4_MCR_INT_SIGNAL_ENABLE UA4_MCR_ISEN
#define UA4_MCR_DCDLP		0x08	/* DCD Loopback */
#define UA4_MCR_LOOPBACK_DCD	UA4_MCR_DCDLP
#define UA4_MCR_LOOP		0x10	/* Loopback Enable */
#define UA4_MCR_LOOPBACK_ENABLE	UA4_MCR_LOOP

/*
** Line Status
*/
#define UA4_LSR			(UA4_PORT+5)   	/* line status register */
#define UA4_LINE_STATUS		UA4_LSR

#define UA4_LSR_RXDA		0x01	/* Receiver Data Available */
#define UA4_LSR_RX_DATA_AVAILABLE UA4_LSR_RXDA
#define UA4_LSR_OE		0x02	/* Overrun Error */
#define UA4_LSR_OVERRUN_ERROR	UA4_LSR_OE
#define UA4_LSR_PE		0x04	/* Parity Error */
#define UA4_LSR_PARITY_ERROR	UA4_LSR_PE
#define UA4_LSR_FE		0x08	/* Framing Error */
#define UA4_LSR_FRAMING_ERROR	UA4_LSR_FE
#define UA4_LSR_BRK		0x10	/* Break Event Detected */
#define UA4_LSR_BREAK_DETECTED	UA4_LSR_BRK
#define UA4_LSR_TXRDY		0x20	/* Transmitter Ready */
#define UA4_LSR_TX_READY	UA4_LSR_TXRDY
#define UA4_LSR_TXEMP		0x40	/* Transmitter Empty */
#define UA4_LSR_TX_EMPTY	UA4_LSR_TXEMP
#define UA4_LSR_ER_INF		0x80	/* Error in RX_FIFO */
#define UA4_LSR_RX_FIFO_ERROR	UA4_LSR_ER_INF

/*
** Modem Status
*/
#define UA4_MSR			(UA4_PORT+6)   	/* modem status register */
#define UA4_MODEM_STATUS	UA4_MSR

#define UA4_MSR_DCTS		0x01	/* Delta Clear to Send */
#define UA4_MSR_DELTA_CLEAR_TO_SEND UA4_MSR_DCTS
#define UA4_MSR_DDSR		0x02	/* Delta Data Set Ready */
#define UA4_MSR_DELTA_DATA_SET_READY UA4_MSR_DDSR
#define UA4_MSR_TERI		0x04	/* Trailing Edge Ring Indicate */
#define UA4_MSR_TRAILING_EDGE_RING UA4_MSR_TERI
#define UA4_MSR_DDCD		0x08	/* Delta Data Carrier Detect */
#define UA4_MSR_DELTA_DATA_CARRIER_DETECT UA4_MSR_DDCD
#define UA4_MSR_CTS 		0x10	/* Clear to Send */
#define UA4_MSR_CLEAR_TO_SEND	UA4_MSR_CTS
#define UA4_MSR_DSR 		0x20	/* Data Set Ready */
#define UA4_MSR_DATA_SET_READY	UA4_MSR_DSR
#define UA4_MSR_RI 		0x40	/* Ring Indicate */
#define UA4_MSR_RING_INDICATE	UA4_MSR_RI
#define UA4_MSR_DCD 		0x80	/* Data Carrier Detect */
#define UA4_MSR_DATA_CARRIER_DETECT UA4_MSR_DCD

/*
** Scratchpad
*/
#define UA4_UA5_SCR		(UA4_PORT+7)	/* scratch register for 16550 */
#define UA4_UA5_SCRATCH		UA4_UA5_SCR


/*
******************************************************************
** BANK 1 REGISTERS
******************************************************************
*/

#define UA4_LBGD_L		(UA4_PORT+0x00)	/* Baud Divisor (low byte) */
#define UA4_LBGD_H		(UA4_PORT+0x01)	/* Baud Divisor (high byte) */

/* Baud rate generator divisor settings */

#define	BAUD_HIGH_BYTE(x)	( (x) >> 8 )
#define	BAUD_LOW_BYTE(x)	( (x) & 0xff )

#define	BAUD_50			2304
#define	BAUD_75			1536
#define	BAUD_110		1047
#define	BAUD_150		768
#define	BAUD_300		384
#define	BAUD_600		192
#define	BAUD_1200		96
#define	BAUD_1800		64
#define	BAUD_2000		58
#define	BAUD_2400		48
#define	BAUD_3600		32
#define	BAUD_4800		24
#define	BAUD_7200		16
#define	BAUD_9600		12
#define	BAUD_14400		8
#define	BAUD_19200		6
#define	BAUD_28800		4
#define	BAUD_38400		3
#define	BAUD_57600		2
#define	BAUD_115200		1


#endif		/* uart.h */
