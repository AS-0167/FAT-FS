#pragma once
#include "utils.h"

typedef struct meta_data {

                        //^ MAX LIMITS OF THIS FILE-SYSTEM
                        //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    /*01✔*/  uint64_t DISK_SIZE : 40;                    //^ 2^40 = 1 TB                                         
    /*02✔*/  uint32_t CLUSTER_SIZE : 20;                 //^ 2^20 = 1 MB    
    /*03✔*/  uint32_t NO_OF_CLUSTERS_FOR_FILES : 20;     //^ 2^20 = 1 MB

    /*04✔*/  uint16_t MAX_FILE_SIZE_IN_CLUSTERS : 10;    //^ 1 K clusters 
    /*05✔*/  uint32_t MAX_FILE_SIZE : 30;                //^ 1 K * 1 MB = 1^30 = 10 MB 
    /*06✔*/  uint8_t MAX_FILE_NAME_IN_BYTES;             //^ 2^8 = 256 B
    /*07✔*/  uint16_t MAX_FOLDERS : 11;                  //^ 2^10 = 1 K folders
    /*08✔*/  uint32_t MAX_FILES : 20;                    //^ 2^20 = 1 M files
    /*09✔*/  uint32_t MAX_FULL_SIZE_FILES : 19;          //^ 1 TB/ 10 MB = 2^19 = 512 K files

    /*10✔*/  uint32_t NO_OF_DIR_ENTRIES : 21;            //^ 1 K folders + 1 M files = 2^10 + 2^20 ~ 1 M enteries
    /*11✔*/  uint16_t DIR_ENTRY_SIZE_IN_BYTES : 12;      //^ 2280 bits = 285 bytes
    /*12✔*/  uint64_t DIR_SIZE : 33;                     //^ 285 x 2^21 = 597688320 = 570 MB
    /*13✔*/  uint64_t DIR_SIZE_IN_CLSTRS;                //^ dir size / clstr size + (dir size % cluster size == 0) ? 0 : 1 --- 

    /*14✔*/  uint32_t FAT_ENTRIES : 20;                  //^ no of clusters = 2^20 = 1 MB
    /*15✔*/  uint32_t FAT_SIZE;                          //^ FAT ENTRIES * 20 bits = 20971520 / 8 = 2621440 ~ 2 MB
    /*16✔*/  uint32_t FAT_SIZE_IN_CLSTRS;                //^ (3(for 1st free reg) + fat size) / cluster size  + (fat size % cluster size == 0) ? 0 : 1 --- 

    /*17✔*/  uint32_t FIRST_FREE_CLSTR : 20;             //^ a value of cluster idx 
    /*18✔*/  uint8_t FREE_CLSTR_REG_IDX;                 //^ location of this reg = 1st 20 bits of file = 0th loc

                                                       //^ REG FOR FIRST FREE CLSTR + FAT SIZE + DIR SIE 
    /*19✔*/  uint32_t CLUSTER_OFFSET_IN_BYTES;           //^ 20/8 + FAT_size + DIR_size = 4782555156 th bit 
    /*20✔*/  uint32_t CLUSTER_OFFSET_IN_CLSTRS;          //^ fat size in clstr + dir size in clstr
                    

    //^ REG FOR FIRST FREE CLSTR + FAT SIZE + DIR SIZE + SIZE OF DISK
    /*21✔*/  uint64_t TOTAL_SIZE;                        //^ 20/8 + 2621440 + 597688320 + 2^40 = 1.100111938e12
    /*22✔*/  uint32_t TOTAL_CLSTRS;                      //^ CLUSTER_OFFSET_IN_CLSTRS+ NO_OF_CLSTRS_FOR_FILES

} meta_data;

extern meta_data md;


int8_t compute_meta_data(meta_data* md, __uint128_t disk_size, uint64_t cluster_size, uint16_t max_file_size_in_clusters, uint8_t max_file_name_in_bytes, uint16_t max_folders);
void print_meta_data(const meta_data* md);
int8_t write_meta_data_in_file(const char *file_name, const meta_data *data);

/*


DISK_SIZE: 67108864 Bytes
CLUSTER_SIZE: 1024 Bytes
NO_OF_CLUSTERS: 65536
MAX_FILE_SIZE_IN_CLUSTERS: 128 clusters
MAX_FILE_SIZE: 131072 Bytes
MAX_FILE_NAME_IN_BYTES: 64 Bytes
MAX_FOLDERS: 1024
MAX_FILES: 65536
MAX_FULL_SIZE_FILES: 512 files
NO_OF_DIR_ENTRIES: 66560 entries
DIR_ENTRY_SIZE_IN_BYTES: 285 bits
DIR_SIZE: 18969600 bytes
DIR_SIZE_IN_CLSTRS: 18525 clusters
FAT_ENTRIES: 65536 entries
FAT_SIZE: 163840 bytes
FAT_SIZE_IN_CLSTRS: 161 clusters
FIRST_FREE_CLSTR: 0
FREE_CLSTR_REG_IDX: 0
CLUSTER_OFFSET_IN_BYTES: 19133443 bytes
CLUSTER_OFFSET_IN_CLSTRS: 18686 clusters
TOTAL_SIZE: 86242306 bytes
TOTAL_CLSTRS: 84222 

*/
