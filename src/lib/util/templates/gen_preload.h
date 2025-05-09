#pragma once

#include <dlfcn.h>
#include <unistd.h>

#include "concat.h"
#include "gen_payload.h"

#define PRELOAD_PAYLOAD_BODY(ORIG_FUN, ...)              \
  if (GET_ORIGINAL(ORIG_FUN) == NULL) {                  \
    SET_ORIGINAL(ORIG_FUN, dlsym(RTLD_NEXT, #ORIG_FUN)); \
  }                                                      \
  return INVOKE_PAYLOAD(ORIG_FUN, __VA_ARGS__);

#define PRELOAD_CONSTRUCTOR(CONSTRUCTOR_FUNC, ...) \
  __attribute__((constructor)) void                \
  CAT2(CONSTRUCTOR_FUNC, constructor)() {          \
    CONSTRUCTOR_FUNC(__VA_ARGS__);                 \
  }
