#define PROXYBOUND_CONF_FILE_ENV_VAR "PROXYBOUND_CONF_FILE"
#define PROXYBOUND_QUIET_MODE_ENV_VAR "PROXYBOUND_QUIET_MODE"
#define PROXYBOUND_CONF_FILE "proxybound.conf"
#define LOG_PREFIX "[Proxybound] "
#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif

#include <stddef.h>

char *get_config_path(char* default_path, char* pbuf, size_t bufsize);

//RcB: DEP "common.c"