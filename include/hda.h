
#ifndef HDA_H
#define HDA_H

#include "types.h"


//
// DEFINES
//

#define INTEL_HDA_CONTROLLER_CLASS 0x040300

//
// PCI Offsets (offsets into PCI cfg space)
//

#define PCI_CMD_W 		0x04
#define PCI_AZBARL_L 	0x10
#define PCI_AZBARU_L 	0x14
#define PCI_INTLN_B 	0x3C
#define PCI_AZCTL_B 	0x40
#define PCI_DEVC_W		0x78
#define PCI_DEVS_W		0x7A

//
// HDA Offsets (hda_bar + offset) 
//

#define HDA_GCTL_L 		0x08
#define HDA_WAKEEN_W	0x0C
#define HDA_STATESTS_W 	0x0E
#define HDA_INTCTL_L 	0x20
#define HDA_INTSTS_L 	0x24

#define HDA_WALLCLCK_L 	0x30
#define HDA_SSYNC_L		0x34

#define HDA_CORBLBASE_L 0x40
#define HDA_CORBUBASE_L 0x44
#define HDA_CORBRP_W 	0x4A
#define HDA_CORBCTL_B 	0x4C

#define HDA_RIRBLBASE_L 0x50
#define HDA_RIRBUBASE_L 0x54
#define HDA_RIRBWP_W 	0x58
#define HDA_RIRBCTL_B 	0x5C

#define HDA_IC_L 		0x60
#define HDA_IR_L 		0x64
#define HDA_IRS_W 		0x68

#define HDA_DPLBASE_L	0x70

#define HDA_IS0CTLSTS_L 0x80
#define HDA_IS0PIB_L 	0x84
#define HDA_IS0CBL_L 	0x88
#define HDA_IS0LVI_W 	0x8C
#define HDA_IS0FMT_W 	0x92
#define HDA_IS0BDPL_L 	0x98
#define HDA_IS0BDPU_L 	0x9C

#define HDA_OS0CTLSTS_L 0x100
#define HDA_OS0PIB_L 	0x104
#define HDA_OS0CBL_L 	0x108
#define HDA_OS0LVI_W 	0x10C
#define HDA_OS0FMT_W 	0x112
#define HDA_OS0BDPL_L 	0x118
#define HDA_OS0BDPU_L 	0x11C


//
//STRUCTS
//


struct _hda_bdl_entry
{
	uint32_t bd_address; //LSB must be 0
	uint32_t bd_address_upper; //MUST BE ALL 0's
	uint32_t bd_length;
	uint32_t bd_ioc;	//must be 0 OR 1
}__attribute__((packed));


//
//GLOBAL VARIABLES
//

struct _pci_dev * hda_dev;

uint32_t  hda_bar;
uint32_t  dma_bar;
uint8_t	  hda_intln;
uint8_t	  hda_codec_mask;

uint32_t * corb_bar;
uint32_t * rirb_bar;
struct _hda_bdl_entry * isd_bdl_bar;
struct _hda_bdl_entry * osd_bdl_bar;

//
//METHOD HEADERS
//

void _hda_init(void);
uint32_t _hda_imm_command_8( uint8_t cad, uint8_t nid, uint16_t vid, uint8_t payload);
uint32_t _hda_imm_command_16( uint8_t cad, uint8_t nid, uint8_t vid, uint16_t payload);
static void _isr_hda( int vector, int code );
void _hda_reset_codec( uint8_t cid );
#endif
