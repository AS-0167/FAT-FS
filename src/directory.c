#include"../include/directory.h"

directory initialize_directory(uint32_t num_entries) {
    directory dir;
    dir.entries = (dir_entry *)malloc(num_entries * sizeof(dir_entry));
    if (!dir.entries) {
        perror("Failed to allocate memory for directory entries");
        exit(EXIT_FAILURE);
    }
    dir.no_of_entries = num_entries;

    uint32_t time = get_current_epoch_time(); 
    
    dir.entries[0] = (dir_entry){
        .idx = 0,
        .parentIdx = 0,
        .name = "root",
        .ext = "\0",
        .size = 0,
        .firstClstr = 0,
        .accessbit = 0,
        .deleted = 0,
        .isfile = 0,
        .isValid = 1,
        .creationEpoch = time,
        .accessEpoch = time,
        .modifyEpoch = time,
    };
    
    for (uint32_t i = 1; i < num_entries; i++) {
        dir.entries[i] = (dir_entry){
            .idx = i,
            .parentIdx = i - 1,
            .name = "",
            .ext = "",
            .size = 0,
            .firstClstr = 0,
            .accessbit = 0,
            .deleted = 0,
            .isfile = 0,
            .isValid = 0,
            .creationEpoch = 0,
            .accessEpoch = 0,
            .modifyEpoch = 0,
        };
    }
    return dir;
}

uint32_t find_idx(const directory* directory) {
    for (uint32_t i = 0; i < directory->no_of_entries; i++) {
        if(!directory->entries[i].isValid) {
            return i;
        }
    }
    return 0;
}
void update_ith_dir_entry(directory *directory, uint32_t index, dir_entry new_entry) {
    if (index >= directory->no_of_entries) {
        printf("Error: Index out of bounds.\n");
        return;
    }
    new_entry.idx = index;
    directory->entries[index] = new_entry; // Overwrite the existing entry
    printf("Directory entry at index %u updated successfully.\n", index);
}
void insert_entry(directory* directory, dir_entry new_entry) {
    uint32_t idx = find_idx(directory);
    if (!idx) {
        printf("Error: No space left in directory! \n");
        return;
    }
    update_ith_dir_entry(directory, idx, new_entry);
}

void write_directory(FILE *file, const directory *dir) {
    if (file == NULL || dir == NULL || dir->entries == NULL) {
        perror("Invalid input to write_directory");
        return;
    }

    if (md.DIR_ENTRY_SIZE_IN_BYTES != sizeof(dir_entry)) {
        fprintf(stderr, "Mismatch between DIR_ENTRY_SIZE_IN_BYTES and dir_entry size.\n");
        return;
    }

    if (md.CLUSTER_SIZE == 0 || md.CLUSTER_SIZE > (1 << 20)) {
        fprintf(stderr, "Invalid CLUSTER_SIZE: %u\n", md.CLUSTER_SIZE);
        return;
    }


    cluster clstr;
    clstr.buffer = malloc(md.CLUSTER_SIZE);
    clstr.size = md.CLUSTER_SIZE;
    if (clstr.buffer == NULL) {
        perror("Failed to allocate memory for cluster buffer");
        return;
    }

    size_t entries_per_cluster = md.CLUSTER_SIZE / md.DIR_ENTRY_SIZE_IN_BYTES;
    uint64_t offset = md.FAT_SIZE_IN_CLSTRS * md.CLUSTER_SIZE;

    for (uint32_t clstr_idx = 0; clstr_idx * entries_per_cluster < dir->no_of_entries; clstr_idx++) {
        memset(clstr.buffer, 0, md.CLUSTER_SIZE);

        for (size_t i = 0; i < entries_per_cluster; i++) {
            uint32_t entry_idx = clstr_idx * entries_per_cluster + i;
            if (entry_idx < dir->no_of_entries) {
                dir_entry *entry = &dir->entries[entry_idx];
                memcpy(clstr.buffer + i * md.DIR_ENTRY_SIZE_IN_BYTES, (unsigned char*)entry, md.DIR_ENTRY_SIZE_IN_BYTES);
            } else {
                break;
            }
        }

        if (disk_write(&clstr, file, offset) < 0) {
            perror("Error writing directory cluster to file");
            free(clstr.buffer);
            return;
        }
        offset += md.CLUSTER_SIZE;
    }

    free(clstr.buffer);
    printf("Directory written successfully to the file starting at offset %u.\n", md.FAT_SIZE_IN_CLSTRS);
}

