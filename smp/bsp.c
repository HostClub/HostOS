#include "headers.h"
#include "bsp.h"

void inline write_eflags(uint32_t a) {
	asm("push  %%eax;\
	     popfl;"
	    :
	    : "a"(a)
	   );
}

uint32_t inline read_eflags() {
	uint32_t a;
	asm("pushfl;\
	     pop  %%eax;"
	    : "=a"(a)
	    :
	   );
	return a;
}

void inline cpuid_data_with_param(uint32_t command, uint32_t param, uint32_t *data) {
	asm("cpuid;"
	    : "=a"(data[0]), "=b"(data[1]), "=c"(data[2]), "=d"(data[3])
	    : "a"(command), "c"(param)
	   );
}

void inline cpuid_data(uint32_t command, uint32_t *data) {
	cpuid_data_with_param(command, 0, data);
}

uint32_t inline cpuid_value(uint32_t command) {
	uint32_t a;
	asm("cpuid;"
	    : "=a"(a)\
	    : "a"(command)\
	   );
	return a;
}

void inline unscramble_cpuid(uint32_t *data) {
	//Swap the third and fourth registers (to undo the backward compatibility)
	data[2] ^= data[3];
	data[3] ^= data[2];
	data[2] ^= data[3];
}

void strncpy(char *src, char *dst, int len) {
	dst[len] = 0;
	while (len--) {
		dst[len] = src[len];
	}
}

MPFloatPointer_t *findMPFPS() {
	//check to see is the extended BIOS data area is defined
	uint32_t *ebdaBase = (uint32_t *)EBDA_POINTER;
	c_printf("Checking EBDA base @ 0x%x: 0x%x\n", EBDA_POINTER, *ebdaBase);
	if (ebdaBase) {
		//search for the floating pointer structure in the extended BIOS data area
		*ebdaBase = *ebdaBase << 4;
		c_printf("EBDA defined to be: 0x%x\n", *ebdaBase);
		uint32_t *loc = (uint32_t *)*ebdaBase;
		c_printf("Checking the EBDA between 0x%x and 0x%x\n", loc, *ebdaBase + 0x0400 - 4);
		while ((uint32_t)loc < *ebdaBase + 0x0400 - 4) {
			//c_putchar(*loc);
			if (*loc == MP_SIG) {
				return (MPFloatPointer_t *)loc;
			}
			loc += 1;
		}
	} else {
		//search for the structure in the last 1KB of system base memory
		c_printf("No EBDA defined\n");

	}


	c_printf("Checking the BIOS ROM (0x%x - 0x%x)\n", BIOSROM_LOW, BIOSROM_HIGH);
	//search for the structure in BIOS ROM
	uint32_t *loc = (uint32_t *)BIOSROM_LOW;
	while ((uint32_t)loc < BIOSROM_HIGH - 4) {
		if (*loc == MP_SIG) {
			return (MPFloatPointer_t *)loc;
		}
		loc += 1;
	}

	uint32_t *topMem = (uint32_t *)TOPOFMEM_POINTER;
	c_printf("Checking top of memory pointer @ 0x%x: 0x%x\n", TOPOFMEM_POINTER, *topMem);
	if (topMem) {
		//search for the floating pointer structure in the extended BIOS data area
		//(*topMem)++;
		(*topMem) *= 1024;
		c_printf("Top of memory defined to be: 0x%x\n", *topMem);
		uint32_t *loc = (uint32_t *)*topMem;
		c_printf("Checking the top of memory between 0x%x and 0x%x\n", loc, *topMem + 0x0400 - 4);
		while ((uint32_t)loc < *topMem + 0x0400 - 4) {
			//c_putchar(*loc);
			if (*loc == MP_SIG) {
				return (MPFloatPointer_t *)loc;
			}
			loc += 1;
		}
	}

	c_printf("Checking the default top of memory (0x%x - 0x%x)\n", DEFAULT_TOPOFMEM, DEFAULT_TOPOFMEM - 1024);
	//search for the structure in BIOS ROM
	loc = (uint32_t *)DEFAULT_TOPOFMEM - 1024;
	while ((uint32_t)loc < DEFAULT_TOPOFMEM - 4) {
		if (*loc == MP_SIG) {
			return (MPFloatPointer_t *)loc;
		}
		loc += 1;
	}

	return NULL;
}

