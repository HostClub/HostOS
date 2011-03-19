#include "file.h"

//Header file for the FAT32 filesystem implementation

int _read_fat32(struct _file * file , char * buffer , int bytes_to_read);

int _write_fat32(struct _file * file , char * buffer , int bytes_to_write);

struct _file * _open_fat32(char * filename);

int device_number;

//driver read function pointer
//
//driver write function pointer
//
//driver open function pointer
