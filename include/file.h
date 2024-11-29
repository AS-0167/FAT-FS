#pragma once
#include "utils.h"
#include "meta_data.h"
#include "directory.h"
#include "fat.h"
#include "disk_operations.h"

//🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷🌷

int8_t create_file(directory* directory, char* f_name, uint32_t parent_idx);

uint32_t input_file_content(char** content);
int8_t write_in_file(FILE* file, directory* directory, FAT* fat, uint32_t* first_free, uint32_t idx, char** content, uint32_t size);
int8_t read_from_file(FILE* file, directory* directory, FAT* fat, uint32_t idx, char** content, uint32_t* size);
