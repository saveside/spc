#include "rooms.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/socket.h>
#include <logger.h>
#include <utils.h>

void cmd_kick(int client_idx, char *tokens[], int token_count, struct pollfd *fds, ClientProfile *profiles, nfds_t *cnfds) {
    int kickedIdx = getClientIdxByUsername(tokens[1], profiles);
    int roomIdx = getRoomIdxByRoomname(profiles[client_idx].currentRoom);

    if (strcmp(profiles[kickedIdx].currentRoom, "Lobby") == 0) return;
    if (strcmp(rooms[roomIdx].roomAdmin, profiles[client_idx].id) != 0) {
        sendToClient(fds[client_idx].fd, "You are not allowed to use this command!");
        return;
    }

    kickUserFromRoom(kickedIdx, profiles);
    profiles[kickedIdx].isMuted = false;
    sendToClient(fds[kickedIdx].fd, "You've been kicked from room!\n");
    sendToClient(fds[kickedIdx].fd, "> ");
}
