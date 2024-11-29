#include"../include/disk_operations.h"

int8_t disk_read    (   cluster* clstr, 
                        FILE* file, 
                        uint64_t byte_offset
                    )
{

    if (clstr == NULL || file == NULL) {
        perror("Invalid input to read_cluster");
        return -1;      //~ file not read
    }

    if (fseek(file, byte_offset, SEEK_SET) != 0) {
        perror("Error seeking to cluster position in file");
        return -1;     //~ File not read
    }

    size_t bytesRead = fread(clstr->buffer, 1, clstr->size, file);

    if (bytesRead < clstr->size) {
        if (feof(file)) {
            printf("Reached end of file while reading.\n");
            return 0;       //~ file ended 
        } else if (ferror(file)) {
            perror("Error reading from file");
            return -1;      //~ file not read
        }
    }
    fseek(file, 0, SEEK_SET);
    return 1;           //~ complete cluster read
}

int8_t disk_write   (   cluster* clstr, 
                        FILE* file, 
                        uint64_t byte_offset
                    )
{
    if (clstr == NULL || file == NULL) {
        perror("Invalid input to write_cluster");
        return -1;      //~ file not written
    }
    
    if (fseek(file, byte_offset, SEEK_SET) != 0) {
        perror("Error seeking to cluster position in file");
        return -1;     //~ File not read
    }
    size_t bytesWritten = fwrite(clstr->buffer, 1, clstr->size, file);

    if (bytesWritten < clstr->size) {
        perror("Error writing to file");
        return -1;      //~ file not written
    }
    fseek(file, 0, SEEK_SET);
    return 1;           //~ complete cluster written
}


int8_t update_first_free    (
                                FILE* file, 
                                uint32_t first_free
                            )
{
    if (first_free > 0xFFFFF) return FIELD_LIMITATION_ERROR();
    if (!file) return FILE_ERROR();

    cluster clstr;
    clstr.buffer = (uint8_t *)malloc(clstr.size);
    if (!clstr.buffer) return ALLOCATION_ERROR();
    clstr.size = 3;

    clstr.buffer[0] = (first_free >> 12) & 0xFF;        // High 8 bits
    clstr.buffer[1] = (first_free >> 4) & 0xFF;         // Middle 8 bits
    clstr.buffer[2] = (first_free & 0xF) << 4;          // Low 4 bits

    if (disk_write(&clstr, file, 0) == 1) {
        free(clstr.buffer);
        return 1;
    }
    free(clstr.buffer);
    return  OPERATION_UNSUCCESSFUL();
}

int8_t read_first_free(FILE *file, uint32_t *first_free) {
    if (!file) return FILE_ERROR();

    cluster clstr;
    clstr.size = 3;
    clstr.buffer = (uint8_t *)malloc(clstr.size);
    if (!clstr.buffer) return ALLOCATION_ERROR();

    if (disk_read(&clstr, file, 0) < 0) {
        free(clstr.buffer);
        return OPERATION_UNSUCCESSFUL();
    }

    *first_free = ((uint32_t)clstr.buffer[0] << 12) |    // High 8 bits
                  ((uint32_t)clstr.buffer[1] << 4)  |    // Middle 8 bits
                  ((uint32_t)(clstr.buffer[2] >> 4));    // Low 4 bits

    free(clstr.buffer);
    return 1;
}

void create_disk    (   
                        const char* disk_name, 
                        uint64_t disk_size
                    ) 
{
    FILE* file = fopen(disk_name, "wb");
    
    fseek(file, disk_size , SEEK_SET);
    fputc('\0', file);
    fclose(file);

}

int8_t formate_disk    (   
                            FILE* disk, 
                            const char* disk_name,
                            uint64_t disk_size,
                            uint32_t cluster_size,
                            uint16_t max_file_size_in_clstr,
                            uint8_t max_file_name_in_bytes,
                            uint16_t max_folders
                        )
{    
    if (!disk) return FILE_ERROR();
    if (compute_meta_data(&md, disk_size, cluster_size, max_file_size_in_clstr, max_file_name_in_bytes, max_folders)) {
        
        print_meta_data(&md);
        write_meta_data_in_file("meta_data.bin", &md);
        create_disk(disk_name, md.TOTAL_SIZE);
        
        directory directory = initialize_directory(md.NO_OF_DIR_ENTRIES);
        FAT fat = initialize_fat(md.FAT_ENTRIES);
        uint32_t first_free = 5;
        
        if (!update_first_free(disk, first_free)) return OPERATION_UNSUCCESSFUL();
        if (!read_first_free(disk, &first_free)) return OPERATION_UNSUCCESSFUL();
        printf("\nfirst free : %u\n", first_free);
        
        if (!write_fat(disk, &fat)) return OPERATION_UNSUCCESSFUL();
        if (!read_fat(disk, &fat)) return OPERATION_UNSUCCESSFUL();
        print_fat(&fat);

        print_directory(&directory);
        if (!write_directory(disk, &directory)) return OPERATION_UNSUCCESSFUL();
        if (!read_directory(disk, & directory)) return OPERATION_UNSUCCESSFUL();
        print_directory(&directory);

        return 1;
    }
    return 0;

}
