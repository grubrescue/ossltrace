#define _GNU_SOURCE

#include "firewall_client.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../common/logger.h"

static int sockfd = -1;

void
init_firewall_client(const char *socket_path) {
  if (socket_path == NULL) {
    OSSLTRACE_LOG("init_firewall_client: socket path is null\n");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un addr;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    perror("connect");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  OSSLTRACE_LOG("*** child connected to %s\n", socket_path);
}

int
firewall_match_in_buf(const void *buf, int buf_len) {
  assert(sockfd > 0 && "firewall client not initialized");

  static char denylist_buf[FIREWALL_DENY_BUF_SIZE];

  unsigned short command = htons(2);
  if (send(sockfd, &command, sizeof(command), 0) == -1) {
    perror("send");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  ssize_t received = recv(sockfd, denylist_buf, FIREWALL_DENY_BUF_SIZE, 0);
  if (received == -1) {
    perror("recv");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  char *line = strtok(denylist_buf, "\n");
  while (line != NULL) {
    char *result = memmem(buf, buf_len, line, strlen(line));
    if (result != NULL) {
      OSSLTRACE_LOG("! found \"%s\" in buf, packet refused\n", line);
      return 1;
    }
    line = strtok(NULL, "\n");
  }
  return 0;
}
