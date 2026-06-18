#include "history.h"
#include "utils.h"
#include <string.h>
#include <sys/socket.h>

typedef struct {
  char messages[MAX_HISTORY][MAX_MSG_LEN];
  int head;
  int tail;
} ChatHistory; // an circular buffer

static ChatHistory history = { .head = 0, .tail = 0 }; 

void history_save(const char *msg) {
  int next_tail = (history.tail + 1) % MAX_HISTORY;
  
  if (next_tail == history.head) {
    history.head = (history.head + 1) % MAX_HISTORY;
  }
  
  strncpy(history.messages[history.tail], msg, MAX_MSG_LEN - 1);
  history.messages[history.tail][MAX_MSG_LEN - 1] = '\0';
  
  history.tail = next_tail;
}

void history_print(int client_fd) {
  if (history.head == history.tail) {
    return;
  }

  int current = history.head;

  const char *header = "\n==================== HISTORY =================\n";
  sendToClient(client_fd, header);

  while (current != history.tail) {
    sendToClient(client_fd, history.messages[current]);
    current = (current + 1) % MAX_HISTORY;
  }

  const char *footer = "================= HISTORY END ===============\n\n";
  sendToClient(client_fd, footer);
}
