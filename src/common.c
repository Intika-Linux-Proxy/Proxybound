#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static int check_path(char *path) {
	if(!path)
		return 0;
	return access(path, R_OK) != -1;
}

char *get_config_path(char* default_path, char* pbuf, size_t bufsize) {
	char buf[512];
	// top priority: user defined path
	char *path = default_path;
	if(check_path(path))
		goto have;
    
	if (!pbuf) {
        perror("couldnt find configuration file");
        exit(1);
    }
	
	// priority 1: env var PROXYBOUND_CONF_FILE
	path = getenv(PROXYBOUND_CONF_FILE_ENV_VAR);
	if(check_path(path))
		goto have;

	// priority 2; proxybound conf in actual dir
	path = getcwd(buf, sizeof(buf));
	snprintf(pbuf, bufsize, "%s/%s", path, PROXYBOUND_CONF_FILE);
	path = pbuf;
	if(check_path(path))
		goto have;

	// priority 3; $HOME/.proxybound/proxybound.conf
	path = getenv("HOME");
	snprintf(pbuf, bufsize, "%s/.proxybound/%s", path, PROXYBOUND_CONF_FILE);
	path = pbuf;
	if(check_path(path))
		goto have;

	// priority 4: $SYSCONFDIR/proxybound.conf
	path = SYSCONFDIR "/" PROXYBOUND_CONF_FILE;
	if(check_path(path))
		goto have;

	// priority 5: /etc/proxybound.conf
	path = "/etc/" PROXYBOUND_CONF_FILE;
	if(check_path(path))
		goto have;
	
	perror("couldnt find configuration file");
	exit(1);
	
	return NULL;
	have:
	return path;
}
