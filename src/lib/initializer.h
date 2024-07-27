#pragma once

#include "../common/envvars.h"
#include "logger.h"
#include "firewall.h"
#include "payloads.h"
#include "server.h"

#include <stdlib.h>
#include <unistd.h>

static void 
ossl_initialize() {
    init_log();
    OSSLTRACE_LOG("running subprocess with pid %d...\n", getpid());
    init_firewall();   
    init_ca_ignore_mode();
    if (getenv(OSSLTRACE_ENABLE_CONF_SERVER_ENV_VAR) != NULL) {
        run_parasite_server();
    }
}
