#include "commands.h"
#include "rooms.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_help(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_nick(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_quit(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);
void cmd_room(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds);

static void handle_room_create(int client_fd, int client_idx, char *tokens[], int token_count, ClientProfile *profiles);
static void handle_room_list(int client_fd);
static void handle_room_quit(int client_fd, int client_idx, ClientProfile *profiles);
static void handle_room_join(int client_fd, int client_idx, char *tokens[], int token_count, ClientProfile *profiles);

Command command_list[] = {
    { "help", cmd_help, "Displays this help menu with all available commands." },
    { "nick", cmd_nick, "Changes your display name. Usage: /nick <new_name>" },
    { "quit", cmd_quit, "Safely disconnects you from the chat server." },
    { "room", cmd_room, "Creates a Room with specified arguments." }
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

void run_cmd(const char *target, int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int size = sizeof(command_list) / sizeof(command_list[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(command_list[i].cmd_name, target) == 0) {
            command_list[i].handler(client_idx, tokens, token_count, fds, profiles, cnfds);
            return;
        }
    }
}

void cmd_help(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
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

    sendToClient(client_fd, help_buffer);
}

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

void cmd_room(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int client_fd = fds[client_idx].fd;
    
    if (token_count < 2 || tokens[1] == NULL) {
        sendToClient(client_fd, "Usage: /room <create|list|join|quit> [args]\n> ");
        return;
    }
    
    char *action = tokens[1];
    
    if (strcmp(action, "create") == 0) {
        handle_room_create(client_fd, client_idx, tokens, token_count, profiles);
    } else if (strcmp(action, "list") == 0) {
        handle_room_list(client_fd);
    } else if (strcmp(action, "quit") == 0) {
        handle_room_quit(client_fd, client_idx, profiles);
        return; 
    } else if (strcmp(action, "join") == 0) {
        handle_room_join(client_fd, client_idx, tokens, token_count, profiles);
        return; 
    }
    
    sendToClient(client_fd, "> ");
}

static void handle_room_create(int client_fd, int client_idx, char *tokens[], int token_count, ClientProfile *profiles) {
    if (token_count < 3) { 
        sendToClient(client_fd, "Usage: /room create <name> [password]\n");
        return;
    }
    
    bool is_public = (token_count < 4);
    RoomStatus status = createRoom(tokens[2], tokens[3], is_public, client_idx, profiles);
    
    switch(status) {
        case ROOM_SUCCESS:
            sendToClient(client_fd, "Room created successfully!\n");
            break;
        case ROOM_ERR_FULL:
            sendToClient(client_fd, "Error: Server room limit reached.\n");
            break;
        case ROOM_ERR_DUPLICATE:
            sendToClient(client_fd, "Error: A room with that name already exists.\n");
            break;
        case USER_ALREADY_IN_A_ROOM:
            sendToClient(client_fd, "You are already in a room!\n");
            break;
    }
}

static void handle_room_list(int client_fd) {
    char servers_buffer[2048];
    int offset = 0;

    offset += snprintf(servers_buffer + offset, sizeof(servers_buffer) - offset, 
                       "\n============ AVAILABLE SERVERS ============\n");
    
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i].isPublic) {
            offset += snprintf(servers_buffer + offset, sizeof(servers_buffer) - offset,
                               "  %-8s - %s\n", 
                               rooms[i].name, 
                               rooms[i].roomAdmin);
        }
    }

    offset += snprintf(servers_buffer + offset, sizeof(servers_buffer) - offset,
                       "============================================\n");
    
    sendToClient(client_fd, servers_buffer);
}

static void handle_room_quit(int client_fd, int client_idx, ClientProfile *profiles) {
    if (strcmp(profiles[client_idx].currentRoom, "Lobby") == 0) {
        sendToClient(client_fd, "Error: You are not in a room!\n");
        sendToClient(client_fd, "> ");
        return;
    }
    strncpy(profiles[client_idx].currentRoom, "Lobby", sizeof(profiles[client_idx].currentRoom) - 1);
    profiles[client_idx].currentRoom[sizeof(profiles[client_idx].currentRoom) - 1] = '\0';
    sendToClient(client_fd, "> ");
}

static void handle_room_join(int client_fd, int client_idx, char *tokens[], int token_count, ClientProfile *profiles) {
    if (token_count < 3 || tokens[2] == NULL) {
        sendToClient(client_fd, "Usage: /room join <room_name> [password]\n> ");
        return;
    }
    
    int room_found = 0;
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (strcmp(rooms[i].name, tokens[2]) == 0) {
            room_found = 1;
            if (!rooms[i].isPublic && rooms[i].password[0] != '\0') {
                if (token_count < 4) {
                    sendToClient(client_fd, "Usage: /room join <name> <password>\n");
                    sendToClient(client_fd, "> ");
                    return;
                }
                
                if (strcmp(tokens[3], rooms[i].password) != 0) {
                    sendToClient(client_fd, "Authentication failure! Please check room credentials again.\n");
                    sendToClient(client_fd, "> ");
                    return;
                }
            }
    
            strncpy(profiles[client_idx].currentRoom, rooms[i].name, sizeof(profiles[client_idx].currentRoom) - 1);
            profiles[client_idx].currentRoom[sizeof(profiles[client_idx].currentRoom) - 1] = '\0';
            
            sendToClient(client_fd, "Successfully joined the room.\n");
            sendToClient(client_fd, "> ");
            return;
        }
    }
    
    if (!room_found) {
        sendToClient(client_fd, "Error: Room not found.\n");
    }
    sendToClient(client_fd, "> ");
}
