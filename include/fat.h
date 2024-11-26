#pragma once
#include"utils.h"

//🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷🔷

typedef struct FAT {
    uint32_t* entries;
    uint32_t no_of_entries;

} FAT;

FAT initialize_fat(uint32_t fat_entries);

void write_fat(FILE *file, const FAT *fat);
void read_fat(FILE *file, FAT *fat);

void print_fat(const FAT* fat);