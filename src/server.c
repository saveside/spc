#include "logger.h"
#include "commands.h"
#include "utils.h"
#include <rooms.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "history.h"

#define BACKLOG 5
#define MAX_CLIENTS 10
#define NFDS (MAX_CLIENTS + 1)
#define PORT 8080

void handle_chat_command(int client_idx, char *buffer, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
      int max_tokens = 10;
      char *tokens[max_tokens];
      int token_count = tokenizer(buffer + 1, tokens, max_tokens);
      char *command = tokens[0];
      int client_fd = fds[client_idx].fd;

      if (check_cmd(command) == false) {
        char *cmdNotFound_msg = "Command Not Found!";
        write(client_fd, cmdNotFound_msg, strlen(cmdNotFound_msg));
      }
      run_cmd(command, client_idx, tokens, token_count, fds, profiles, cnfds);

}

void broadcast_message(int sender_idx, const char *msg, struct pollfd *fds, ClientProfile *profiles, nfds_t cnfds) {
    char chat_msg[1500];
    snprintf(chat_msg, sizeof(chat_msg), "[%s]: %s\n", profiles[sender_idx].username, msg);
    printf("#%s [%s] in Room(%s): %s\n", profiles[sender_idx].id, profiles[sender_idx].username, profiles[sender_idx].currentRoom, msg);

    for (int j = 1; j < cnfds; j++) {
        if (fds[j].fd != fds[sender_idx].fd && fds[j].fd > 0) {
          if (strcmp(profiles[j].currentRoom, profiles[sender_idx].currentRoom) == 0) {
            write(fds[j].fd, chat_msg, strlen(chat_msg));
            write(fds[j].fd, "\n> ", 3);
          }
        }
    }
    write(fds[sender_idx].fd, "\n> ", 3);
}

int init_server() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (sock_fd < 0) {
        log_err("Failed to open a socket.");
        return -1;
    }
    log_info("Created a  socket.");

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_err("Failed to bind socket.");
        close(sock_fd);
        return -1;
    }
    log_info("Bond socket.");

    if (listen(sock_fd, BACKLOG) < 0) {
        log_err("Failed to listen socket.");
        close(sock_fd);
        return -1;
    }
    log_info("Listening socket...");
    log_info("Waiting connections...");

    return sock_fd;
}

void handle_new_connection(int sock_fd, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        log_err("Failed to accept incoming client connection.");
        return;
    }

    if (*cnfds >= NFDS) {
      log_err("Server Full. Rejecting connection on fd: %d", client_fd);
      char *reject_msg = "Server is currently full. Please try again later.\n";
      write(client_fd, reject_msg, strlen(reject_msg));
      close(client_fd);
      return;
    }

    log_info("Client connected with fd: %d", client_fd);

    profiles[*cnfds].fd = client_fd;
    profiles[*cnfds].username[0] = '\0';
    randomId(profiles[*cnfds].id, 5);
    strcpy(profiles[*cnfds].currentRoom, "Lobby");
    profiles[*cnfds].isMuted = false;

    char welcome_msg[256];
    snprintf(welcome_msg, sizeof(welcome_msg),
             "\n==========================================\n"
             "Welcome to the Save's Pocket Chat Server!\n"
             "Your auto-generated ID is: [%s]\n"
             "==========================================\n"
             "Please enter your username: ",
             profiles[*cnfds].id);
    write(client_fd, welcome_msg, strlen(welcome_msg));

    fds[*cnfds].fd = client_fd;
    fds[*cnfds].events = POLLIN;
    fds[*cnfds].revents = 0;
    (*cnfds)++;
}

int handle_client_data(int i, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(fds[i].fd, buffer, sizeof(buffer) - 1);
    buffer[strcspn(buffer, "\r\n")] = '\0';

    if (bytes_read < 0) {
        log_err("Failed to read message.");
        return 0;
    }

    if (bytes_read == 0) {
        log_warn("Client disconnected without sending anything.");
        close(fds[i].fd);
        fds[i] = fds[*cnfds - 1];
        profiles[i] = profiles[*cnfds - 1];
        (*cnfds)--;
        return 1;
    }

    if (profiles[i].isMuted && strcmp(profiles[i].currentRoom, "Lobby") != 0) {
        sendToClient(fds[i].fd, "You are muted!\n");
        return 0;
    }

    if (buffer[0] == '/') {
      handle_chat_command(i, buffer, fds, profiles, cnfds);
      return 0;
    }

    if (profiles[i].username[0] == '\0') {
      for (int j = 0; j < *cnfds; j++) {
        if (strcmp(profiles[j].username, buffer) == 0) {
          sendToClient(fds[i].fd, "This username is taken. Please try a different name.\n> ");
          return 0;
        }
      }
      strncpy(profiles[i].username, buffer, sizeof(profiles[i].username) - 1);
      profiles[i].username[sizeof(profiles[i].username) - 1] = '\0';
      history_print(fds[i].fd);
      write(fds[i].fd, "\n> ", 3);
      return 0;
    }

    broadcast_message(i, buffer, fds, profiles, *cnfds);
    char historyMessage[MAX_MSG_LEN];
    snprintf(historyMessage, sizeof(historyMessage), "[%s]: %s\n", profiles[i].username, buffer);
    history_save(historyMessage);
    return 0;

}

int main() {
    srand(time(NULL));

    int sock_fd = init_server();
    if (sock_fd < 0) {
        return 1;
    }

    struct pollfd fds[NFDS];
    ClientProfile profiles[NFDS];

    fds[0].fd = sock_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    nfds_t cnfds = 1;

    while (1) {
        int activity = poll(fds, cnfds, -1);
        if (activity < 0) {
            log_err("Poll error.");
            break;
        }

        if (fds[0].revents & POLLIN) {
            handle_new_connection(sock_fd, fds, profiles, &cnfds);
        }

        for (int i = 1; i < cnfds; i++) {
            if (fds[i].revents & POLLIN) {
                int disconnected = handle_client_data(i, fds, profiles, &cnfds);
                if (disconnected) {
                    i--;
                }
            }
        }
    }

    close(sock_fd);
    log_info("Closed main server socket.");

    return 0;
}
