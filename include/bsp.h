#ifndef BSP_H
#define BSP_H

#include "headers.h"

#define VENDOR_ID      0x00000000
#define FEATURE_INFO   0x00000001
#define D_CACHE_PARAM  0x00000004
#define EXT_FUNCS      0x80000000
#define EXT_FEATURES   0x80000001
#define BRAND_STRING1  0x80000002
#define BRAND_STRING2  0x80000003
#define BRAND_STRING3  0x80000004

#define SIG_I7    0x106A5
#define SIG_CORE2 0x1067A

#define BIOSROM_LOW      0x0E0000
#define BIOSROM_HIGH     0x100000
#define EBDA_BASE        0x09FC00
#define EBDA_POINTER     0x00040E
#define TOPOFMEM_POINTER 0x000413
#define DEFAULT_TOPOFMEM 0x0A0000
#define MP_SIG           0x5f504d5f    /* _MP_ */

#define DEFAULT_LAPIC_ADDRESS  0xFEE00000

typedef struct _MPConfigTable {
	char signature[4];
	uint16_t baseTableLength;
	uint8_t specRev;
	uint8_t checksum;
	char oemID[8];
	char productID[12];
	void *oemTable;
	uint16_t oemTableSize;
	uint16_t entryCount;
	void *localAPIC;
	uint16_t extendedTableLength;
	uint8_t extendedTableChecksum;
	uint8_t reserved;
} MPConfigTable_t;

typedef struct _MPFloatPointer {
	char signature[4];
	MPConfigTable_t *table;
	uint8_t tableLength;
	uint8_t specRev;
	uint8_t checksum;
	uint8_t features[5];
} MPFloatPointer_t;

#define PAD3 uint32_t : 32; uint32_t : 32; uint32_t : 32
#define PAD4 uint32_t : 32; uint32_t : 32; uint32_t : 32; uint32_t : 32

typedef struct __attribute__((packed)) _LAPIC {
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	uint32_t id;           PAD3;
	uint32_t versionLVT;   PAD3;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	uint32_t tpr;          PAD3;
	uint32_t apr;          PAD3;
	uint32_t ppr;          PAD3;
	uint32_t eoi;          PAD3;
	/* reserved */         PAD4;
	uint32_t ldr;          PAD3;
	uint32_t dfr;          PAD3;
	uint32_t svr;          PAD3;
	uint32_t isr0;         PAD3;
	uint32_t isr1;         PAD3;
	uint32_t isr2;         PAD3;
	uint32_t isr3;         PAD3;
	uint32_t isr4;         PAD3;
	uint32_t isr5;         PAD3;
	uint32_t isr6;         PAD3;
	uint32_t isr7;         PAD3;
	uint32_t tmr0;         PAD3;
	uint32_t tmr1;         PAD3;
	uint32_t tmr2;         PAD3;
	uint32_t tmr3;         PAD3;
	uint32_t tmr4;         PAD3;
	uint32_t tmr5;         PAD3;
	uint32_t tmr6;         PAD3;
	uint32_t tmr7;         PAD3;
	uint32_t irr0;         PAD3;
	uint32_t irr1;         PAD3;
	uint32_t irr2;         PAD3;
	uint32_t irr3;         PAD3;
	uint32_t irr4;         PAD3;
	uint32_t irr5;         PAD3;
	uint32_t irr6;         PAD3;
	uint32_t irr7;         PAD3;
	uint32_t esr;          PAD3;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	uint32_t icr_lo;       PAD3;
	uint32_t icr_hi;       PAD3;
	uint32_t lvt_timer;    PAD3;
	/* reserved */         PAD4;
	uint32_t lvt_pcint;    PAD3;
	uint32_t lvt_lint0;    PAD3;
	uint32_t lvt_lint1;    PAD3;
	uint32_t lvt_error;    PAD3;
	uint32_t icr_timer;    PAD3;
	uint32_t ccr_timer;    PAD3;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	/* reserved */         PAD4;
	uint32_t dcr_timer;    PAD3;
	/* reserved */         PAD4;
} LAPIC_t;

typedef struct _IPICommand {
	struct IPICommandUpper {
		uint8_t destination : 8;
		uint32_t : 24;
	} upper;

	struct IPICommandLower {
		uint32_t : 12;
		uint8_t shorthand : 2;
		uint8_t : 2;
		uint8_t triggerMode : 1;
		uint8_t level : 1;
		uint8_t deliveryStatus : 1;
		uint8_t : 1;
		uint8_t destMode : 1;
		uint8_t deliveryMode : 3;
		uint8_t vector : 8;
	} lower;
} IPICommand_t;

uint8_t inline get_lapic_version(LAPIC_t* lapic);
uint8_t inline get_lapic_maxLVT(LAPIC_t* lapic);

void inline send_IPI(IPICommand_t command);

void inline write_eflags(uint32_t a);

uint32_t inline read_eflags(void);

void inline cpuid_data_with_param(uint32_t command, uint32_t param, uint32_t *data);

void inline cpuid_data(uint32_t command, uint32_t *data);

uint32_t inline cpuid_value(uint32_t command);

void inline unscramble_cpuid(uint32_t *data);

void checkCPUs(void);

MPFloatPointer_t *findMPFPS(void);

void strncpy(char *, char *, int);

void checkCPUs(void);



extern LAPIC_t *local_apic;


#endif
