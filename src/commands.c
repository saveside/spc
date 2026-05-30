#include "commands.h"
#include <string.h>
//#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>

void cmd_help(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  printf("[Test] cmd_help triggered by client %d with arg: %s\n", client_idx, arg ? arg : "NULL");
}
void cmd_nick(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  printf("[Test] cmd_nick triggered by client %d with arg: %s\n", client_idx, arg ? arg : "NULL");
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

Command command_list[] = {
  { "help", cmd_help },
  { "nick", cmd_nick },
  { "quit", cmd_quit }
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
