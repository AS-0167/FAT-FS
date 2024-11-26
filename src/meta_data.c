#include"../include/meta_data.h"

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
