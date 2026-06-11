#ifndef TYPES_H
#define TYPES_H

typedef struct {
    int fd;
    char username[32];
    char id[6];
    char currentRoom[6];
} ClientProfile;

#endif // TYPES_H
