#ifndef HISTORY_H
#define HISTORY_H

#define MAX_MSG_LEN 1024
#define MAX_HISTORY 50

void history_save(const char *msg);
void history_print(int client_fd);

#endif
