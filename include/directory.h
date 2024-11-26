#pragma once
#include"utils.h"

//🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸
extern meta_data md;

typedef struct dir_entry {
    uint32_t idx : 21;                      //^ 21 bits 
    uint32_t parentIdx : 21;                //^ 21 bits 
    char name[256];                         //^ 2048 bits (256 bytes)
    char ext[4];                            //^ 32 bits (3 bytes + 1 for null terminator)
    uint32_t size : 30;                     //^ 30 bits
    uint32_t firstClstr : 20;               //^ 20 bits
    uint16_t accessbit : 9;                 //^ 9 bits (r-w-x permissions)
    uint8_t deleted : 1;                    //^ 1 bit
    uint8_t isfile : 1;                     //^ 1 bit
    uint8_t isValid : 1;                    //^ 1 bit
    uint32_t creationEpoch;                 //^ 32 bits
    uint32_t accessEpoch;                   //^ 32 bits
    uint32_t modifyEpoch;                   //^ 32 bits

} dir_entry;


typedef struct directory {
    dir_entry *entries; 
    uint32_t no_of_entries;      
} directory;

directory initialize_directory(uint32_t num_entries);

uint32_t find_idx(const directory* directory);
void update_ith_dir_entry(directory *directory, uint32_t index, dir_entry new_entry);
void insert_entry(directory* directory, dir_entry new_entry);

void write_directory(FILE *file, const directory *dir);

void read_directory(FILE *file, directory *dir);

void print_directory_with_time(const directory* dir);

void print_directory(const directory* dir);
