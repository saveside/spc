#ifndef TYPES_H
#define TYPES_H

#include "stdbool.h"

typedef struct {
    int fd;
    char username[32];
    char id[16];
    char currentRoom[32];
    bool isMuted;
} ClientProfile;

#endif // TYPES_H
