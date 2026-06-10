// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void parse_request(char *buffer, char *method, char *path) {
  sscanf(buffer, "%s %s", method, path);
}

int main() {
  int server_fd, client_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};
  char method[8], path[256];

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

    // parse the request
    parse_request(buffer, method, path);
    printf("--- Method and Path ---\n%s\n%s\n", method, path);

    char *body;
    if (strcmp(path, "/") == 0) {
      body = "<h1>Home Page</h1>";
    } else if (strcmp(path, "/about") == 0) {
      body = "<h1>About Page</h1>";
    } else {
      // 404
      char *not_found = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<h1>404 - Not Found</h1>";
      
      send(client_fd, not_found, strlen(not_found), 0);
      close(client_fd);
      continue;
    }

    // build a 200 response with the body
    char response[1024];
    snprintf(response, sizeof(response), 
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             strlen(body), body);
    
    send(client_fd, response, strlen(response), 0);
    close(client_fd);

    memset(buffer, 0, BUFFER_SIZE);
  }

  return 0;
}
