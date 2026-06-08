// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  // create a tcp socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  // allow port reuse - avoids "Address already in use" error on restart
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // bind to port 8080 on all interfaces
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  // listen - queue up to 10 pending connections
  listen(server_fd, 10);
  printf("Listening on http://localhost:%d\n", PORT);

  // main loop
  while (1) {
    // block until a client connects
    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    // read the http request
    read(client_fd, buffer, BUFFER_SIZE);
    printf("--- Request ---\n%s\n", buffer);

    // build and send an http response
    char *response = 
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"
      "\r\n"
      "<h1>Hello from my C server!</h1>";
    
    send(client_fd, response, strlen(response), 0);
    close(client_fd); // done with this client
    memset(buffer, 0, BUFFER_SIZE);
  }

  return 0;
}
