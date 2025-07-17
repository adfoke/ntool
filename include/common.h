#ifndef NTOOL_COMMON_H
#define NTOOL_COMMON_H

/**
 * @file common.h
 * @brief 通用定义和函数声明
 * 
 * 该文件包含ntool工具的通用定义、结构体和函数声明。
 * 它提供了错误处理、配置结构和地址格式化等功能。
 */

// 标准库头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

// 系统头文件
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

// 网络相关头文件
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp_var.h>
#include <netinet/udp_var.h>
#include <netinet/tcp_fsm.h>
#include <arpa/inet.h>

/**
 * @brief IPv4和IPv6标志定义
 * 
 * 如果系统头文件中没有定义这些标志，我们提供自己的定义
 */
#ifndef INP_IPV4
#define INP_IPV4 0x1  // IPv4标志
#endif

#ifndef INP_IPV6
#define INP_IPV6 0x2  // IPv6标志
#endif

// Version information
#define NTOOL_VERSION "1.0.0"

// Error handling
#define NTOOL_ERROR_NONE 0
#define NTOOL_ERROR_SYSCTL 1
#define NTOOL_ERROR_MEMORY 2
#define NTOOL_ERROR_PERMISSION 3
#define NTOOL_ERROR_INVALID_ARG 4
#define NTOOL_ERROR_NOT_SUPPORTED 5
#define NTOOL_ERROR_IO 6
#define NTOOL_ERROR_UNKNOWN 99

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