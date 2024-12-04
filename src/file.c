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

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // Consume all characters until newline or EOF
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

    printf("Enter the file content (type 'tixe' on a new line to stop):\n");

    *content[0] = '\0';

    while (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);

        if (strncmp(buffer, "tixe", 4) == 0 && (len == 4 || buffer[4] == '\n')) {
            break;
        }

        if (size + len + 1 > buffer_size) {
            buffer_size *= 2;
            char* new_content = realloc(*content, buffer_size);
            if (!new_content) {
                perror("Failed to reallocate memory");
                free(*content);
                return 0;
            }
            *content = new_content;
        }
        memcpy(*content + size, buffer, len);
        size += len;
    }

    (*content)[size] = '\0';  // Null-terminate the content
    clear_input_buffer();     // Clear any leftover input in the buffer
    return size;
}

int8_t write_in_file(FILE* file, directory* directory, FAT* fat, uint32_t* first_free, uint32_t idx, char** content, uint32_t size) {
    if (!file || !directory || !fat || !*content) return INVALID_VALUE();
    if (idx >= directory->no_of_entries) return FIELD_LIMITATION_ERROR();
    dir_entry* entry = &directory->entries[idx];
    entry->firstClstr = *first_free;
    
    uint32_t prev = *first_free;
    uint32_t iterations = size / md.CLUSTER_SIZE;
    iterations += (size % md.CLUSTER_SIZE == 0) ? 0 : 1; 
    uint32_t current_cluster = entry->firstClstr;
    
    uint64_t offset = md.CLUSTER_OFFSET_IN_CLSTRS + (current_cluster * md.CLUSTER_SIZE);

    for (uint32_t i = 0 ;i < iterations; i++) {

        cluster clstr;
        clstr.size = md.CLUSTER_SIZE;
        clstr.buffer = malloc(clstr.size);
        if (!clstr.buffer) return ALLOCATION_ERROR();
    
        memcpy(clstr.buffer, *content, clstr.size);
        *content += clstr.size;

        // Write the cluster to the file
        printf("\n\nCr clstr : %d",current_cluster );
        printf("\n\nCr offset : %d",offset );
        printf("%s", clstr.buffer);

        if (disk_write(&clstr, file, offset) < 0) return OPERATION_UNSUCCESSFUL(); 
        free(clstr.buffer);
        uint32_t prev2 = *first_free;
        if (!add_link(fat, prev, current_cluster, first_free)) return OPERATION_UNSUCCESSFUL();
        prev = prev2;
        current_cluster = *(first_free);
        offset = current_cluster * md.CLUSTER_SIZE;
    }
    // if (!add_link(fat, prev, current_cluster, first_free)) return OPERATION_UNSUCCESSFUL();
    
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

    *content = (char*)malloc(total_size + 1 * sizeof(char));  // Allocate memory for the file content
    if (!*content) return ALLOCATION_ERROR();
    char* write_ptr = *content;  // Separate pointer for writing
  
    uint64_t offset = md.CLUSTER_OFFSET_IN_CLSTRS + (current_cluster * md.CLUSTER_SIZE);

    printf("\nclstr off set : %d", md.CLUSTER_OFFSET_IN_CLSTRS);
    while (current_cluster != UINT32_MAX) {
    
        printf("\n\nCr clstr : %d",current_cluster );
        printf("\n\noffset : %d",offset );
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
        memcpy(write_ptr, clstr.buffer, to_copy);  // Use write_ptr for copying
        write_ptr += to_copy;  // Move write_ptr forward
        printf("%s", clstr.buffer);
        bytes_read += to_copy;

        free(clstr.buffer);

        current_cluster = fat->entries[current_cluster];

        offset = current_cluster * md.CLUSTER_SIZE;
    }
    *write_ptr = '\0';  // Null-terminate the content
    *size = bytes_read;
    return 1;  //~ Successfully read from file
}
