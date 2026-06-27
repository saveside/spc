#include "rooms.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_unmute(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int mutedUser = getClientIdxByUsername(tokens[1], profiles);
    int roomIdx = getRoomIdxByRoomname(profiles[client_idx].currentRoom);
    char adminMsg[128];
    char mutedMsg[128];

    if (strcmp(profiles[mutedUser].currentRoom, "Lobby") == 0) return;
    if (strcmp(rooms[roomIdx].roomAdmin, profiles[client_idx].id) != 0) {
        sendToClient(fds[client_idx].fd, "You are not allowed to use this command!");
        return;
    }

    profiles[mutedUser].isMuted = false;

    snprintf(adminMsg, sizeof(adminMsg), "System: You have unmuted %s.\n", profiles[mutedUser].username);
    snprintf(mutedMsg, sizeof(mutedMsg), "System: You have been unmuted by the room admin (%s).\n", profiles[client_idx].username);

    sendToClient(fds[mutedUser].fd, mutedMsg);
    sendToClient(fds[client_idx].fd, adminMsg);
}
