#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "../cutils.h"

int main() {
  log_info("test");
  log_debug("test");
  log_warn("test");
  log_error("test");
  log_fatal("test");
  return 0;
}
