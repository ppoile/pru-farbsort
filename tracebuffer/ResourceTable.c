#include "ResourceTable.h"


#include <stddef.h>
#include <rsc_types.h>
#include "pru_virtio_ids.h"

/*
 * Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define PRU_RPMSG_VQ0_SIZE	2
#define PRU_RPMSG_VQ1_SIZE	2

/* flip up bits whose indices represent features we support */
#define RPMSG_PRU_C0_FEATURES	1

/* Definition for unused interrupts */
#define HOST_UNUSED		255

/* Mapping sysevts to a channel. Each pair contains a sysevt, channel */
struct ch_map pru_intc_map[] = { {16, 1},
			       };

#define TABLE_ENTRY_COUNT	2

struct my_resource_table {
	struct resource_table base;

	uint32_t offset[TABLE_ENTRY_COUNT];

	struct fw_rsc_carveout memory;
	struct fw_rsc_trace trace;
};

#pragma DATA_SECTION(am335x_pru_remoteproc_ResourceTable, ".resource_table")
#pragma RETAIN(am335x_pru_remoteproc_ResourceTable)
struct my_resource_table am335x_pru_remoteproc_ResourceTable = {
	1,	/* we're the first version that implements this */
	TABLE_ENTRY_COUNT,	/* number of entries in the table */
	0, 0,	/* reserved, must be zero */
	/* offsets to entries */
	{
		offsetof(struct my_resource_table, memory),
		offsetof(struct my_resource_table, trace),
	},

	.memory = {
		.type = TYPE_CARVEOUT,
		.da = 0,		// da does nothing since the PRU is in the normal physical address space
		.pa = 0,
		.len = 0x8000,
		.flags = 0,
		.name = "TracerMemory",
	},

	.trace = {
		.type = TYPE_TRACE,
		.da = 0,		// trace has to bve inside a carveout
		.len = 0x8000,
		.name = "TheTrace",
	},
};

volatile char *traceAddress()
{
	const uint32_t traceOffset = am335x_pru_remoteproc_ResourceTable.trace.da - am335x_pru_remoteproc_ResourceTable.memory.da;
	const uint32_t traceAddr = am335x_pru_remoteproc_ResourceTable.memory.pa + traceOffset;

	return (char*)traceAddr;

}

size_t traceSize()
{
	return am335x_pru_remoteproc_ResourceTable.trace.len;
}
