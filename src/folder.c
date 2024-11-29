#include"../include/folder.h"

int8_t create_folder(directory* directory, char* f_name, uint32_t parent_idx) {
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
        .isfile = 0,
        .isValid = 1,
        .creationEpoch = time,
        .accessEpoch = time,
        .modifyEpoch = time,
    };
    strncpy(new_entry.name, f_name, strlen(f_name));
    new_entry.name[strlen(f_name)] = '\0';

    insert_entry(directory, new_entry);
    return 1;
}