#include "utils.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"

void randomId(char *str, int length) {
    char charset[] = "0123456789"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                     "abcdefghijklmnopqrstuvwxyz";
                     
    int charset_size = sizeof(charset) - 1;

    for (int i = 0; i < length; i++) {
        int key = rand() % charset_size;
        str[i] = charset[key];
    }
    
    str[length] = '\0';
}

int is_all_whitespace(const char *str) {
    if (*str == '\0') return 1;
    return 1;
}

int tokenizer(char *input, char *output_tokens[], int max_tokens) {
  int count = 0;

  char *token = strtok(input, " ");

  while (token != NULL && count < max_tokens) {
    output_tokens[count] = token;
    count++;
    token = strtok(NULL, " ");
  }

  return count;
}

void sendToClient(int fd, const char *msg) {
    write(fd, msg, strlen(msg));
}
