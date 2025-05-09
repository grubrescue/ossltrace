#pragma once

#include "../common/logger.h"

#define FIREWALL_DENY_BUF_SIZE 4096

void
init_firewall_client(const char *socket_path);

int
firewall_match_in_buf(const void *buf, int buf_len);
