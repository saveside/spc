#include "commands.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>

void cmd_help(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_nick(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_quit(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

Command command_list[] = {
    { "help", cmd_help, "Displays this help menu with all available commands." },
    { "nick", cmd_nick, "Changes your display name. Usage: /nick <new_name>" },
    { "quit", cmd_quit, "Safely disconnects you from the chat server." }
};

bool check_cmd(const char *target) {
  bool found = false;
  int size = sizeof(command_list) / sizeof(command_list[0]);

  for (int i = 0; i < size; i++) {
    if (strcmp(command_list[i].cmd_name, target) == 0) {
      found = true;
      break;
    }
  }
  return found;
}

void run_cmd(const char *target, int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  int size = sizeof(command_list) / sizeof(command_list[0]);

  for (int i = 0; i < size; i++) {
    if (strcmp(command_list[i].cmd_name, target) == 0) {
      command_list[i].handler(client_idx, arg, fds, profiles, cnfds);
      return;
    }
  }
}

void cmd_help(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  int client_fd = fds[client_idx].fd;
  char help_buffer[2048];
  int offset = 0;

  offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset, 
      "\n============ AVAILABLE COMMANDS ============\n");

  int size = sizeof(command_list) / sizeof(command_list[0]);
  for (int i = 0; i < size; i++) {
    offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset,
                       "  /%-8s - %s\n", 
                       command_list[i].cmd_name, 
                       command_list[i].description);
  }

  offset += snprintf(help_buffer + offset, sizeof(help_buffer) - offset,
                     "============================================\n>");

  write(client_fd, help_buffer, strlen(help_buffer));
}

void cmd_nick(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  int client_fd = fds[client_idx].fd;
  
  if (arg == NULL || strlen(arg) == 0) {
    char *err_msg = "Usage: /nick <new_nickname>\r\n> ";
    write(client_fd, err_msg, strlen(err_msg));
    return;
  }

  strncpy(profiles[client_idx].username, arg, sizeof(profiles[client_idx].username) - 1);
  profiles[client_idx].username[sizeof(profiles[client_idx].username) - 1] = '\0';
}

void cmd_quit(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  int client_fd = fds[client_idx].fd;
  char *quit_msg = "Goodbye!";

  write(client_fd, quit_msg, strlen(quit_msg));
  shutdown(client_fd, SHUT_RDWR);
  close(client_fd);

  log_warn("The user named %s with fd: %d, left.", profiles[client_idx].username, client_fd);

  fds[client_idx] = fds[*cnfds - 1];
  profiles[client_idx] = profiles[*cnfds - 1];
  (*cnfds)--;
}
