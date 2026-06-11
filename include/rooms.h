#ifndef ROOMS_H
#define ROOMS_H

#include <stdbool.h>
#include "types.h"

#define DEFAULT_LOBBY_ID "00000"
#define MAX_ROOMS 16

typedef struct {
  char name[32];
  char password[32];
  bool isPublic;
  char roomId[6];
  char roomAdmin[6];
} Room;

typedef enum {
    ROOM_SUCCESS = 0,
    ROOM_ERR_FULL,
    ROOM_ERR_DUPLICATE,
    USER_ALREADY_IN_A_ROOM
} RoomStatus;

RoomStatus createRoom(const char *room_name, const char *password, bool is_public, int client_idx, ClientProfile *profiles);

extern Room rooms[MAX_ROOMS];

#endif // ROOMS_H
