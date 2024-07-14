#pragma once

#include "../common/envvars.h"
#include "logger.h"
#include "firewall.h"
#include "payloads.h"
// #include "server.h"

#include <stdlib.h>

static void 
ossl_initialize() {
    init_log();
    init_firewall();   
    init_ca_ignore_mode();
    // if (getenv(OSSLTRACE_ENABLE_PARASITE_SERVER_ENV_VAR)) {
    //     run_parasite_server();
    // }
}