void checkCPUs() {
	uint32_t id;
	int i;
	uint32_t data[5];
	uint32_t maxCpuIdOp;

	id = read_eflags();
	write_eflags(id | 0x200000);
	id = read_eflags();
	id = (id >> 21) & 1;

	if (!id) {
		c_puts("CPUID not supported!!!\n");
		return;
	}


	cpuid_data(VENDOR_ID, data);
	unscramble_cpuid(data);
	maxCpuIdOp = data[0];
	//Null terminate the string
	data[4] = 0;

	c_printf( "Max CPUID Op: %d\n", maxCpuIdOp );
	c_printf( "CPU Vendor: '%s'\n", (char *)(data + 1) );

	id = cpuid_value(EXT_FUNCS);
	if (id >= BRAND_STRING3) {
		//Supports extended functions
		cpuid_data(BRAND_STRING1, data);
		c_printf("%s", data);
		cpuid_data(BRAND_STRING2, data);
		c_printf("%s", data);
		cpuid_data(BRAND_STRING3, data);
		c_printf("%s\n", data);

		cpuid_data(EXT_FEATURES, data);
		c_printf("Extended Features (ECX):  0x%x\n", *((uint32_t *)(data + 8)));
		c_printf("Extended Features (EDX):  0x%x\n", *((uint32_t *)(data + 12)));
	} else {
		//Doesn't support extended functions
		cpuid_data(FEATURE_INFO, data);
		unscramble_cpuid(data);
		id = *((uint32_t *)data) & 0x0FFF3FFF;

		c_printf("Signature:       0x%x (", id);
		switch (id) {
			case SIG_I7:
				c_puts("Core i7 Processor)\n");
				break;
			case SIG_CORE2:
				c_puts("Core 2 Extreme Processor)\n");
				break;
			default:
				c_puts("Unknown Processor)\n");
		}
	}

	cpuid_data(FEATURE_INFO, data);
	unscramble_cpuid(data);
	id = *((uint32_t *)data) & 0x0FFF3FFF;

	char steppingID =     (id & 0x0000000F) >> 0;
	char modelNumber =    (id & 0x000000F0) >> 4;
	char familyCode =     (id & 0x00000F00) >> 8;
	char type =           (id & 0x00003000) >> 12;
	char extendedModel =  (id & 0x000F0000) >> 16;
	char extendedFamily = (id & 0x0FF00000) >> 20;

	c_printf("Features (ECX): 0x%x\n", *((uint32_t *)data + 8));
	c_printf("Features (EDX): 0x%x\n", *((uint32_t *)data + 12));

	c_printf("Stepping ID:     %d\n", steppingID);
	c_printf("Model Number:    %d\n", modelNumber);
	c_printf("Family Code:     %d\n", familyCode);
	c_printf("Type:            %d\n", type);
	c_printf("Extended Model:  %d\n", extendedModel);
	c_printf("Extended Family: %d\n", extendedFamily);

	id = *((uint32_t *)(data + 4));
	c_printf("\nLogical Processors: %d\n", (id & 0x00FF0000) >> 16);


	// Enumerate the deterministic cache parameters
	cpuid_data_with_param(D_CACHE_PARAM, 0, data);
	unscramble_cpuid(data);

	id = *((uint32_t *)(data));
	c_printf("APIC IDs:           %d\n", 1 + ((id & 0xFC000000) >> 26));

	i = 0;
	while ((id & 0x0F) != 0) {
		char sharingThreads = ((id & 0x03FFC000) >> 14) + 1;
		char level = (id & 0xF0) >> 4;
		char type = (id & 0x0F);

		c_printf("Cache Number: %d   ", i);
		c_printf("  Threads sharing this cache: %d   ", sharingThreads);
		c_printf("  Level: %d   ", level);
		c_printf("  Type: ");
		switch (type) {
			case 1:
				c_printf("Data");
				break;
			case 2:
				c_printf("Instruction");
				break;
			case 3:
				c_printf("Unified");
				break;
			default:
				c_printf("INVALID");
		}

		c_putchar('\n');

		i++;
		cpuid_data_with_param(D_CACHE_PARAM, i, data);
		id = *((uint32_t *)(data));
	}

	char temp[13];

	MPFloatPointer_t *mpStruct = findMPFPS();

	if (mpStruct) {
		c_printf("MP Floating Pointer\n");
		strncpy(mpStruct->signature, temp, 4);
		c_printf("  Signature:     %s\n", temp);
		c_printf("  Table Address: 0x%x\n", mpStruct->table);
		c_printf("  Table Length:  0x%x\n", mpStruct->tableLength * 16);
		c_printf("  Revision:      0x%x\n", mpStruct->specRev);
		c_printf("  Checksum:      0x%x\n", mpStruct->checksum);
		c_printf("  Features:      0x%x 0x%x 0x%x 0x%x 0x%x\n", mpStruct->features[0], mpStruct->features[1], mpStruct->features[2], mpStruct->features[3], mpStruct->features[4]);

		c_printf("MP Configuration Table\n");
		MPConfigTable_t *config = mpStruct->table;

		strncpy(config->signature, temp, 4);
		c_printf("  Signature:               %s\n", temp);
		strncpy(config->oemID, temp, 8);
		c_printf("  OEM ID:                  %s\n", temp);
		strncpy(config->productID, temp, 12);
		c_printf("  Product ID:              %s\n", temp);
		c_printf("  Revision:                0x%x\n", config->specRev);
		c_printf("  Base Table Length:       0x%x\n", config->baseTableLength);
		c_printf("  Checksum:                0x%x\n", config->checksum);
		c_printf("  OEM Table Size:          0x%x\n", config->baseTableLength);
		c_printf("  Entry Count:             %d\n", config->entryCount);
		c_printf("  Local APIC:              0x%x\n", config->localAPIC);
		c_printf("  Extended Table Length:   0x%x\n", config->extendedTableLength);
		c_printf("  Extended Table Checksum: 0x%x\n", config->extendedTableChecksum);
	} else {
		c_printf("No MP Floating Pointer Structure Found - Falling back to defaults\n");


	}


	__panic("Hey, look!");
}
