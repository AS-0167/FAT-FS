#include"../include/utils.h"

uint32_t get_current_epoch_time() {
    time_t current_time = time(NULL);

    if (current_time == ((time_t)-1)) {
        perror("Error getting the current time");
        return 0; 
    }

    return (uint32_t)current_time;
}

int8_t LIMITATION_ERROR() {
    perror("\nDISK LIMITATION EXCEEDED !\n");
    return 0;
}
int8_t FIELD_LIMITATION_ERROR() {
    perror("\nFEILD LIMITATION EXCEEDED !\n");
    return 0;
}
int8_t FILE_ERROR(void) {
    perror("\nFILE NOT OPENING !");
    return 0;
}
int8_t OPERATION_UNSUCCESSFUL(void) {
    perror("\nTHE LATEST OPERATION PERFORMED IS UNSUCCESSFUL !\n");
    return 0;
}
int8_t INVALID_VALUE(void) {
    perror("\nTHE VALUE IS INVALID !\n");
    return 0;
} 
int8_t ALLOCATION_ERROR(void) {
    perror("\nERROR WHILE ALLOCATING THE MEMORY !\n");
    return 0;
}



void separate_filename_and_extension(const char *filename, char *name, char *extension) {
    const char *dot = strrchr(filename, '.');
    if (dot != NULL && dot != filename) {
        size_t name_length = dot - filename;
        strncpy(name, filename, name_length);
        name[name_length] = '\0';
        strcpy(extension, dot + 1);
    } else {
        strcpy(name, filename);
        extension[0] = '\0'; 
    }
}
