#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void randomId(char *str, int length);

int is_all_whitespace(const char *str);

int tokenizer(char *input, char *output_tokens[], int max_tokens);

void sendToClient(int fd, const char *msg);

int getClientIdxByUsername(const char *username, ClientProfile *profiles);

int getRoomIdxByRoomname(const char *roomName);

#endif // UTILS_H
