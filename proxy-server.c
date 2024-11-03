#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CONNECTIONS 10
#define MAX_BYTES 1024

int main(int argc, char **argv) {
  int PORT;

  if (argc < 2) {
    fprintf(stderr, "Not enough parameters supplied\n");
    exit(EXIT_FAILURE);
  }

  PORT = strtoul(argv[1], NULL, 10);
  if (PORT == 0 || PORT == INT_MAX || PORT == INT_MIN) {
    perror("Error converting port number. Please supply a valid PORT.");
    exit(EXIT_FAILURE);
  }

  /**
   * AF_INET
   * IPv4 Internet protocols
   *
   * SOCK_STREAM
   * Provides sequenced, reliable, two-way, connection-based
   * byte streams. An out-of-band data transmission mechanism
   * may be supported.
   *
   * 0
   * System will automatically choose the appropriate protocol
   * based on the first two parameters.
   *
   * For SOCK_STREAM (TCP):
   * When protocol = 0, it automatically selects TCP (IPPROTO_TCP)
   * You could explicitly write: socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
   *
   * For SOCK_DGRAM (UDP):
   * When protocol = 0, it automatically selects UDP (IPPROTO_UDP)
   * You could explicitly write: socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)
   */

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    perror("Error in creating the main socket");
    exit(EXIT_FAILURE);
  }

  /**
   * SOL_SOCKET
   * The level at which the option is defined.
   * It is used to manipulate options at the socket API level.
   *
   * SO_REUSEADDR
   * Indicates that the rules used in validating addresses supplied
   * in a bind() call should allow reuse of local addresses.
   * For AF_INET sockets this means that a socket may bind, except
   * when there is an active listening socket bound to the address.
   *
   */

  if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) ==
      -1) {
    perror("Error in setting socket options");
    close(socketfd);
    exit(EXIT_FAILURE);
  }

  /**
   * struct sockaddr_in
   * structure to hold the address and port information
   * for the server.
   *
   * sin_family
   * It should always be set to AF_INET for IPv4.
   *
   * sin_port
   * The port number in network byte order.
   *
   * sin_addr.s_addr
   * The IP address in network byte order.
   *
   * INADDR_ANY
   * It is used when you don't need to bind a socket to a specific IP.
   *
   * htons()
   * Host to Network Short
   * It converts the port number from host byte order to network byte order.
   *
   */

  struct sockaddr_in server_address = {.sin_family = AF_INET,
                                       .sin_port = htons(PORT),
                                       .sin_addr.s_addr = INADDR_ANY};

  if (bind(socketfd, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    perror("Error in binding the socket");
    close(socketfd);
    exit(EXIT_FAILURE);
  }

  if (listen(socketfd, MAX_CONNECTIONS) == -1) {
    perror("Error in listening on the socket");
    close(socketfd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addresss;
  bzero(&client_addresss, sizeof(client_addresss));

  socklen_t client_addr_len = sizeof(client_addresss);

  int client_socket_id =
      accept(socketfd, (struct sockaddr *)&client_addresss, &client_addr_len);

  if (client_socket_id == -1) {
    perror("Error in accepting connection.");
    close(socketfd);
    exit(EXIT_FAILURE);
  }

  char msg_buffer[MAX_BYTES];
  memset(msg_buffer, 0, sizeof(msg_buffer));

  int bytes_recv = 0;

  while ((bytes_recv = recv(client_socket_id, msg_buffer,
                            sizeof(msg_buffer) - 1, 0)) > 0) {
    msg_buffer[bytes_recv] = '\0';
    printf("%s", msg_buffer);
  }

  close(client_socket_id);
  close(socketfd);
}
