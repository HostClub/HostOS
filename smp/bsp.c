#include "headers.h"
#include "bsp.h"
#include "trampoline.h"
#include "log.h"
#include "structs.h"
#include "mm.h"

#define DEBUG_LEVEL DL_DEBUG

LAPIC_t *local_apic;
mutex_t *processor_started = (mutex_t *)STARTUP_MUTEX;

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

uint8_t inline get_lapic_version(LAPIC_t* lapic) {
	return (lapic->versionLVT & 0xFF);
}

uint8_t inline get_lapic_maxLVT(LAPIC_t* lapic) {
	return ((lapic->versionLVT >> 16) & 0xFF);
}

void strncpy(char *src, char *dst, int len) {
	dst[len] = 0;
	while (len--) {
		dst[len] = src[len];
	}
}


/*
** This is used to find the MP Floating Pointer Structure on machines that
** follow Intel's MP Spec. It returns a pointer to a stucture that is
** overlayed onto the actual MPFP structure in memory.
*/

MPFloatPointer_t *findMPFPS() {
	//check to see is the extended BIOS data area is defined
	uint32_t *ebdaBase = (uint32_t *)EBDA_POINTER;

	info("Checking EBDA base @ 0x%x: 0x%x\n", EBDA_POINTER, *ebdaBase);
	if (ebdaBase) {
		//search for the floating pointer structure in the extended BIOS data area
		*ebdaBase = *ebdaBase << 4;
		info("EBDA defined to be: 0x%x\n", *ebdaBase);
		uint32_t *loc = (uint32_t *)*ebdaBase;
		info("Checking the EBDA between 0x%x and 0x%x\n", loc, *ebdaBase + 0x0400 - 4);
		while ((uint32_t)loc < *ebdaBase + 0x0400 - 4) {
			//c_putchar(*loc);
			if (*loc == MP_SIG) {
				return (MPFloatPointer_t *)loc;
			}
			loc += 1;
		}
	} else {
		//search for the structure in the last 1KB of system base memory
		info("No EBDA defined\n");
	}


	info("Checking the BIOS ROM (0x%x - 0x%x)\n", BIOSROM_LOW, BIOSROM_HIGH);
	//search for the structure in BIOS ROM
	uint32_t *loc = (uint32_t *)BIOSROM_LOW;
	while ((uint32_t)loc < BIOSROM_HIGH - 4) {
		if (*loc == MP_SIG) {
			return (MPFloatPointer_t *)loc;
		}
		loc += 1;
	}

	uint32_t *topMem = (uint32_t *)TOPOFMEM_POINTER;
	info("Checking top of memory pointer @ 0x%x: 0x%x\n", TOPOFMEM_POINTER, *topMem);
	if (topMem) {
		//search for the floating pointer structure in the extended BIOS data area
		//(*topMem)++;
		(*topMem) *= 1024;
		info("Top of memory defined to be: 0x%x\n", *topMem);
		uint32_t *loc = (uint32_t *)*topMem;
		info("Checking the top of memory between 0x%x and 0x%x\n", loc, *topMem + 0x0400 - 4);
		while ((uint32_t)loc < *topMem + 0x0400 - 4) {
			//c_putchar(*loc);
			if (*loc == MP_SIG) {
				return (MPFloatPointer_t *)loc;
			}
			loc += 1;
		}
	}

	info("Checking the default top of memory (0x%x - 0x%x)\n", DEFAULT_TOPOFMEM, DEFAULT_TOPOFMEM - 1024);
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


/*
** Sends the INIT-SIPI-SIPI command sequence to the specified
** processor. Returns whether or not the processor actually started.
*/

bool_t startup_CPU(int id) {
	info("Starting up CPU (id=%d)\n", id);
	mutex_clear(processor_started);

	IPICommand_t ipi;
	set_ipi_destination(&ipi, id);
	set_ipi_shorthand(&ipi, IPI_SHORTHAND_NONE);
	set_ipi_triggermode(&ipi, IPI_TRIGGER_EDGE);
	set_ipi_level(&ipi, IPI_LEVEL_ASSERT);
	set_ipi_destinationmode(&ipi, IPI_DESTMODE_PHYSICAL);
	set_ipi_deliverymode(&ipi, IPI_DELMODE_INIT);
	set_ipi_deliverymode(&ipi, IPI_DELMODE_INIT);
	set_ipi_vector(&ipi, 0);

	send_IPI(&ipi);

	uint32_t a = 0xFFFF0000;
	while (++a);

	set_ipi_destination(&ipi, id);
	set_ipi_shorthand(&ipi, IPI_SHORTHAND_NONE);
	set_ipi_triggermode(&ipi, IPI_TRIGGER_EDGE);
	set_ipi_level(&ipi, IPI_LEVEL_ASSERT);
	set_ipi_destinationmode(&ipi, IPI_DESTMODE_PHYSICAL);
	set_ipi_deliverymode(&ipi, IPI_DELMODE_STARTUP);
	set_ipi_vector(&ipi, AP_TARGET_ADDRESS >> 12);


	a = 0xFFFF0000;
	send_IPI(&ipi);
	while (++a);

	a = 0xFFFF0000;
	send_IPI(&ipi);
	while (++a);

	a = 0xF0000000;
	while (++a);

	debug("0x%x\n", *((int *)STARTUP_MUTEX));

	debug("APIC: 0x%x\n", *((int *)0x1B));
	//debug("TEST: 0x%x\n", *((int *)0x9000));

	//mutex_test_and_set(processor_started);

	bool_t result = mutex_is_set(processor_started);
	mutex_clear(processor_started);

	if (result) {
		debug("Started processor (id = %d)\n", id);
	} else {
		debug("Could not start processor (id = %d)\n", id);
	}

	return result;
}


/*
** Start up all of the CPUs specified in the cpus structure.
** NOTE: This currently doesn't actually start the processors.
*/

void startup_cpus(cpus_t *cpus) {
	int i;

	info("Attempting to start %d processor(s)\n", cpus->total_count - 1);
	for (i = 1; i < cpus->total_count; i++) {
		cpus->infos[i].apicID = i;

		/*if (startup_CPU(i)) {
			cpus->infos[i].state = halted;
		} else {*/
			cpus->infos[i].state = offline;
			error("Could not start processor %d\n", i);
		//}
	}
}


/*
** Initialize SMP. This detects the type of processor used by the system
** and detects the number of logical cores. This populates a cpus_t structure
** to hold information about each of the cores.
*/

void initSMP() {
	int cpu_count = checkCPUs();

	setDebugLevel(l_info);

	if (cpu_count == -1) {
		error("Error while checking number of CPUs\n");
		return;
	}

	cpus_t *cpus = (cpus_t *)kalloc(sizeof(cpus_t), 0, 0);
	cpus->online_count = 0;
	cpus->total_count = cpu_count;
	cpus->infos = (cpu_info_t *)kalloc(sizeof(cpu_info_t *) * cpu_count, 0, 0);


	build_lapic_info();

	startup_cpus(cpus);
}


/*
** Checks to see if the processor supports multiple processors.
** Returns the number of supported processing cores or zero if
** there was an error.
*/

int32_t checkCPUs() {
	uint32_t id;
	uint32_t data[5];
	uint32_t maxCpuIdOp;

	id = read_eflags();
	write_eflags(id | 0x200000);
	id = read_eflags();
	id = (id >> 21) & 1;

	if (!id) {
		info("CPUID not supported!!!\n");
		return -1;
	}


	cpuid_data(VENDOR_ID, data);
	unscramble_cpuid(data);
	maxCpuIdOp = data[0];
	//Null terminate the string
	data[4] = 0;

	info( "Max CPUID Op: %d\n", maxCpuIdOp );
	info( "CPU Vendor: '%s'\n", (char *)(data + 1) );

	id = cpuid_value(EXT_FUNCS);
	if (id >= BRAND_STRING3) {
		//Supports extended functions
		cpuid_data(BRAND_STRING1, data);
		info("%s", data);
		cpuid_data(BRAND_STRING2, data);
		info("%s", data);
		cpuid_data(BRAND_STRING3, data);
		info("%s\n", data);

		cpuid_data(EXT_FEATURES, data);
		info("Extended Features (ECX):  0x%x\n", *((uint32_t *)(data + 8)));
		info("Extended Features (EDX):  0x%x\n", *((uint32_t *)(data + 12)));
	} else {
		//Doesn't support extended functions
		cpuid_data(FEATURE_INFO, data);
		unscramble_cpuid(data);
		id = *((uint32_t *)data) & 0x0FFF3FFF;

		info("Signature:       0x%x (", id);
		switch (id) {
			case SIG_I7:
				info("Core i7 Processor)\n");
				break;
			case SIG_CORE2:
				info("Core 2 Extreme Processor)\n");
				break;
			default:
				info("Unknown Processor)\n");
				return -1;
		}
	}

	cpuid_data(FEATURE_INFO, data);
	unscramble_cpuid(data);
	id = data[0] & 0x0FFF3FFF;

	char steppingID =     (id & 0x0000000F) >> 0;
	char modelNumber =    (id & 0x000000F0) >> 4;
	char familyCode =     (id & 0x00000F00) >> 8;
	char type =           (id & 0x00003000) >> 12;
	char extendedModel =  (id & 0x000F0000) >> 16;
	char extendedFamily = (id & 0x0FF00000) >> 20;

	info("Features (ECX): 0x%x\n", *((uint32_t *)data + 8));
	info("Features (EDX): 0x%x\n", *((uint32_t *)data + 12));

	info("Stepping ID:     %d\n", steppingID);
	info("Model Number:    %d\n", modelNumber);
	info("Family Code:     %d\n", familyCode);
	info("Type:            %d\n", type);
	info("Extended Model:  %d\n", extendedModel);
	info("Extended Family: %d\n", extendedFamily);

	id = data[1];
	info("\nLogical Processors: %d\n", (id & 0x00FF0000) >> 16);
	return (id & 0x00FF0000) >> 16;
}


/*
** Prints out basic cpu caching information
*/

void cache_info(char *dummy) {
	// Enumerate the deterministic cache parameters
	uint32_t data[5];
	uint32_t id;

	cpuid_data_with_param(D_CACHE_PARAM, 0, data);
	unscramble_cpuid(data);

	id = *((uint32_t *)(data));
	info("APIC IDs:           %d\n", 1 + ((id & 0xFC000000) >> 26));

	int i = 0;
	while ((id & 0x0F) != 0) {
		char sharingThreads = ((id & 0x03FFC000) >> 14) + 1;
		char level = (id & 0xF0) >> 4;
		char type = (id & 0x0F);

		info("Cache Number: %d   ", i);
		info("  Threads sharing this cache: %d   ", sharingThreads);
		info("  Level: %d   ", level);
		info("  Type: ");
		switch (type) {
			case 1:
				info("Data");
				break;
			case 2:
				info("Instruction");
				break;
			case 3:
				info("Unified");
				break;
			default:
				info("INVALID");
		}

		c_putchar('\n');

		i++;
		cpuid_data_with_param(D_CACHE_PARAM, i, data);
		id = *((uint32_t *)(data));
	}
}


/*
** Loads the local apic's information. This is used to interogate the CPU
** and send IPIs.
*/

void build_lapic_info() {
	char temp[13];

	MPFloatPointer_t *mpStruct = findMPFPS();

	if (mpStruct) {
		info("MP Floating Pointer\n");
		strncpy(mpStruct->signature, temp, 4);
		info("  Signature:     %s\n", temp);
		info("  Table Address: 0x%x\n", mpStruct->table);
		info("  Table Length:  0x%x\n", mpStruct->tableLength * 16);
		info("  Revision:      0x%x\n", mpStruct->specRev);
		info("  Checksum:      0x%x\n", mpStruct->checksum);
		info("  Features:      0x%x 0x%x 0x%x 0x%x 0x%x\n", mpStruct->features[0], mpStruct->features[1], mpStruct->features[2], mpStruct->features[3], mpStruct->features[4]);

		info("MP Configuration Table\n");
		MPConfigTable_t *config = mpStruct->table;

		strncpy(config->signature, temp, 4);
		info("  Signature:               %s\n", temp);
		strncpy(config->oemID, temp, 8);
		info("  OEM ID:                  %s\n", temp);
		strncpy(config->productID, temp, 12);
		info("  Product ID:              %s\n", temp);
		info("  Revision:                0x%x\n", config->specRev);
		info("  Base Table Length:       0x%x\n", config->baseTableLength);
		info("  Checksum:                0x%x\n", config->checksum);
		info("  OEM Table Size:          0x%x\n", config->baseTableLength);
		info("  Entry Count:             %d\n", config->entryCount);
		info("  Local APIC:              0x%x\n", config->localAPIC);
		info("  Extended Table Length:   0x%x\n", config->extendedTableLength);
		info("  Extended Table Checksum: 0x%x\n", config->extendedTableChecksum);

		local_apic = (LAPIC_t *)DEFAULT_LAPIC_ADDRESS;
	} else {
		warn("No MP Floating Pointer Structure Found - Falling back to defaults\n");
		local_apic = (LAPIC_t *)DEFAULT_LAPIC_ADDRESS;
	}

	info("Local APIC\n");
	info("  ID:      0x%x\n", local_apic->id);
	info("  Version: 0x%x\n", get_lapic_version(local_apic));
	info("  Max LVT: 0x%x\n", get_lapic_maxLVT(local_apic));
	info("  SVR:     0x%x\n", local_apic->svr);
}


/*
** Send the IPI described in 'command'
*/

void inline send_IPI(IPICommand_t *command) {
	/*debug("===IPI===\n");
	debug(" Upper:0x%x\n", command->upper);
	debug(" Lower:0x%x\n", command->lower);
	debug("=========\n");*/

	local_apic->icr_hi = command->upper;
	local_apic->icr_lo = command->lower;

	//while (local_apic->icr_lo & 0x1000) {
	//	debug("Waiting for send");
	//}
}


/*
** Helper functions for the IPI structure
*/

void set_ipi_destination(IPICommand_t *ipi, uint8_t value) {
	ipi->upper = value << 24;
}

void set_ipi_shorthand(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x000C0000;
	ipi->lower |= (value & 0x03) << 17;
}

void set_ipi_triggermode(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x00008000;
	ipi->lower |= (value & 0x01) << 15;
}

void set_ipi_level(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x00004000;
	ipi->lower |= (value & 0x01) << 14;
}

void set_ipi_destinationmode(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x00000800;
	ipi->lower |= (value & 0x01) << 11;
}

void set_ipi_deliverymode(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x00000700;
	ipi->lower |= (value & 0x07) << 8;
}

void set_ipi_vector(IPICommand_t *ipi, uint8_t value) {
	ipi->lower &= ~0x000000FF;
	ipi->lower |= value;
}