void read_directory(FILE *file, directory *dir) {
    if (file == NULL || dir == NULL || dir->entries == NULL) {
        perror("Invalid input to read_directory");
        return;
    }

    if (md.DIR_ENTRY_SIZE_IN_BYTES != sizeof(dir_entry)) {
        fprintf(stderr, "Mismatch between DIR_ENTRY_SIZE_IN_BYTES and dir_entry size.\n");
        return;
    }

    if (md.CLUSTER_SIZE == 0 || md.CLUSTER_SIZE > (1 << 20)) {
        fprintf(stderr, "Invalid CLUSTER_SIZE: %u\n", md.CLUSTER_SIZE);
        return;
    }

    if (fseek(file, md.FAT_SIZE_IN_CLSTRS * md.CLUSTER_SIZE, SEEK_SET) != 0) {
        perror("Error seeking to directory offset in file");
        return;
    }

    cluster clstr;
    clstr.buffer = malloc(md.CLUSTER_SIZE);
    clstr.size = md.CLUSTER_SIZE;
    if (clstr.buffer == NULL) {
        perror("Failed to allocate memory for cluster buffer");
        return;
    }

    size_t entries_per_cluster = md.CLUSTER_SIZE / md.DIR_ENTRY_SIZE_IN_BYTES;
    uint64_t offset = md.FAT_SIZE_IN_CLSTRS * md.CLUSTER_SIZE;
    dir->no_of_entries = 0;

    for (uint32_t clstr_idx = 0; clstr_idx * entries_per_cluster < md.NO_OF_DIR_ENTRIES; clstr_idx++) {
        if (disk_read(&clstr, file, offset) < 0) {
            perror("Error reading directory cluster from file");
            free(clstr.buffer);
            return;
        }
        offset += md.CLUSTER_SIZE;

        for (size_t i = 0; i < entries_per_cluster; i++) {
            uint32_t entry_idx = clstr_idx * entries_per_cluster + i;
            if (entry_idx >= md.NO_OF_DIR_ENTRIES) {
                break;
            }

            dir_entry *entry = (dir_entry *)(clstr.buffer + i * md.DIR_ENTRY_SIZE_IN_BYTES);
            dir->entries[entry_idx] = *entry;
            dir->no_of_entries++;

            if (dir->no_of_entries >= md.NO_OF_DIR_ENTRIES) {
                break;
            }
        }
        memset(clstr.buffer, '\0', clstr.size);

        if (dir->no_of_entries >= md.NO_OF_DIR_ENTRIES) {
            break;
        }
    }

    free(clstr.buffer);
    printf("Directory read successfully from the file starting at offset %u.\n", md.FAT_SIZE_IN_CLSTRS);
}

void print_directory_with_time(const directory* dir) {
    if (dir == NULL || dir->entries == NULL) {
        printf("Invalid directory structure.\n");
        return;
    }

    // Print the table header
    printf("________________________________________________________________________________________________________________________________________________________________________________________________________________\n");
    printf("|   idx    |   parentIdx    |    name    |     ext     |    size    |   firstBlk   |    accessbit     |   deleted     |   isfile    |   isValid    |    CreationTime    |     AccessTime    |     ModifyTime    |\n");
    printf("|__21-bits_|_____21-bits____|__2048-bits_|____32-bits__|___30-bits__|____20-bits___|__(r-w-x)_9-bits__|____1-bit______|____1-bit____|____1-bit_____|_______32-bits______|______32-bits______|______32-bits______|\n");

    // Iterate over all entries in the directory
    for (uint32_t i = 0; i < dir->no_of_entries; ++i) {
        const dir_entry* entry = &dir->entries[i];
        // Only print valid entries
        if (entry->isValid) {
            // Convert times to human-readable format
            char creation_time_str[20] = "N/A";
            char access_time_str[20] = "N/A";
            char modify_time_str[20] = "N/A";

            time_t creation_time = (time_t)entry->creationEpoch;
            time_t access_time = (time_t)entry->accessEpoch;
            time_t modify_time = (time_t)entry->modifyEpoch;

            if (entry->creationEpoch != 0)
                strftime(creation_time_str, sizeof(creation_time_str), "%Y-%m-%d %H:%M:%S", localtime(&creation_time));
            if (entry->accessEpoch != 0)
                strftime(access_time_str, sizeof(access_time_str), "%Y-%m-%d %H:%M:%S", localtime(&access_time));
            if (entry->modifyEpoch != 0)
                strftime(modify_time_str, sizeof(modify_time_str), "%Y-%m-%d %H:%M:%S", localtime(&modify_time));
            // Print the entry details
            printf("&| %-8u | %-14u | %-10s | %-10s | %-10u | %-12u | %-16u | %-12u | %-10u | %-10u | %-18s | %-18s | %-18s |\n",
                   entry->idx,              // 21 bits
                   entry->parentIdx,        // 21 bits
                   entry->name,             // 2048 bits
                   entry->ext,              // 32 bits
                   entry->size,             // 30 bits
                   entry->firstClstr,       // 20 bits
                   entry->accessbit,        // 9 bits
                   entry->deleted,          // 1 bit
                   entry->isfile,           // 1 bit
                   entry->isValid,          // 1 bit
                   creation_time_str,       // 32 bits
                   access_time_str,         // 32 bits
                   modify_time_str          // 32 bits
            );
        }
    }
    printf("________________________________________________________________________________________________________________________________________________________________________________________________________________\n");
}

void print_directory(const directory* dir) {
    if (dir == NULL || dir->entries == NULL) {
        printf("Invalid directory structure.\n");
        return;
    }

    // Print the table header
    printf("\n____________________________________________________________________________________________________________________________________________________\n");
    printf("|   idx    |   parentIdx    |    name    |     ext     |    size    |   firstBlk   |    accessbit     |   deleted     |   isfile    |   isValid    |\n");
    printf("|__21-bits_|_____21-bits____|__2048-bits_|____32-bits__|___30-bits__|____20-bits___|__(r-w-x)_9-bits__|____1-bit______|____1-bit____|____1-bit_____| \n");

    // Iterate over all entries in the directory
    for (uint32_t i = 0; i < dir->no_of_entries; ++i) {
        const dir_entry* entry = &dir->entries[i];
        // Only print valid entries
        if (entry->isValid) {
            // Print the entry details without times
            printf("| %-8u | %-14u | %-10s | %-10s  | %-10u | %-12u | %-16u | %-12u  | %-10u  |  %-10u  |\n",
                   entry->idx,              // 21 bits
                   entry->parentIdx,        // 21 bits
                   entry->name,             // 2048 bits
                   entry->ext,              // 32 bits
                   entry->size,             // 30 bits
                   entry->firstClstr,       // 20 bits
                   entry->accessbit,        // 9 bits
                   entry->deleted,          // 1 bit
                   entry->isfile,           // 1 bit
                   entry->isValid           // 1 bit
            );
            printf("|__________________________________________________________________________________________________________________________________________________|\n");
        }
    }
}
