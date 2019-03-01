#ifndef COMMON_H
#define COMMON_H

#define PROXYBOUND_CONF_FILE_ENV_VAR "PROXYBOUND_CONF_FILE"
#define PROXYBOUND_QUIET_MODE_ENV_VAR "PROXYBOUND_QUIET_MODE"
#define PROXYBOUND_CONF_FILE "proxybound.conf"
#define LOG_PREFIX "[proxybound] "
#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif

#include <stddef.h>

char *get_config_path(char* default_path, char* pbuf, size_t bufsize);
void pc_stringfromipv4(unsigned char *ip_buf_4_bytes, char *outbuf_16_bytes);

//RcB: DEP "common.c"
#endif
