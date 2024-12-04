#include"../include/fat.h"
#include"../include/meta_data.h"
#include"../include/disk_operations.h"


/*
0->1->2->3->NA
4->5->NA

n = 1;

*/

FAT initialize_fat  (
                        uint32_t fat_entries
                    ) 
{
    FAT fat;
    fat.no_of_entries = fat_entries;
    fat.entries = (uint32_t *)malloc(fat_entries * sizeof(uint32_t));   
    for (uint32_t i = 0; i < fat_entries; i++) {
        fat.entries[i] = i + 1;
    }
    fat.entries[fat_entries - 1] = UINT32_MAX;
    return fat;
}

int8_t write_fat    (   
                        FILE *file, 
                        const FAT *fat
                    )
{
    if (file == NULL) return FILE_ERROR();
    if (fat == NULL || fat->entries == NULL) return INVALID_VALUE();
    if (md.CLUSTER_SIZE == 0 || md.CLUSTER_SIZE > (1 << 20)) return FIELD_LIMITATION_ERROR();

    cluster clstr;
    clstr.buffer = malloc(md.CLUSTER_SIZE);
    clstr.size = md.CLUSTER_SIZE;
    if (clstr.buffer == NULL) return ALLOCATION_ERROR(); 

    size_t entries_per_cluster = md.CLUSTER_SIZE / sizeof(uint32_t);
    uint64_t offset = md.CLUSTER_SIZE;

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
        // printf("\nfat offset : %d", offset);
        if (disk_write(&clstr, file, offset) < 0) {
            free(clstr.buffer);
            return OPERATION_UNSUCCESSFUL();
        }
        offset += md.CLUSTER_SIZE;
    }
    free(clstr.buffer);
    printf("FAT written successfully to the file.\n");
    return 1;
}

int8_t read_fat (   
                    FILE *file, 
                    FAT *fat, 
                    uint32_t fat_entries
                ) 
{
    if (file == NULL) return FILE_ERROR();

    fat->no_of_entries = fat_entries;
    fat->entries = (uint32_t *)malloc(fat->no_of_entries * sizeof(uint32_t));   

    if (fat->entries == NULL) return INVALID_VALUE();
    if (md.CLUSTER_SIZE == 0 || md.CLUSTER_SIZE > (1 << 20)) return FIELD_LIMITATION_ERROR();

    cluster clstr;
    clstr.buffer = malloc(md.CLUSTER_SIZE);
    clstr.size = md.CLUSTER_SIZE;
    if (clstr.buffer == NULL) return ALLOCATION_ERROR();

    size_t entries_per_cluster = md.CLUSTER_SIZE / sizeof(uint32_t);
    uint64_t offset = md.CLUSTER_SIZE;
    fat->no_of_entries = 0;

    for (uint32_t clstr_idx = 0; clstr_idx * entries_per_cluster < md.FAT_ENTRIES; clstr_idx++) {
        if (disk_read(&clstr, file, offset) < 0) {
            free(clstr.buffer);
            return OPERATION_UNSUCCESSFUL();
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
    return 1;
}


int8_t extend_link  (
                        FAT* fat, 
                        uint32_t prev, 
                        uint32_t nxt
                    ) 
{
    fat->entries[prev] = nxt;
    fat->entries[nxt] = UINT32_MAX;
    return 1;
}

int8_t add_link   (
                    FAT* fat, uint32_t prev_extended,
                    uint32_t to_extend, 
                    uint32_t* first_free
                ) 
{
    if (*first_free == UINT32_MAX) {
        perror("NO space left in disk !");
        return 0;
    }
    uint32_t firstFree = *(first_free);
    uint32_t nxtFree = fat->entries[firstFree];
    *(first_free) = nxtFree;
    fat->entries[prev_extended] = firstFree;
    fat->entries[firstFree] = UINT32_MAX;
    return 1;
}


void print_fat  (   
                    const FAT* fat
                ) 
{
    if (fat == NULL || fat->entries == NULL) return;
    // for (uint32_t i = 0; i < fat->no_of_entries; i++) {
    //     printf("%d -> ", fat->entries[i]);
    // }
    
    for (uint32_t i = 0; i < 20; i++) {
        printf("%d -> %d -- ", i, fat->entries[i]);
    }
}

