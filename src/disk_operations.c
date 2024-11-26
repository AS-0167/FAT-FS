#include"../include/disk_operations.h"

int disk_read(cluster* clstr, FILE* file, uint64_t byte_offset) {
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
            return 1;       //~ file ended 
        } else if (ferror(file)) {
            perror("Error reading from file");
            return -1;      //~ file not read
        }
    }
    fseek(file, 0, SEEK_SET);
    return 0;           //~ complete cluster read
}

int disk_write(cluster* clstr, FILE* file, uint64_t byte_offset) {
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
    return 0;           //~ complete cluster written
}

