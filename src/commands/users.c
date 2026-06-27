#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

#define MAX_CLIENTS 10

void cmd_users(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int client_fd = fds[client_idx].fd;
    char users_buffer[2048];
    int offset = 0;

    offset += snprintf(users_buffer + offset, sizeof(users_buffer) - offset,
                       "\n============ USERS IN ROOM ============\n");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(profiles[client_idx].currentRoom, profiles[i].currentRoom) == 0) {
            offset += snprintf(users_buffer + offset, sizeof(users_buffer) - offset,
                           "  %-8s\n",
                           profiles[i].username);
        }
    }

    offset += snprintf(users_buffer + offset, sizeof(users_buffer) - offset,
                       "=======================================\n>");

    sendToClient(client_fd, users_buffer);
}
