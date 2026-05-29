#include "../include/commands.h"
#include <string.h>
//#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

void cmd_help(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  printf("[Test] cmd_help triggered by client %d with arg: %s\n", client_idx, arg ? arg : "NULL");
}
void cmd_nick(int client_idx, char *arg, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
  printf("[Test] cmd_nick triggered by client %d with arg: %s\n", client_idx, arg ? arg : "NULL");
}

Command command_list[] = {
  { "help", cmd_help },
  { "nick", cmd_nick }
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
