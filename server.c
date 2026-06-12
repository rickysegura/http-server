// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void parse_request(char *buffer, char *method, char *path) {
  sscanf(buffer, "%s %s", method, path);
}

void serve_file(int client_fd, const char *filepath) {
  FILE *f = fopen(filepath, "r");

  if (!f) {
    char *err = "HTTP/1.1 404 Not Found\r\n\r\n<h1>Not Found</h1>";
    send(client_fd, err, strlen(err), 0);
    return;
  }

  // get file size
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  char *content = malloc(size + 1);
  if (!content) {
    fclose(f);
    return;
  }

  fread(content, 1, size, f);
  content[size] = '\0';
  fclose(f);

  char header[256];
  snprintf(header, sizeof(header),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %ld\r\n"
           "Connection: close\r\n"
           "\r\n", size);

  send(client_fd, header, strlen(header), 0);
  send(client_fd, content, size, 0);
  free(content);
}

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  free(arg);

  char buffer[BUFFER_SIZE] = {0};
  read(client_fd, buffer, BUFFER_SIZE);

  char method[8], path[256];
  parse_request(buffer, method, path);
  printf("--- Request ---\n%s\n", buffer);

  if (strcmp(path, "/") == 0) {
    serve_file(client_fd, "public/index.html");
  } else if (strcmp(path, "/about") == 0) {
    serve_file(client_fd, "public/about.html");
  } else {
    // 404
    char *not_found = 
      "HTTP/1.1 404 Not Found\r\n"
      "Content-Type: text/html\r\n"
      "Connection: close\r\n"
      "\r\n"
      "<h1>Not Found</h1>";

    send(client_fd, not_found, strlen(not_found), 0);
  }

  close(client_fd);
  return NULL;
}

int main() {
  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

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
    int *client_fd = malloc(sizeof(int));
    *client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    pthread_t tid;
    pthread_create(&tid, NULL, handle_client, client_fd);
    pthread_detach(tid);
  }

  return 0;
}
