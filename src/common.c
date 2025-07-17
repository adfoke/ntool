#include "../include/common.h"

/**
 * @brief 错误处理函数
 * 
 * 根据错误代码打印相应的错误信息。
 * 
 * @param message 错误消息前缀
 * @param error_code 错误代码，定义在common.h中
 */
void ntool_error(const char *message, int error_code) {
    fprintf(stderr, "ERROR: ");
    
    switch (error_code) {
        case NTOOL_ERROR_SYSCTL:
            fprintf(stderr, "%s: ", message);
            perror(NULL);  // 使用系统的错误信息
            break;
        case NTOOL_ERROR_MEMORY:
            fprintf(stderr, "%s: Memory allocation failed\n", message);
            break;
        case NTOOL_ERROR_PERMISSION:
            fprintf(stderr, "%s: Permission denied. Try running with sudo.\n", message);
            break;
        case NTOOL_ERROR_INVALID_ARG:
            fprintf(stderr, "%s: Invalid argument provided\n", message);
            break;
        case NTOOL_ERROR_NOT_SUPPORTED:
            fprintf(stderr, "%s: Operation not supported on this system\n", message);
            break;
        case NTOOL_ERROR_IO:
            fprintf(stderr, "%s: I/O error occurred\n", message);
            break;
        case NTOOL_ERROR_UNKNOWN:
            fprintf(stderr, "%s: Unknown error occurred\n", message);
            break;
        default:
            fprintf(stderr, "%s: Error code %d\n", message, error_code);
            break;
    }
}

/**
 * @brief 格式化网络地址和端口
 * 
 * 将网络连接的地址和端口格式化为可读字符串，支持IPv4和IPv6。
 * 
 * @param inp 指向inpcb结构体的指针，包含连接信息
 * @param local 如果为true，格式化本地地址；如果为false，格式化远程地址
 * @param numeric_only 如果为true，只使用数字形式的地址；如果为false，尝试解析主机名
 * @param buffer 输出缓冲区
 * @param buffer_len 缓冲区长度
 */
void format_address(struct inpcb *inp, bool local, bool numeric_only, char *buffer, size_t buffer_len) {
    char addr_str[INET6_ADDRSTRLEN];
    void *addr_ptr = NULL;
    int port = 0;

    // Handle IPv4 addresses
    if (inp->inp_vflag & INP_IPV4) {
        addr_ptr = local ? (void *)&inp->inp_laddr : (void *)&inp->inp_faddr;
        port = local ? inp->inp_lport : inp->inp_fport;
        
        // Convert IP address to string
        inet_ntop(AF_INET, addr_ptr, addr_str, sizeof(addr_str));
        
        // If not numeric_only, try to resolve hostname (not implemented yet)
        // This would require a DNS lookup which is beyond the scope of this example
        
        snprintf(buffer, buffer_len, "%s:%d", addr_str, ntohs(port));
        return;
    } 
    // Handle IPv6 addresses
    else if (inp->inp_vflag & INP_IPV6) {
        // 尝试访问IPv6地址 - 这里使用通用方法，可能需要根据系统调整
        struct in6_addr *in6_addr_ptr = NULL;
        
        // 在macOS上，可能需要使用不同的字段名称
        #ifdef __APPLE__
            // macOS上通常使用in6p_laddr和in6p_faddr字段
            in6_addr_ptr = local ? &inp->in6p_laddr : &inp->in6p_faddr;
        #else
            // 其他系统可能使用不同的字段名
            in6_addr_ptr = local ? (struct in6_addr *)&inp->in6p_laddr : (struct in6_addr *)&inp->in6p_faddr;
        #endif
        
        port = local ? inp->inp_lport : inp->inp_fport;
        
        // 尝试转换IPv6地址
        if (inet_ntop(AF_INET6, in6_addr_ptr, addr_str, sizeof(addr_str)) != NULL) {
            snprintf(buffer, buffer_len, "[%s]:%d", addr_str, ntohs(port));
        } else {
            // 如果转换失败，使用通用格式
            snprintf(buffer, buffer_len, "[IPv6]:%d", ntohs(port));
        }
        return;
    } 
    // Handle unknown address types
    else {
        snprintf(buffer, buffer_len, "*.*");
        return;
    }
}