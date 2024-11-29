#pragma once
#include "utils.h"

//🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷

typedef struct FAT {
    uint32_t* entries;
    uint32_t no_of_entries;

} FAT;

FAT initialize_fat(uint32_t fat_entries);

int8_t write_fat(FILE *file, const FAT *fat);
int8_t read_fat(FILE *file, FAT *fat);

int8_t extend_link(FAT* fat, uint32_t prev, uint32_t nxt);
int8_t add_link(FAT* fat, uint32_t to_extend, uint32_t* first_free);


void print_fat(const FAT* fat);