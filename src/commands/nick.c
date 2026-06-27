#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_nick(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int client_fd = fds[client_idx].fd;
    char *arg = tokens[1];

    if (arg == NULL || strlen(arg) == 0) {
        sendToClient(client_fd, "Usage: /nick <new_nickname>\r\n> ");
        return;
    }

    strncpy(profiles[client_idx].username, arg, sizeof(profiles[client_idx].username) - 1);
    profiles[client_idx].username[sizeof(profiles[client_idx].username) - 1] = '\0';
}
