/* vim: set expandtab ts=2: */
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "json_object.h"

extern int input_pos;
extern int input_len;
extern char *input_string;
extern struct json_object *root;
int json_parse_success;

#define DEBUG 1
/*
 *
 */
int mlab_ns(char *service, char *mlabns_server, struct sockaddr_in *service_ip) {

  struct addrinfo *addrs = NULL, *addrs_iter = NULL, addrs_hints;
  int gai_error = 0;
  struct sockaddr *server_addr = NULL;
  int no_dns_results = 1;
  char *get_linebreak = "\r\n";
  char *get = NULL;
  int get_len = 0;
  int success = 1;
  int http_socket = -1;
  char end_of_header[4] = {'\r', '\n', '\r', '\n',};
  int end_of_header_ctr = 0, end_of_header_len = 4;
  char response[513] = {'\0',};
  int response_len_ctr = 0, response_len_max = 42; //512;
  char b;
  int in_header = 1;
#ifdef DEBUG
  char buff[512] = {'\0', };
#endif

  if (!mlabns_server) {
    mlabns_server = "mlab-ns.appspot.com";
  }

  /*
   * do server resolution.
   */
  memset(&addrs_hints, 0, sizeof(addrs_hints));
  addrs_hints.ai_family = AF_INET;
  addrs_hints.ai_socktype = SOCK_STREAM;
  addrs_hints.ai_flags = AI_NUMERICSERV;
  if ((gai_error = getaddrinfo(mlabns_server, "80", &addrs_hints, &addrs))) {
    printf("getaddrinfo error: %s\n", gai_strerror(gai_error));
    return -1;
  }

  server_addr = (struct sockaddr*)malloc(sizeof(struct sockaddr));
  addrs_iter = addrs;
  while (addrs_iter) {
    memcpy(server_addr, addrs_iter->ai_addr, sizeof(struct sockaddr));
    addrs_iter = addrs_iter->ai_next;

    no_dns_results = 0;

#ifdef DEBUG
    printf("ip: %s\n", inet_ntop(AF_INET, 
                               &((struct sockaddr_in*)server_addr)->sin_addr, 
                                buff, 
                                512)
          );
#endif
  }
  freeaddrinfo(addrs);

  if (no_dns_results) {
    return -2;
  }
  get_len = strlen("GET ") +
            strlen("/") + strlen(service) +
            strlen(" HTTP/1.1") + strlen(get_linebreak) + 
            strlen("Host: ") + strlen(mlabns_server) + strlen(get_linebreak) +
            strlen("Connection: close") + strlen(get_linebreak) +
            strlen(get_linebreak);
  get = (char*)malloc(get_len + 1);
  memset(get, 0, get_len+1);

  strcpy(get, "GET /");
  strcat(get, service);
  strcat(get, " HTTP/1.1");
  strcat(get, get_linebreak);

  strcat(get, "Host: ");
  strcat(get, mlabns_server);
  strcat(get, get_linebreak);

  strcat(get, "Connection: close");
  strcat(get, get_linebreak);
  strcat(get, get_linebreak);

  printf("get: %s\n", get);

  if ((http_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    success = -3;
    goto error;
  }
  if (connect(http_socket, server_addr, sizeof(struct sockaddr_in))) {
    success = -4;
    goto error;
  }

  write(http_socket, get, get_len);

  while (read(http_socket, &b, 1)) {
    if (in_header) {
      if (end_of_header[end_of_header_ctr++] != b) {
        end_of_header_ctr = 0;
      }
      if (end_of_header_ctr>=end_of_header_len) {
        in_header = 0;
      }
    } else {
      if (response_len_ctr>=response_len_max)
        break;
      response[response_len_ctr] = b;
      response_len_ctr++;
    }
  }

#ifdef DEBUG
  printf("response: -%s-\n", response);
#endif

  json_parse_success = 1;
  input_string = response;
  input_len = strlen(input_string);
  yyparse();

  if (!json_parse_success) {
    printf("failed to parse.\n");
    goto error;
  }

#ifdef DEBUG
  if (json_parse_success) print_json_object(root);
#endif

error:
  close(http_socket);
  free(get);
  return success;
}

int main() {
  mlab_ns("ndt", NULL, NULL);
}
