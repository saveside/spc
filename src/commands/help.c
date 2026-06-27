#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>
#include <commands.h>

void cmd_help(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int client_fd = fds[client_idx].fd;
    char help_buffer[2048];
    int offset = 0;

    offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset,
                       "\n============ AVAILABLE COMMANDS ============\n");

    for (int i = 0; i < command_count; i++) {
        offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset,
                           "  /%-8s - %s\n",
                           command_list[i].cmd_name,
                           command_list[i].description);
    }

    offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset,
                       "============================================\n>");

    sendToClient(client_fd, help_buffer);
}
