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

typedef struct MPConfigTable {
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

typedef struct MPFloatPointer {
	char signature[4];
	MPConfigTable_t *table;
	uint8_t tableLength;
	uint8_t specRev;
	uint8_t checksum;
	uint8_t features[5];
} MPFloatPointer_t;



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


#endif
