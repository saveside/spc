#include <unistd.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_quit(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int client_fd = fds[client_idx].fd;

    sendToClient(client_fd, "Goodbye!\n");
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    log_warn("The user named %s with fd: %d, left.", profiles[client_idx].username, client_fd);

    fds[client_idx] = fds[*cnfds - 1];
    profiles[client_idx] = profiles[*cnfds - 1];
    (*cnfds)--;
}
