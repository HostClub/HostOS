#include "hda.h"
#include "startup.h"
#include "c_io.h"
#include "kalloc.h"
#include "ulib.h"
#include "pci.h"

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
	
	//ALLOCATE ENOUGH MEMORY FOR THE HDA CONTROL BLOCK

	//SET THE BARs IN THE PCI CFG 
	
	//SET INTERRUPT LINE AND REGISTER ISR

	//FLIP THE CONTROLLE RESET BIT to 1

	//WAIT FOR IT TO READ 1

	//WAIT 250micros THEN FIND ALL CODECS

	//INIT CODECS

	//ALLOCATE CORB/RIRB?

	//ALLOCATE STREAM BUFFERS AND BDLS AND CONFIG


    c_puts("Completed Audio Initialization\n");	
}

