ProxyBound v5.50
================

ProxyBound is a UNIX program, that hooks network-related libc functions in dynamically linked programs via a preloaded DLL and redirects the connections through SOCKS4a/5 or HTTP proxies. This is based on  proxychains-ng by rofl0r, proxychains by haad and torsocks by dgoulet
  
Features:
=========

- Proxify applications (like mainstream proxychains)
- Works with tcp (like mainstream proxychains)
- No leaks over udp/raw/icmp/etc...
- Unsupported protocols are blocked
- Works with chrome/chromium/similar 
- Incompatible applications are terminated with a proper message
- Many additional settings over environment variable
- etc.

Used environment variable:
==========================

```
- PROXYBOUND_CONF_FILE:         Path to config file (default ./proxybound.conf then /etc/proxybound.conf)
- PROXYBOUND_QUIET_MODE:        Quiet mode (1 or 0, default 0)
- PROXYBOUND_SOCKS5_HOST:       Specify unique socks 5 proxy to use (default not used)
- PROXYBOUND_SOCKS5_PORT:       Socks 5 port (default not used)
- PROXYBOUND_FORCE_DNS:         Force dns resolv requests through (1 or 0, default 1)
- PROXYBOUND_ALLOW_DNS:         Allow direct dns, allow udp port 53 and 853 (1 or 0, default 0)
- PROXYBOUND_ALLOW_LEAKS:       Allow/Block unproxyfied protocols "UDP/ICMP/RAW", blocked by default (1 or 0, default 0)
- PROXYBOUND_WORKING_INDICATOR: Create '/tmp/proxybound.tmp' when dll is working as intended (1 or 0, default 0)
```

How it works:
=============

Proxybound hook libc functions like connect(), dynamic loader facilities are used, namely dl_sym() and LD_PRELOAD thus dynamically linked programs are required.
  
Limits : 
========

- IPv6 is blocked and is not supported
- Some applications are incompatible (they will be explicitly terminated 2 sec after startup, to avoid leaks)

Install:
========

```
  ./configure
  make
  [optional] sudo make install
```

if you dont install, you can use proxybound from the build directory like this: `./proxybound -f src/proxybound.conf telnet google.com 80`

Install debug version :
=======================

```
  ./configure
  make debug
  [optional] sudo make install
```

Changelog:
==========

**Version 5.50:**

- Block non tcp packet on send()
- Prevent bypass noleak
- Add support for bind() to block listen on unsupported protocol 
- Add skype support

**Version 5.40:**

- Add "--help" and "-h" parameter
- Add "--version" and "-v" parameter
- Update help page

**Version 5.30:**

- Code cleaning
- Move readme to md
- Add support for hosts file (upstream import)

**Version 5.20:**

- add PROXYBOUND_ALLOW_DNS
- update lock feature (send sendto sendmsg)
- add function to terminate when app is not supported
- add PROXYBOUND_WORKING_INDICATOR
- Code cleaning
- Update readme 

**Version 5.10:**

- leak lock feature (send sendto sendmsg)

**Version 5.00:**

- Initial leak lock feature
- Code cleaning
- Update readme 

**Version 4.90:**

- Import simple SOCKS5 proxy mode from @haad/proxychains
- Code cleaning
- Update readme 

**Version 4.80:**

- Updates with some features from @haad/proxychains

**Version 4.70:**

- Fix chrome compatibility

**Version 4.60:**

- Code cleanning & update

**Version 4.50:**

- Changing the name to ProxyBound

**Version 4.40:**

- Import security issue fix CVE-2015-3887 
- Used v4.3 (4.03) for initial fork

Configuration:
==============

Proxybound config file in following order:

```
1)	File listed in environment variable ${PROXYBOUND_CONF_FILE} or
    provided as a -f argument to proxybound script or binary.
2)	./proxybound.conf
3)	$(HOME)/.proxybound/proxybound.conf
4)	/etc/proxybound.conf
```

Usage Example:
==============

In this example it will run telnet through proxy without using proxybound binary

```
$ export PROXYBOUND_QUIET_MODE="1"
$ export LD_PRELOAD=/usr/local/lib/libproxybound.so
$ export PROXYBOUND_CONF_FILE=/etc/proxybound.conf
$ telnet targethost.com
```

In this example it will run telnet through proxy(or chained proxies) specified by proxybound.conf

```
$ proxybound telnet targethost.com
```

In this example it will use different configuration file then proxybound.conf to connect to targethost2.com host.

```
$ proxybound -f /etc/proxybound-other.conf targethost2.com
```

In this example it will resolve targethost.com through proxy(or chained proxies) specified by proxybound.conf

```
$ proxyresolv targethost.com
```
