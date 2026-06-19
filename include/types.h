#ifndef TYPES_H
#define TYPES_H

typedef struct {
    int fd;
    char username[32];
    char id[16];
    char currentRoom[32];
} ClientProfile;

#endif // TYPES_H
