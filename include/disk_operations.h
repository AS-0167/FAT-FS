#pragma once
#include"./utils.h"

typedef struct cluster {
    unsigned char* buffer;
    uint32_t size : 20;
}cluster;

int disk_read(cluster* clstr, FILE* file, uint64_t byte_offset);
int disk_write(cluster* clstr, FILE* file, uint64_t byte_offset);

/*
🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔🤔
*/