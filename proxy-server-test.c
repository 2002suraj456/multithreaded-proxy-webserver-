#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int socket_fd;
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "Error creating a socket.");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_address = {.sin_family = AF_INET,
                                       .sin_port = htons(PORT)};

  if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) == -1) {
    fprintf(stderr, "Error converting the address.");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  if (connect(socket_fd, (struct sockaddr *)&server_address,
              sizeof(server_address)) == -1) {
    fprintf(stderr, "Error connecting to the server.");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  const char *msg = "hello world!!!";
  if (send(socket_fd, msg, strlen(msg), 0) == -1) {
    fprintf(stderr, "Error converting the address.");
    close(socket_fd);
    exit(EXIT_FAILURE);
  }

  close(socket_fd);
}
