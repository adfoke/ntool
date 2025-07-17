#ifndef NTOOL_COMMON_H
#define NTOOL_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp_var.h>
#include <netinet/udp_var.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>

// 定义IPv4和IPv6标志，如果头文件中没有定义
#ifndef INP_IPV4
#define INP_IPV4 0x1
#endif

#ifndef INP_IPV6
#define INP_IPV6 0x2
#endif

// Version information
#define NTOOL_VERSION "1.0.0"

// Error handling
#define NTOOL_ERROR_NONE 0
#define NTOOL_ERROR_SYSCTL 1
#define NTOOL_ERROR_MEMORY 2

// Common structures for configuration
typedef struct {
    bool show_tcp;
    bool show_udp;
    bool numeric_only;      // Don't resolve hostnames
    bool show_process_info; // Show process information
    char *filter_addr;      // Filter by address
    int filter_port;        // Filter by port
} ntool_config_t;

// Error handling function
void ntool_error(const char *message, int error_code);

// Common utility functions
void format_address(struct inpcb *inp, bool local, bool numeric_only, char *buffer, size_t buffer_len);

#endif // NTOOL_COMMON_H