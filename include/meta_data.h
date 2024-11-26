#pragma once
#include"utils.h"

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


int compute_meta_data(meta_data* md, __uint128_t disk_size, uint64_t cluster_size, uint16_t max_file_size_in_clusters, uint8_t max_file_name_in_bytes, uint16_t max_folders) {
    
    if (disk_size > 1.099511628e12) return limitation_error();
    md->DISK_SIZE = disk_size;
    if (cluster_size > 1048576) return limitation_error();
    md->CLUSTER_SIZE = cluster_size;
    md->NO_OF_CLUSTERS_FOR_FILES = md->DISK_SIZE / md->CLUSTER_SIZE;
    
    if (max_file_size_in_clusters > 1024) return limitation_error();
    md->MAX_FILE_SIZE_IN_CLUSTERS = max_file_size_in_clusters;
    md->MAX_FILE_SIZE = md->MAX_FILE_SIZE_IN_CLUSTERS * md->CLUSTER_SIZE;
    if (max_file_name_in_bytes > (uint8_t)255) return limitation_error();
    md->MAX_FILE_NAME_IN_BYTES = max_file_name_in_bytes;
    if (max_folders > 1024) return limitation_error();
    md->MAX_FOLDERS = max_folders;
    md->MAX_FILES = md->NO_OF_CLUSTERS_FOR_FILES;
    md->MAX_FULL_SIZE_FILES = md->DISK_SIZE / md->MAX_FILE_SIZE;

    md->NO_OF_DIR_ENTRIES = md->MAX_FOLDERS + md->MAX_FILES;
    md->DIR_ENTRY_SIZE_IN_BYTES = 288;
    md->DIR_SIZE = md->DIR_ENTRY_SIZE_IN_BYTES * md->NO_OF_DIR_ENTRIES;
    if (md->DIR_SIZE > 597688320) return limitation_error();
    md->DIR_SIZE_IN_CLSTRS = (md->DIR_SIZE / md->CLUSTER_SIZE) + ((md->DIR_SIZE % md->CLUSTER_SIZE == 0) ? 0 : 1);

    md->FAT_ENTRIES = md->NO_OF_CLUSTERS_FOR_FILES;
    md->FAT_SIZE = (md->FAT_ENTRIES * sizeof(uint32_t)); 
    md->FAT_SIZE_IN_CLSTRS = ((3 + md->FAT_SIZE) / md->CLUSTER_SIZE) + (((3 + md->FAT_SIZE) % md->CLUSTER_SIZE == 0) ? 0 : 1);

    md->FIRST_FREE_CLSTR = 0;
    md->FREE_CLSTR_REG_IDX = 0;

    md->CLUSTER_OFFSET_IN_BYTES = 3 + md->FAT_SIZE + md->DIR_SIZE;
    if (md->CLUSTER_OFFSET_IN_BYTES > (uint32_t)4782555155) return limitation_error();
    md->CLUSTER_OFFSET_IN_CLSTRS = md->FAT_SIZE_IN_CLSTRS + md->DIR_SIZE_IN_CLSTRS;

    md->TOTAL_SIZE = (20/8) + md->FAT_SIZE + md->DIR_SIZE + md->DISK_SIZE;
    if (md->TOTAL_SIZE > 1.100111938e12) return limitation_error();
    md->TOTAL_CLSTRS = (md->TOTAL_SIZE / md->CLUSTER_SIZE) + ((md->TOTAL_SIZE % md->CLUSTER_SIZE == 0) ? 0 : 1);


    return 1;
}

void print_meta_data(const meta_data* md) {
    printf("DISK_SIZE: %lu Bytes\n", (unsigned long)md->DISK_SIZE);
    printf("CLUSTER_SIZE: %u Bytes\n", md->CLUSTER_SIZE);
    printf("NO_OF_CLUSTERS: %u\n", md->NO_OF_CLUSTERS_FOR_FILES);
    printf("MAX_FILE_SIZE_IN_CLUSTERS: %u clusters\n", md->MAX_FILE_SIZE_IN_CLUSTERS);
    printf("MAX_FILE_SIZE: %u Bytes\n", md->MAX_FILE_SIZE);
    printf("MAX_FILE_NAME_IN_BYTES: %u Bytes\n", md->MAX_FILE_NAME_IN_BYTES);
    printf("MAX_FOLDERS: %u\n", md->MAX_FOLDERS);
    printf("MAX_FILES: %u\n", md->MAX_FILES);
    printf("MAX_FULL_SIZE_FILES: %u files\n", md->MAX_FULL_SIZE_FILES);
    printf("NO_OF_DIR_ENTRIES: %u entries\n", md->NO_OF_DIR_ENTRIES);
    printf("DIR_ENTRY_SIZE_IN_BYTES: %u bits\n", md->DIR_ENTRY_SIZE_IN_BYTES);
    printf("DIR_SIZE: %lu bytes\n", (unsigned long)md->DIR_SIZE);
    printf("DIR_SIZE_IN_CLSTRS: %lu clusters\n", (unsigned long)md->DIR_SIZE_IN_CLSTRS);
    printf("FAT_ENTRIES: %u entries\n", md->FAT_ENTRIES);
    printf("FAT_SIZE: %u bytes\n", md->FAT_SIZE);
    printf("FAT_SIZE_IN_CLSTRS: %u clusters\n", md->FAT_SIZE_IN_CLSTRS);
    printf("FIRST_FREE_CLSTR: %u\n", md->FIRST_FREE_CLSTR);
    printf("FREE_CLSTR_REG_IDX: %u\n", md->FREE_CLSTR_REG_IDX);
    printf("CLUSTER_OFFSET_IN_BYTES: %u bytes\n", md->CLUSTER_OFFSET_IN_BYTES);
    printf("CLUSTER_OFFSET_IN_CLSTRS: %u clusters\n", md->CLUSTER_OFFSET_IN_CLSTRS);
    printf("TOTAL_SIZE: %lu bytes\n", (unsigned long)md->TOTAL_SIZE);
    printf("TOTAL_CLSTRS: %lu \n", (unsigned long)md->TOTAL_CLSTRS);
}
int write_meta_data_in_file(const char *file_name, const meta_data *data) {
    FILE *file = fopen(file_name, "wb");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    size_t bytes_written = fwrite(data, sizeof(meta_data), 1, file);
    if (bytes_written != 1) {
        perror("Failed to write data to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

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
