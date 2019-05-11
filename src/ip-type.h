#include <stdint.h>

typedef union {
	unsigned char octet[4];
	uint32_t as_int;
} ip_type;

const ip_type ip_type_invalid = { .as_int = -1 };
const ip_type ip_type_localhost = { {127, 0, 0, 1} };

// Definition for ipv6
/*
typedef union {
	unsigned char octet[4];
	uint32_t as_int;
} ip_type4;

typedef struct {
	union {
		ip_type4 v4;
		unsigned char v6[16];
	} addr;
	char is_v6;
} ip_type;

const ip_type ip_type_invalid = { .addr.v4.as_int = -1 };
const ip_type ip_type_localhost = { .addr.v4.octet = {127, 0, 0, 1} };
*/