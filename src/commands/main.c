#include "commands.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_help(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_nick(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_quit(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_room(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_kick(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_mute(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_unmute(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_users(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

Command command_list[] = {
    { "help", cmd_help, "Displays this help menu with all available commands." },
    { "nick", cmd_nick, "Changes your display name. Usage: /nick <new_name>" },
    { "quit", cmd_quit, "Safely disconnects you from the chat server." },
    { "room", cmd_room, "Creates a Room with specified arguments." },
    { "kick", cmd_kick, "Kicks user from current room. Requires Room Admin Permission!" },
    { "mute", cmd_mute, "Mute a user. Requires Room Admin Permission!" },
    { "unmute", cmd_unmute, "Unmute a user. Requires Room Admin Permission!" },
    { "users", cmd_users, "Displays all users in the current room." }
};
const int command_count = sizeof(command_list) / sizeof(command_list[0]);

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

void run_cmd(const char *target, int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int size = sizeof(command_list) / sizeof(command_list[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(command_list[i].cmd_name, target) == 0) {
            command_list[i].handler(client_idx, tokens, token_count, fds, profiles, cnfds);
            return;
        }
    }
}
