#include "hda.h"
#include "startup.h"
#include "c_io.h"
#include "kalloc.h"
#include "ulib.h"

void _hda_init(void)
{
	struct _pci_dev search;
	search.class = INTEL_HDA_CONTROLLER_CLASS;
	hda_dev = _pci_find(&search);

	if( hda_dev == NULL )
	{
		c_puts("Audio Device Initialization Failed!\n" );
		return;
	}

    c_puts("Completed Audio Initialization\n");	
}

