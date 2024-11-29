#include"../include/file.h"


int8_t create_file(directory* directory, char* f_name, uint32_t parent_idx) {
    if (strlen(f_name) > md.MAX_FILE_NAME_IN_BYTES) return FIELD_LIMITATION_ERROR();
    uint32_t time = get_current_epoch_time();
    dir_entry new_entry = (dir_entry){
        .idx = 0,
        .parentIdx = parent_idx,
        .name = "",
        .ext = "\0",
        .size = 0,
        .firstClstr = 0,
        .accessbit = 0,
        .deleted = 0,
        .isfile = 1,
        .isValid = 1,
        .creationEpoch = time,
        .accessEpoch = time,
        .modifyEpoch = time,
    };
    char name[256], ext[4];
    separate_filename_and_extension(f_name, name, ext);
    strncpy(new_entry.name, name, strlen(name));
    strncpy(new_entry.ext, ext, strlen(ext));
    new_entry.name[strlen(name)] = '\0';
    new_entry.ext[strlen(ext)] = '\0';

    insert_entry(directory, new_entry);

    return 1;
}

uint32_t input_file_content(char** content) {
    size_t buffer_size = 256;
    *content = malloc(buffer_size);
    if (!*content) {
        perror("Failed to allocate memory");
        return 0;
    }

    uint32_t size = 0;
    char buffer[256];

    printf("Enter the file content (press Ctrl+D to finish):\n");

    // Clear the content buffer
    *content[0] = '\0';
   // Read input line by line
    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);

        // Check if more space is needed
        if (size + len + 1 > buffer_size) {  // +1 for null terminator
            buffer_size *= 2;
            char* new_content = realloc(*content, buffer_size);
            if (!new_content) {
                perror("Failed to reallocate memory");
                free(*content);
                return 0;
            }
            *content = new_content;
        }

        strcat(*content, buffer);  // Append to the content
        size += len;
    }
    return size;
}

int8_t write_in_file(FILE* file, directory* directory, FAT* fat, uint32_t* first_free, uint32_t idx, char** content, uint32_t size) {
    if (!file || !directory || !fat || !*content) return INVALID_VALUE();
    if (idx >= directory->no_of_entries) return FIELD_LIMITATION_ERROR();
    dir_entry* entry = &directory->entries[idx];
    
    if (entry->size == 0) {
        if (!add_link(fat, entry->firstClstr, first_free)) return OPERATION_UNSUCCESSFUL();
        entry->firstClstr = *(first_free);
    }

    int bytes_remaining = size;
    uint32_t current_cluster = entry->firstClstr;
    uint64_t offset = current_cluster * md.CLUSTER_SIZE;

    while (bytes_remaining > 0) {
        cluster clstr;
        clstr.size = md.CLUSTER_SIZE;
        clstr.buffer = malloc(clstr.size);
        if (!clstr.buffer) return ALLOCATION_ERROR();
        printf("here");
        fflush(stdout);

        memcpy(clstr.buffer, *content, clstr.size);
        *content += clstr.size;
        bytes_remaining -= clstr.size;

        // Write the cluster to the file
        if (disk_write(&clstr, file, offset) < 0) return OPERATION_UNSUCCESSFUL(); 
        free(clstr.buffer);
        
        if (bytes_remaining > 0) {
            if (!add_link(fat, current_cluster, first_free)) return OPERATION_UNSUCCESSFUL();
            current_cluster = *(first_free);
        } else {
            fat->entries[current_cluster] = INT32_MAX;
        }
        // Update offset and get next cluster
        offset = current_cluster * md.CLUSTER_SIZE;
    }
    
    // Update the file size in the directory
    if (!update_size(directory, idx, size)) return OPERATION_UNSUCCESSFUL();

    return 1;  //~ Successfully written to file
}


int8_t read_from_file(FILE* file, directory* directory, FAT* fat, uint32_t idx, char** content, uint32_t* size) {
    if (!file || !directory || !fat || !content || !size) return INVALID_VALUE();
    if (idx >= directory->no_of_entries) return FIELD_LIMITATION_ERROR();

    dir_entry* entry = &directory->entries[idx];
    uint32_t current_cluster = entry->firstClstr;
    uint32_t total_size = entry->size;
    uint32_t bytes_read = 0;

    *content = malloc(total_size + 1);  // Allocate memory for the file content
    if (!*content) return ALLOCATION_ERROR();
    memset(*content, 0, total_size + 1);

    uint64_t offset = current_cluster * md.CLUSTER_SIZE;
    while (current_cluster != INT32_MAX && bytes_read < total_size) {
        cluster clstr;
        clstr.size = md.CLUSTER_SIZE;
        clstr.buffer = malloc(clstr.size);
        if (!clstr.buffer) {
            free(*content);
            return ALLOCATION_ERROR();
        }

        if (disk_read(&clstr, file, offset) < 0) {
            free(clstr.buffer);
            free(*content);
            return OPERATION_UNSUCCESSFUL();
        }

        // Copy read data into the content buffer
        uint32_t to_copy = (bytes_read + clstr.size > total_size) ? (total_size - bytes_read) : clstr.size;
        memcpy(*content + bytes_read, clstr.buffer, to_copy);
        bytes_read += to_copy;

        free(clstr.buffer);

        if (fat->entries[current_cluster] == INT32_MAX) {
            break;
        }

        current_cluster = fat->entries[current_cluster];
        offset = current_cluster * md.CLUSTER_SIZE;
    }

    *size = bytes_read;
    return 1;  //~ Successfully read from file
}