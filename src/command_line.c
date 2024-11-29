#include "../include/command_line.h"


int input_command(char* f_name) {

    char input[256];
    f_name = (char*)malloc(256 * sizeof(char));
    if (!f_name) return ALLOCATION_ERROR();
    
    printf("Enter a command: ");
    fgets(input, sizeof(input), stdin);
    
    input[strcspn(input, "\n")] = '\0';
    int op_id = parse_command(input, f_name);
    return op_id;
}

int parse_command(const char *input, char *f_name) {
    char command[256];
    char name[256];
    int matched = sscanf(input, "%s %255s", command, name);

    // Clear f_name before using it
    f_name[0] = '\0';

    if (matched == 2) {
        strcpy(f_name, name);
    } else if (matched == 1) {
        f_name[0] = '\0';  // Clear f_name for commands without names
    }

    if (strcmp(command, "cd") == 0) {
        if (strcmp(f_name, "..") == 0) {
            return CD_PARENT;
        } else {
            return CD_FOLDER;
        }
    } else if (strcmp(command, "ls") == 0) {
        if (strcmp(f_name, ".") == 0 || strcmp(f_name, "\0") == 0) {
            return LS_SELF;
        } else {
            return LS_FOLDER;
        }
    } else if (strcmp(command, "cat") == 0) {
        return CAT;
    } else if (strcmp(command, "touch") == 0) {
        return TOUCH;
    } else if (strcmp(command, "vim") == 0) {
        return VIM;
    } else if (strcmp(command, "mkdir") == 0) {
        return MKDIR;
    } else if (strcmp(command, "rmdir") == 0) {
        return RMDIR;
    } else if (strcmp(command, "rm") == 0) {
        return RM;
    }
    return INVALID;
}