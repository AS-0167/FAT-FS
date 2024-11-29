#pragma once
#include "utils.h"
#include "meta_data.h"
#include "fat.h"
#include "directory.h"

extern meta_data md;

typedef struct cluster {
    unsigned char* buffer;
    uint32_t size : 20;
}cluster;


int8_t disk_read(cluster* clstr, FILE* file, uint64_t byte_offset);
int8_t disk_write(cluster* clstr, FILE* file, uint64_t byte_offset);

int8_t update_first_free(FILE* file, uint32_t first_free);
int8_t read_first_free(FILE *file, uint32_t *first_free);

void create_disk(const char* disk_name, uint64_t disk_size);
int8_t formate_disk(FILE* disk, const char* disk_name, uint64_t disk_size, uint32_t cluster_size, uint16_t max_file_size_in_clstr, uint8_t max_file_name_in_bytes, uint16_t max_folders);

/*
🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔
*/