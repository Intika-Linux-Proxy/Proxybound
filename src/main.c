/***************************************************************************
 *                                                                         *
 *   (C) 2019 intika                                                       *
 *   (C) 2011, 2012 rofl0r                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "common.h"


static char appVersion[5] = "5.40\n";

static const char *dll_name = DLL_NAME;
static pid_t child_pid = -1 ;
static char own_dir[256];
static const char *dll_dirs[] = {
	//".",
	own_dir,
	LIB_DIR,
	"/lib",
	"/usr/lib",
	"/usr/local/lib",
	"/lib64",
	NULL
};

static int usage(char **argv) {
    printf("\nProxybound version %s", appVersion);
    printf("https://github.com/Intika-Linux-Proxy/Proxybound\n");
    printf("\nUsage:\n");
	printf("%s -q -f config_file command-or-app arguments\n", argv[0]);
    printf("\nOptions:\n");
	printf("-q \t makes proxybound quiet, this overrides the config setting\n");
    printf("-f \t allows to manually specify a configfile to use\n");
    printf("-v \t or --version, disaplay application version\n");
    printf("\nExample:\n");
    printf("proxybound telnet somehost.com\n");
    printf("\nAvailable environment variables:\n");
    printf("- PROXYBOUND_CONF_FILE:         Path to config file (default ./proxybound.conf then /etc/proxybound.conf)\n");
    printf("- PROXYBOUND_QUIET_MODE:        Quiet mode (1 or 0, default 0)\n");
    printf("- PROXYBOUND_SOCKS5_HOST:       Specify unique socks 5 proxy to use (default not used)\n");
    printf("- PROXYBOUND_SOCKS5_PORT:       Socks 5 port (default not used)\n");
    printf("- PROXYBOUND_FORCE_DNS:         Force dns resolv requests through (1 or 0, default 1)\n");
    printf("- PROXYBOUND_ALLOW_DNS:         Allow direct dns, allow udp port 53 and 853 (1 or 0, default 0)\n");
    printf("- PROXYBOUND_ALLOW_LEAKS:       Allow/Block unproxyfied protocols 'UDP/ICMP/RAW', blocked by default (1 or 0, default 0)\n");
    printf("- PROXYBOUND_WORKING_INDICATOR: Create '/tmp/proxybound.tmp' when dll is working as intended (1 or 0, default 0)\n");  
    printf("\nMore help:\n");
    printf("More help is available in README.md file https://github.com/Intika-Linux-Proxy/Proxybound\n\n");
	return EXIT_FAILURE;
}

static int version(char **argv) {
    printf("\nProxybound version %s", appVersion);
    printf("https://github.com/Intika-Linux-Proxy/Proxybound\n\n");
	return EXIT_FAILURE;
}

static void set_own_dir(const char *argv0) {
	size_t l = strlen(argv0);
	while(l && argv0[l - 1] != '/')
		l--;
	if(l == 0)
		//memcpy(own_dir, ".", 2);
        memcpy(own_dir, "/dev/null/", 2);
	else {
		memcpy(own_dir, argv0, l - 1);
		own_dir[l] = 0;
	}
}

static int file_exist(const char * filename){
    if (access( filename, F_OK ) != -1 ) {
        return 1;
    } else {
        return 0;
    }
}

static void wait_timer() {
    //Wait 2 sec     
    int msec = 0, trigger = 2000; /* 10ms */
    clock_t before = clock();

    do {
        clock_t difference = clock() - before;
        msec = difference * 1000 / CLOCKS_PER_SEC;
        //iterations++;
    } while ( msec < trigger );
    //printf("Time taken %d seconds %d milliseconds (%d iterations)\n", msec/1000, msec%1000, iterations);
}

static int check_injection() {
    if (file_exist("/tmp/proxybound.tmp")) {
        //File already exist we can not do a lot
        remove("/tmp/proxybound.tmp");
        fprintf(stderr, LOG_PREFIX "injected dll loaded\n");
        return 1;
    } else {
        wait_timer();
        if (file_exist("/tmp/proxybound.tmp")) {
            remove("/tmp/proxybound.tmp");
            fprintf(stderr, LOG_PREFIX "injected dll loaded\n");
            return 1;
        } 
        else {
            kill(child_pid,SIGKILL);
            perror("Proxybound can't be injected, it's not compatible with this application");
            return -1;
        }
    }
    return -1;
}

#define MAX_COMMANDLINE_FLAGS 2

int main(int argc, char *argv[]) {
	char *path = NULL;
	char buf[256];
	char pbuf[256];
	int start_argv = 1;
	int quiet = 0;
	size_t i;
	const char *prefix = NULL;

    if (argc != 2 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        return usage(argv);
    }
    
    if (argc != 2 || !strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
        return version(argv);
    }
    
	for(i = 0; i < MAX_COMMANDLINE_FLAGS; i++) {
		if(start_argv < argc && argv[start_argv][0] == '-') {
			if(argv[start_argv][1] == 'q') {
				quiet = 1;
				start_argv++;
			} else if(argv[start_argv][1] == 'f') {

				if(start_argv + 1 < argc)
					path = argv[start_argv + 1];
				else
					return usage(argv);

				start_argv += 2;
			}
		} else
			break;
	}

	if(start_argv >= argc)
		return usage(argv);

	/* check if path of config file has not been passed via command line */
	path = get_config_path(path, pbuf, sizeof(pbuf));
	
	if(!quiet)
		fprintf(stderr, LOG_PREFIX "config file found: %s\n", path);

	/* Set PROXYBOUND_CONF_FILE to get proxybound lib to use new config file. */
	setenv(PROXYBOUND_CONF_FILE_ENV_VAR, path, 1);
    
    setenv(PROXYBOUND_WORKING_INDICATOR_ENV_VAR, "1", 1);

	if(quiet)
		setenv(PROXYBOUND_QUIET_MODE_ENV_VAR, "1", 1);


	// search DLL
	set_own_dir(argv[0]);
	i = 0;

	while(dll_dirs[i]) {
		snprintf(buf, sizeof(buf), "%s/%s", dll_dirs[i], dll_name);
		if(access(buf, R_OK) != -1) {
			prefix = dll_dirs[i];
			break;
		}
		i++;
	}

	if(!prefix) {
		fprintf(stderr, "couldnt locate %s\n", dll_name);
		return EXIT_FAILURE;
	}
	if(!quiet)
		fprintf(stderr, LOG_PREFIX "preloading %s/%s\n", prefix, dll_name);
    
#ifndef IS_MAC
	snprintf(buf, sizeof(buf), "LD_PRELOAD=%s/%s", prefix, dll_name);
	putenv(buf);
#else
	snprintf(buf, sizeof(buf), "DYLD_INSERT_LIBRARIES=%s/%s", prefix, dll_name);
	putenv(buf);
	putenv("DYLD_FORCE_FLAT_NAMESPACE=1");
#endif
    
    //Running child process ************************************************************
	//execvp(argv[start_argv], &argv[start_argv]);
    child_pid = fork();
    if(child_pid == 0){ 
        //Child process injected application
        execvp(argv[start_argv], &argv[start_argv]);
        exit(0);
    } else {
        //Parent code
    }
    
    if (check_injection())
        return 0;
    else {
	    perror("Proxybound can't load process....");
	    return EXIT_FAILURE;
    }
}
