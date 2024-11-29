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

