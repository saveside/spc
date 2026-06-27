#include "rooms.h"
#include "logger.h"
#include "utils.h"
#include <string.h>

Room rooms[MAX_ROOMS] = {
  [0] = { .name = "Lobby", .password = "", .isPublic = true, .roomId = "00000" }
};

static int room_count = 1;

RoomStatus createRoom(const char *room_name, const char *password, bool is_public, int client_idx, ClientProfile *profiles) {
  if (room_count >= MAX_ROOMS) {
    return ROOM_ERR_FULL;
  }

  for (int i = 0; i < room_count; i++) {
    if (strcmp(rooms[i].name, room_name) == 0) {
      return ROOM_ERR_DUPLICATE;
    }
  }

  if (strcmp(profiles[client_idx].currentRoom, "Lobby") != 0) {
    return USER_ALREADY_IN_A_ROOM;
  }

  strncpy(rooms[room_count].name, room_name, sizeof(rooms[room_count].name) - 1);
  rooms[room_count].name[sizeof(rooms[room_count].name) - 1] = '\0';

  rooms[room_count].isPublic = is_public;

  if (!is_public && password != NULL) {
    strncpy(rooms[room_count].password, password, sizeof(rooms[room_count].password) - 1);
    rooms[room_count].password[sizeof(rooms[room_count].password) - 1] = '\0';
  } else {
    rooms[room_count].password[0] = '\0';
  }

  strncpy(rooms[room_count].roomAdmin, profiles[client_idx].id, sizeof(rooms[room_count].roomAdmin) - 1);
  rooms[room_count].roomAdmin[sizeof(rooms[room_count].roomAdmin) - 1] = '\0';

  randomId(rooms[room_count].roomId, 6);

  strncpy(profiles[client_idx].currentRoom, rooms[room_count].name, sizeof(profiles[client_idx].currentRoom) - 1);
  profiles[client_idx].currentRoom[sizeof(profiles[client_idx].currentRoom) - 1] = '\0';

  log_info("A %s room created by %s: %s", is_public ? "Public" : "Private", profiles[client_idx].username, rooms[room_count].name);

  room_count++;

  return ROOM_SUCCESS;
}

bool isRoomAdmin(const Room *room, int client_idx, ClientProfile *profiles) {
    if (strcmp(room->roomAdmin, profiles[client_idx].id) == 0) {
        return true;
    }
    return false;
}

void kickUserFromRoom(int idx, ClientProfile *profiles) {
    strncpy(profiles[idx].currentRoom, "Lobby", sizeof(profiles[idx].currentRoom) - 1);
    profiles[idx].currentRoom[sizeof(profiles[idx].currentRoom) - 1] = '\0';
}

void toggleMuteUser(int idx, ClientProfile *profiles) {
    profiles[idx].isMuted = !profiles[idx].isMuted;
}
