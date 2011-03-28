typedef cluster_t uint_32;

struct file_t
{
	int drive_number;
	cluster_t cluster_number;
}

enum _FS_TYPE
{
	FAT_32;
};

struct filesystem{
	//read pointer
	//write pointer
	//open pointer
	
	cluster_t root_cluster_number;
	enum _FS_TYPE fs_type;

	struct * storage_device;
};
