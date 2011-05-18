#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct _cpu_info {
	uint32_t apicID;
	enum state_e {
		online,
		halted,
		offline
	} state;
} cpu_info_t;

typedef struct cpus {
	uint32_t online_count;   // Number of online CPUs
	uint32_t total_count;    // Total number of CPUs
	cpu_info_t *infos;       // Info structures for each of the CPUs
} cpus_t;

#endif

