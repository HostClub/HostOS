#include "hda.h"
#include "startup.h"
#include "c_io.h"
#include "kalloc.h"
#include "ulib.h"
#include "pci.h"

void _hda_init(void)
{
	struct _pci_dev search;
	uint32_t azl_reg;
	uint32_t i = 0;
	search.class = INTEL_HDA_CONTROLLER_CLASS;
	search.next = NULL;
	hda_dev = _find_pci(&search);

//	if( hda_dev == NULL )
//	{
//		c_puts("Audio Device Initialization Failed!\n" );
//		return;
//	}

	//Check whats in the PCI_CMD register
	c_printf( "PCI CMD=%x\n", _pci_config_read_word( 0, 27, 0, PCI_CMD_W));		


	//GET THE BAR FROM THE PCI CFG 
	hda_bar = _pci_config_read_word(0, 27, 0, PCI_AZBARL_L) & 0xFFFFFFF0; 


	c_printf( "HDA_BAR= %x\n", hda_bar);
	
	//FIND INTERRUPT LINE AND REGISTER ISR
	hda_intln = _pci_config_read_word(0, 27, 0, PCI_INTLN_B) & 0xFF; 

	c_printf( "HDA_INTLN= %x\n", hda_intln);
	
	c_printf( "Output CTL STS = %x\n", ((uint32_t*)(hda_bar + HDA_OS0CTLSTS_L))[0] );
	
	//RESET REEGISTERS THAT NEED IT
	((uint16_t*)(hda_bar + HDA_WAKEEN_W))[0] = ((uint16_t*)(hda_bar + HDA_WAKEEN_W))[0] | 7;
	((uint16_t*)(hda_bar + HDA_STATESTS_W))[0] = ((uint16_t*)(hda_bar + HDA_STATESTS_W))[0] | 7;
		
	c_printf( "HDA Global CTRL = %x\n", ((uint32_t*)(hda_bar + HDA_GCTL_L))[0]);
	//FLIP THE CONTROLLE RESET BIT to 0 
	((uint32_t*)(hda_bar + HDA_GCTL_L))[0] = 0;
	
	//WAIT FOR IT TO READ 0 
	while( ((uint32_t*)(hda_bar + HDA_GCTL_L))[0]  == 1 ){
		c_puts( "Waiting for HDA controller to enter RESET\n" );
	}

	azl_reg = ((uint32_t*)(hda_bar + HDA_GCTL_L))[0];
	azl_reg = azl_reg | 1;
	((uint32_t*)(hda_bar + HDA_GCTL_L))[0] = azl_reg;


	//WAIT FOR IT TO READ 1
	while( ((uint32_t*)(hda_bar + HDA_GCTL_L))[0] & 1 == 0 ){
		c_puts( "Waiting for HDA controller to exit RESET\n" );
	}
	
	c_printf( "Audio Global Capabilities=%x\n", ((uint16_t*)hda_bar)[0]);
	
	//WAIT 250micros THEN FIND ALL CODECS
	
	sleep( 1000 );

	c_printf( "STATESTS = %x\n", ((uint16_t*)(hda_bar + HDA_STATESTS_W))[0]) ;
	c_printf( "WAKEEN= %x\n", ((uint16_t*)(hda_bar + HDA_WAKEEN_W))[0]) ;

	//BEEP DAMNIT
//	_hda_imm_command(2, 1, 0x70A, 1);


	//ALLOCATE and init CORB/RIRB
	corb_bar = (uint32_t*) _kalloc_align( 4 * 256, 128);
	rirb_bar = (uint32_t*) _kalloc_align( 8 * 256, 128);
	((uint32_t*)(hda_bar + HDA_CORBLBASE_L))[0] = corb_bar;
	((uint32_t*)(hda_bar + HDA_RIRBLBASE_L))[0] = rirb_bar;

	((uint8_t*)(hda_bar + HDA_CORBCTL_B))[0] = 2;
	((uint8_t*)(hda_bar + HDA_RIRBCTL_B))[0] = 2;

	//INIT CODECS
	c_printf("CODEC VID = %x\n", _hda_imm_command(2, 0, 0xF00, 0));
	//PRINT PIN CAPS & JACK SENSE
	for(i = 0x14; i <= 0x1F; i++){
		sleep(50);
		c_printf("Get WIDGET CTL %x= %x\n",i, _hda_imm_command(2, i, 0xF07, 0));
		c_printf("Get Jack Sense %x= %x\n",i, _hda_imm_command(2, i, 0xF09, 0));
	}

	//ALLOCATE STREAM BUFFERS AND BDLS AND CONFIG
	isd_bdl_bar = (struct _hda_bdl_entry *) _kalloc_align( sizeof(struct _hda_bdl_entry) * 32, 128 );		
	osd_bdl_bar = (struct _hda_bdl_entry *) _kalloc_align( sizeof(struct _hda_bdl_entry) * 32, 128 );		
	c_printf( "Input BDL = %x, Output BDL = %x\n", isd_bdl_bar, osd_bdl_bar);
	((uint32_t*)(hda_bar + HDA_IS0BDPL_L))[0] = isd_bdl_bar;
	((uint32_t*)(hda_bar + HDA_OS0BDPL_L))[0] = osd_bdl_bar;

	for(i = 0; i < 32; i++){

		osd_bdl_bar[i].bd_address = ((uint32_t) _kalloc_align( 4 * 128 * 100 , 128));
		osd_bdl_bar[i].bd_address_upper = 0;
		osd_bdl_bar[i].bd_length = 12800 ;
		osd_bdl_bar[i].bd_ioc = 0;

		isd_bdl_bar[i].bd_address = ((uint32_t) _kalloc_align( 4 * 128 * 100 , 128));
		isd_bdl_bar[i].bd_address_upper = 0;
		isd_bdl_bar[i].bd_length= 12800;
		isd_bdl_bar[i].bd_ioc = 0;

	//	c_printf("Buffer %d - Input = %x Output = %x", i, isd_bdl_bar[i].bd_address >> 1, osd_bdl_bar[i].bd_address >> 1 ); 
	}
	
	//ALLOCATE DMA BUFFER AND SET DPLBASE
	dma_bar = (uint32_t) _kalloc_align( 64, 128);
	((uint32_t*)(hda_bar + HDA_DPLBASE_L))[0] = dma_bar | 1;

	//SET LAST VALID BUFFER INDEX
	((uint16_t*)(hda_bar + HDA_IS0LVI_W))[0] = 31;
	((uint16_t*)(hda_bar + HDA_OS0LVI_W))[0] = 31;
	c_printf( "Input LVBI = %x, Output LVBI= %x\n", ((uint16_t*)(hda_bar + HDA_IS0LVI_W))[0], ((uint16_t*)(hda_bar + HDA_OS0LVI_W))[0]);
	
	//SET CYCLIC LENGTH 
	((uint32_t*)(hda_bar + HDA_OS0CBL_L))[0] = 4*128*100*32;
	((uint32_t*)(hda_bar + HDA_IS0CBL_L))[0] = 4*128*100*32;
	c_printf( "Input Cyclic L = %x, Output Cyclic L= %x\n", ((uint32_t*)(hda_bar + HDA_IS0CBL_L))[0], ((uint32_t*)(hda_bar + HDA_OS0CBL_L))[0]);
	
	//SET STREAM FORMATS (24bit 48kHz)
	c_printf( "Input FMT= %x, Output FMT= %x\n",  ((uint16_t*)(hda_bar + HDA_IS0FMT_W))[0], ((uint16_t*)(hda_bar + HDA_OS0FMT_W))[0]);
	((uint16_t*)(hda_bar + HDA_IS0FMT_W))[0] = 0x0031;
	((uint16_t*)(hda_bar + HDA_OS0FMT_W))[0] = 0x0031;
	c_printf( "Input FMT= %x, Output FMT= %x\n",  ((uint16_t*)(hda_bar + HDA_IS0FMT_W))[0], ((uint16_t*)(hda_bar + HDA_OS0FMT_W))[0]);
	
	//SEE IF JACKSENSE WORKS

	//START INPUT STREAM
	c_printf( "Input CTL STS = %x\n", ((uint32_t*)(hda_bar + HDA_IS0CTLSTS_L))[0] );
	((uint32_t*)(hda_bar + HDA_IS0CTLSTS_L))[0] = ((uint32_t*)(hda_bar + HDA_IS0CTLSTS_L))[0] | ( 2 | 1 << 20 );

	c_printf( "SSYNC= %x\n", ((uint32_t*)(hda_bar + HDA_SSYNC_L))[0] );
	
	while(1){
		sleep(1000);
		c_printf( "PCI Dev C= %x\n", _pci_config_read_word(0, 27, 0, PCI_DEVC_W) & 0xFFFF);
		c_printf( "PCI Dev S= %x\n", _pci_config_read_word(0, 27, 0, PCI_DEVS_W) & 0xFFFF);
		c_printf( "PCI AZCTL= %x\n", _pci_config_read_word(0, 27, 0, PCI_AZCTL_B) & 0xFF);
		c_printf( "Position in input buffer = %x\n", ((uint32_t*)(hda_bar + HDA_IS0PIB_L))[0]);
		c_printf( "LVI in input buffer = %x\n", ((uint16_t*)(hda_bar + HDA_IS0LVI_W))[0]);
		c_printf( "Input CTL STS = %x\n", ((uint32_t*)(hda_bar + HDA_IS0CTLSTS_L))[0] );
		c_printf( "Sample at head of buffer 1= %x\n", ((uint32_t*)(isd_bdl_bar->bd_address ))[0] );
		c_printf( "Wall clock = %x\n", ((uint32_t*)(hda_bar + HDA_WALLCLCK_L))[0]);
	
	}
	

    c_puts("Completed Audio Initialization\n");	
}

