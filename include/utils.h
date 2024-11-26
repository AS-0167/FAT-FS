#pragma once
#include<stdio.h>
#include<stdlib.h>
#include <stdint.h>
#include<time.h>
#include<string.h>

uint32_t get_current_epoch_time(void);
int limitation_error(void);
int field_limitation_error(void);

#include"meta_data.h"
#include"disk_operations.h"
#include"directory.h"
#include"folder.h"
#include"file.h"
#include"fat.h"

meta_data md;

uint32_t get_current_epoch_time() {
    time_t current_time = time(NULL);

    if (current_time == ((time_t)-1)) {
        perror("Error getting the current time");
        return 0; 
    }

    return (uint32_t)current_time;
}

int limitation_error() {
    perror("DISK LIMITATION EXCEEDED !");
    return 0;
}
int field_limitation_error() {
    perror("FEILD LIMITATION EXCEEDED !");
    return 0;
}
