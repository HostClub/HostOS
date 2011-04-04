	.file	"sio.c"
	.section	.rodata
	.align 4
.LC0:
	.string	"** SIO line status, LSR = %02x\n"
	.align 4
.LC1:
	.string	"** SIO FIFO timeout, RXD = %02x\n"
	.align 4
.LC2:
	.string	"** SIO modem status, MSR = %02x\n"
.LC3:
	.string	"unknown device status (+100k)"
.LC4:
	.string	"_isr_sio"
.LC5:
	.string	" sio"
.LC6:
	.string	"SIO buffers:  in %d ot %d\n"
.LC7:
	.string	" in: \""
.LC8:
	.string	"\"\n"
.LC9:
	.string	" ot: \""
