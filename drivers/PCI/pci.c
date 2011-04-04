struct pci_bus * list_pci()
{
	for(int i = 0; i < MAX_PCI; i ++)
	{
		
	}
}

uint16_t _pci_config_read_word(uint16_t bus , uint16_t slot, uint16_t function , uint16_t offset)
{
	uint64_t address;
	uint64_t lbus = (uint64_t)bus;
	uint64_t lslot = (uint64_t)slot;
	uint64_t lfunc = (uint64_t)function;
	unsigned short ret = 0;


	//TODO: Remove magic numbers
	address = (uint64_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((UINT32)0x80000000));

	/* write out the address */
	__outl (CONFIG_ADDRESS, address);
	/* read in the data */
	ret = (uint16_t)((__inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return (ret);
}
