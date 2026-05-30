#ifndef COMMANDS_H
#define COMMANDS_H

#include <poll.h>
#include <stdbool.h>

typedef struct {
    int fd;
    char username[32];
    char id[6];
} ClientProfile;

typedef struct {
    char *cmd_name;
    void (*handler)(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
    const char *description;
} Command;

bool check_cmd(const char *target);

void run_cmd(const char *target, int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

int handle_command(char *buffer, int client_idx, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

#endif // COMMANDS_H
