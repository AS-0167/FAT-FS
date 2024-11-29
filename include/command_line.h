#pragma once
#include "utils.h"


#define CD_FOLDER 0
#define CD_PARENT 1
#define LS_SELF 2
#define LS_FOLDER 3
#define CAT 4
#define TOUCH 5
#define VIM 6
#define MKDIR 7
#define RMDIR 8
#define RM 9
#define INVALID -1

int input_command(char** f_name);
int parse_command(const char *input, char *f_name);

