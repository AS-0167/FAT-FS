#pragma once
#include "utils.h"
#include"meta_data.h"

//🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸🧸
struct meta_data;
// extern meta_data md;

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

#define FOR_EACH_ENTRY(dir, i) for (uint32_t i = 0; i < (dir)->no_of_entries; i++)

directory initialize_directory(uint32_t num_entries);

uint32_t find_free_idx(const directory* directory);
int8_t update_ith_dir_entry(directory *directory, uint32_t index, dir_entry new_entry);
int8_t insert_entry(directory* directory, dir_entry new_entry);

int8_t update_size(directory* directory, uint32_t idx, uint32_t size);

void delete_entry(directory* directory, uint32_t idx);

uint32_t find_entry_by_name(directory *dir, const char *filename, uint32_t parent_idx);
uint32_t get_parent_by_idx(directory* directory, uint32_t my_idx);
char* get_name_by_idx(directory* directory, uint32_t my_idx);
void print_children(directory *dir, uint32_t parent_idx);

int8_t write_directory(FILE *file, const directory *dir);
int8_t read_directory(FILE *file, directory *dir);

void print_directory_with_time(const directory* dir);
void print_directory(const directory* dir);

void print_dir_entry_table_header();
void print_dir_entry(const dir_entry *entry);