//
// Sends a command to the audio codecs and returns the immediate response
// Cannot send broadcast commands due to the 1::1 nature of the responses
//
uint32_t _hda_imm_command( uint8_t cad, uint8_t nid, uint16_t vid, uint8_t payload )
{
	uint32_t cmd = payload | (vid << 8) | (nid << 20 ) | (cad << 28);
	uint32_t ret = 0;

	while( ((uint16_t*)(hda_bar + HDA_IRS_W))[0] & 1 != 0 ){ c_puts("Wait to start\n");}
	((uint32_t*)(hda_bar + HDA_IC_L))[0] = cmd;

	//c_printf("IC register = %x\n", ((uint32_t*)(hda_bar + HDA_IC_L))[0] );

	((uint16_t*)(hda_bar + HDA_IRS_W))[0] = ((uint16_t*)(hda_bar + HDA_IRS_W))[0] | 1;

	while( ((uint16_t*)(hda_bar + HDA_IRS_W))[0] & 2 == 0 ){ c_puts("Wait for response\n");}

	//c_printf("IRS register = %x\n", ((uint16_t*)(hda_bar + HDA_IRS_W))[0] );
	sleep(250); //sleep to ensure response gets into the register in time

	ret = ((uint32_t*)(hda_bar + HDA_IR_L))[0];

	((uint16_t*)(hda_bar + HDA_IRS_W))[0] = ((uint16_t*)(hda_bar + HDA_IRS_W))[0] | 2;
	
	//c_printf("IR register = %x\n", ((uint32_t*)(hda_bar + HDA_IR_L))[0] );

	return ret;

}
