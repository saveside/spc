#include "lib/logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BACKLOG 5

int main() {
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;                 
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (sock_fd < 0) {
    log_err("Failed to open a socket.");
    return 1;
  }
  log_info("Created a  socket.");
  
  if(bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    log_err("Failed to bind socket.");
    return 1;
  } 
  log_info("Bond socket.");
  
  if (listen(sock_fd, BACKLOG) < 0) {
    log_err("Failed to listen socket.");
    return 1;
  }
  log_info("Listening socket...");
  log_info("Waiting connections...");

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  
  while(1) {
    int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      log_err("Failed to accept client connection.");
      return 1;
    }
    log_info("Client connected with fd: %d.", client_fd);
    
    log_info("Waiting client message.");

    char buffer[1024];
    
    while(1) {
      memset(buffer, 0, sizeof(buffer));
    
      ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

      if (bytes_read < 0) {
        log_err("Failed to read message.");
      }
      else if (bytes_read == 0) {
        log_warn("Client disconnected without sending anything.");
        break;
      } else {
        printf("[CLIENT]: %s\n", buffer);
      }
      write(client_fd, buffer, bytes_read);
  }
    close(client_fd);
    log_info("Closed client connection.");
  }

  close(sock_fd);
  log_info("Closed main server socket. :pleadey:");

  return 0;
}
