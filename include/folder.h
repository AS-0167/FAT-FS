#ifndef FOLDER_H
#define FOLDER_H
#include "stdint.h"
#include "directory.h"

//🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸🛸
struct directory;

int8_t create_folder(directory* dir, char* f_name, uint32_t parent_idx);


#endif 