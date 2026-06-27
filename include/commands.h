#ifndef COMMANDS_H
#define COMMANDS_H

#include <poll.h>
#include <stdbool.h>
#include "types.h"
#define MAX_TOKENS 6

typedef struct {
    char *cmd_name;
    void (*handler)(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
    const char *description;
} Command;

extern Command command_list[];
extern const int command_count;

bool check_cmd(const char *target);

void run_cmd(const char *target, int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

int handle_command(char *buffer, int client_idx, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

#endif // COMMANDS_H
