/* vim: set expandtab ts=2: */
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "kvl.h"

extern int input_pos;
extern int input_len;
extern char *input_string;
struct key_value *kvl_list;

#define COUNT_OF(arr) (sizeof(arr)/sizeof(arr[0]))

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
#ifdef DEBUG
  char buff[512] = {'\0', };
#endif

  kvl_list = NULL;

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

  char b;
  int in_header = 1;
  int rn_status = 0;
  char response[512] = {'\0',};
  size_t response_size = 0;
  while (read(http_socket, &b, 1) &&
         (response_size < (COUNT_OF(response) - 1))) {
    if (b == '\r' || b == '\n') {
      rn_status++;
      if (rn_status==4) {
        in_header = 0;
      }
    } else {
      rn_status = 0;
    }
    if (!in_header) {
      response[response_size] = b;
      response_size++;
    }
  }

  input_string = response;
  input_len = strlen(input_string);
  yyparse();

#ifdef DEBUG
  struct key_value *kvl_iterator = kvl_list;
  for (; kvl_iterator; kvl_iterator = kvl_iterator->next) {
    printf("%s:%s\n", kvl_iterator->key, kvl_iterator->value);
  }
#endif

error:
  close(http_socket);
  free(get);
  return success;
}

int main() {
  mlab_ns("ndt", NULL, NULL);
}
