#pragma once

#include "logger.h"
#include "firewall.h"
#include "payloads.h"


static void 
ossl_initialize() {
    init_log();
    init_firewall();   
    init_ca_ignore_mode();
}
