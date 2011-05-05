
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

#define PCI_CMD_W 0x04
#define PCI_AZBARL_L 0x10
#define PCI_AZBARU_L 0x14
#define PCI_INTLN_B 0x3C
#define PCI_AZCTL_B 0x40


//
// HDA Offsets (hda_bar + offset) 
//

#define HDA_GCTL_L 0x08
#define HDA_INTCTL_L 0x20
#define HDA_INTSTS_L 0x24

#define HDA_CORBLBASE_L 0x40
#define HDA_CORBUBASE_L 0x44
#define HDA_CORBRP_W 0x4A
#define HDA_CORBCTL_B 0x4C

#define HDA_RIRBLBASE_L 0x50
#define HDA_RIRBUBASE_L 0x54
#define HDA_RIRBWP_W 0x58
#define HDA_RIRBCTL_B 0x5C

#define HDA_IC_L 0x60
#define HDA_IR_L 0x64
#define HDA_IRS_W 0x68

#define HDA_IS0CTLSTS_L 0x80
#define HDA_IS0PIB_L 0x84
#define HDA_IS0CBL_L 0x88
#define HDA_IS0LVI_W 0x8C
#define HDA_IS0FMT_W 0x92
#define HDA_IS0BDPL_L 0x98
#define HDA_IS0BPDU_L 0x9C

#define HDA_OS0CTLSTS_L 0x100
#define HDA_OS0PIB_L 0x104
#define HDA_OS0CBL_L 0x108
#define HDA_OS0LVI_W 0x10C
#define HDA_OS0FMT_W 0x112
#define HDA_OS0BDPL_L 0x118
#define HDA_OS0BPDU_L 0x11C





//
//STRUCTS
//


struct _hda_bdl_entry
{
	uint32_t bd_address; //LSB must be 0
	uint32_t bd_control_length;
};


//
//GLOBAL VARIABLES
//

struct _pci_dev * hda_dev;

uint8_t * hda_bar;

uint32_t* corb_bar;
uint32_t* rirb_bar;
struct _hda_bdl_entry * isd_bdl_bar;
struct _hda_bdl_entry * osd_bdl_bar;

//
//METHOD HEADERS
//

void _hda_init(void);


#endif
