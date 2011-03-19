#include "file.h"
#include "fs.h"

int read(struct * _file file , char * buffer , int bytes_to_read);

int write(struct * _file file, char * buffer , int bytes_to_write);

struct * _file open(char * filename);

//This is an array to map from filesystem numbers to filesystem
//structs, which contain the filesystem read and write pointers
struct _fs[] filesystems;
