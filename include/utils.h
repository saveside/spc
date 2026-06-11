#ifndef UTILS_H
#define UTILS_H

void randomId(char *str, int length);

int is_all_whitespace(const char *str);

int tokenizer(char *input, char *output_tokens[], int max_tokens);

void sendToClient(int fd, const char *msg);

#endif // UTILS_H
