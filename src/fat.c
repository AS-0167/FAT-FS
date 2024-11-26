#include"../include/fat.h"

FAT initialize_fat(uint32_t fat_entries) {
    FAT fat;
    fat.no_of_entries = fat_entries;
    fat.entries = (FAT *)malloc(fat_entries * sizeof(FAT));   
    for (uint32_t i = 0; i < fat_entries; i++) {
        fat.entries[i] = i + 1;
    }
    fat.entries[fat_entries - 1] = INT32_MAX;
    return fat;
}
void write_fat(FILE *file, const FAT *fat) {
    if (file == NULL || fat == NULL || fat->entries == NULL) {
        perror("Invalid input to write_fat");
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

    size_t entries_per_cluster = md.CLUSTER_SIZE / sizeof(uint32_t);
    uint64_t offset = 0;

    for (uint32_t clstr_idx = 0; clstr_idx * entries_per_cluster < fat->no_of_entries; clstr_idx++) {
        memset(clstr.buffer, 0, md.CLUSTER_SIZE);

        for (size_t i = 0; i < entries_per_cluster; i++) {
            uint32_t entry_idx = clstr_idx * entries_per_cluster + i;
            if (entry_idx < fat->no_of_entries) {
                uint32_t *entry = &fat->entries[entry_idx];
                memcpy(clstr.buffer + i * sizeof(uint32_t), (unsigned char *)entry, sizeof(uint32_t));
            } else {
                break;
            }
        }

        if (disk_write(&clstr, file, offset) < 0) {
            perror("Error writing FAT cluster to file");
            free(clstr.buffer);
            return;
        }
        offset += md.CLUSTER_SIZE;
    }

    free(clstr.buffer);
    printf("FAT written successfully to the file.\n");
}

void read_fat(FILE *file, FAT *fat) {
    if (file == NULL || fat == NULL || fat->entries == NULL) {
        perror("Invalid input to read_fat");
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

    size_t entries_per_cluster = md.CLUSTER_SIZE / sizeof(uint32_t);
    uint64_t offset = 0;
    fat->no_of_entries = 0;

    for (uint32_t clstr_idx = 0; clstr_idx * entries_per_cluster < md.FAT_ENTRIES; clstr_idx++) {
        if (disk_read(&clstr, file, offset) < 0) {
            perror("Error reading FAT cluster from file");
            free(clstr.buffer);
            return;
        }
        offset += md.CLUSTER_SIZE;

        for (size_t i = 0; i < entries_per_cluster; i++) {
            uint32_t entry_idx = clstr_idx * entries_per_cluster + i;
            if (entry_idx >= md.FAT_ENTRIES) {
                break;
            }

            uint32_t *entry = (uint32_t *)(clstr.buffer + i * sizeof(uint32_t));
            fat->entries[entry_idx] = *entry;
            fat->no_of_entries++;

            if (fat->no_of_entries >= md.FAT_ENTRIES) {
                break;
            }
        }
        memset(clstr.buffer, '\0', clstr.size);

        if (fat->no_of_entries >= md.FAT_ENTRIES) {
            break;
        }
    }

    free(clstr.buffer);
    printf("FAT read successfully from the file.\n");
}


void print_fat(const FAT* fat) {
    if (fat == NULL || fat->entries == NULL) return;
    for (uint32_t i = 0; i < fat->no_of_entries; i++) {
        printf("%d -> ", fat->entries[i]);
    }
    
}

