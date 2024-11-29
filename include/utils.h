#ifndef UTILS_H
#define UTILS_H

#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include<time.h>
#include<string.h>


uint32_t get_current_epoch_time(void);

int8_t LIMITATION_ERROR(void);
int8_t FIELD_LIMITATION_ERROR(void);
int8_t FILE_ERROR(void);
int8_t OPERATION_UNSUCCESSFUL(void);
int8_t INVALID_VALUE(void);
int8_t ALLOCATION_ERROR(void);

void separate_filename_and_extension(const char *filename, char *name, char *extension);


// #include "meta_data.h"
// #include "disk_operations.h"
// #include "directory.h"
// #include "folder.h"
// #include "file.h"
// #include "fat.h"



#endif