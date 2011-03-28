#include "fs.h"

//Header file for the FAT32 filesystem implementation

int _read_fat32(struct _fs * filesystem , cluster_t cluster_number ,  uint_8 * buffer , int bytes_to_read);

int _write_fat32(struct _fs * filesystem , cluster_t cluster_number , uint_8 * buffer , int bytes_to_write);

cluster_t _open_fat32(struct _fs * filesystem , char * filename);

int FAT32_EBR_OFFSET = 36;

struct fat32_ebr
{
	uint_32 sectors_per_fat;
	uint_16 flags;
	uint_16 fat_version;
	uint_32 root_cluster_number;
	uint_16	fsinfo_cluster;
	uint_16 backup_cluster;
	uint_32 reserved_block[3];
	uint_8	drive_number;
	uint_8	nt_flags;
	uint_8	signature;
	uint_32	serial_number;
	char	label_string[11];
	char	identifier_string[8];
};

int FAT32_ROOT_CLUSTER_OFFSET = 512;

struct fat32_dir_entry
{
	char 	filename[11];
	uint_8	attributes;
	uint_8 	nt_reserved;
	uint_8	creation_time_tenths_seconds;
	
	//Hour		5 bits
	//Minutes	6 bits
	//Seconds	5 bits
	uint_16	creation_time;

	//Year		7 bits
	//Month		4 bits
	//Day		5 bits
	uint_16	creation_date;

	//Same format as creation_date
	uint_16 accessed_date;

	uint_16 high_cluster_number;
	
	//Same format as creation_time
	uint_16	modification_time;

	//Same format as creation_date
	uint_16	modification_date;

	uint_16 low_cluster_number;
	uint_32 file_size;
};

struct fat32_long_file_name_entry
{
	uint_8	order;
	uint_16	char_one[5];
	uint_8	attribute;
	uint_8	long_entry_type;
	uint_8	checksum;
	uint_16 char_two[6];
	uint_16	zero;
	uint_16 char_three[2];

};

