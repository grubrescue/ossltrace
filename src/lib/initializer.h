#pragma once

#include <stdlib.h>
#include <unistd.h>

#include "../common/envvars.h"
#include "../common/logger.h"
#include "payloads.h"

static void
ossl_initialize() {
  init_log(getenv(OSSLTRACE_LOG_OUTPUT_FILE_ENV_VAR));
  init_ca_ignore_mode(getenv(OSSLTRACE_IGNORE_CA_ENV_VAR));
  init_firewall_client(getenv(OSSLTRACE_SOCKET_PATH_ENV_VAR));
}
