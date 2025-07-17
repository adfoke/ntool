#include "../include/common.h"

void ntool_error(const char *message, int error_code) {
    if (error_code == NTOOL_ERROR_SYSCTL) {
        perror(message);
    } else if (error_code == NTOOL_ERROR_MEMORY) {
        fprintf(stderr, "%s: Memory allocation failed\n", message);
    } else {
        fprintf(stderr, "%s: Error code %d\n", message, error_code);
    }
}

void format_address(struct inpcb *inp, bool local, bool numeric_only, char *buffer, size_t buffer_len) {
    char addr_str[INET6_ADDRSTRLEN];
    void *addr_ptr = NULL;
    int port = 0;

    // Handle IPv4 addresses
    if (inp->inp_vflag & INP_IPV4) {
        addr_ptr = local ? (void *)&inp->inp_laddr : (void *)&inp->inp_faddr;
        port = local ? inp->inp_lport : inp->inp_fport;
        inet_ntop(AF_INET, addr_ptr, addr_str, sizeof(addr_str));
        snprintf(buffer, buffer_len, "%s:%d", addr_str, ntohs(port));
        return;
    } 
    // Handle IPv6 addresses - 注意：这部分可能需要根据实际系统头文件调整
    else if (inp->inp_vflag & INP_IPV6) {
        // 由于不同系统的结构体定义可能不同，这里使用简化处理
        snprintf(buffer, buffer_len, "[IPv6]:%d", ntohs(local ? inp->inp_lport : inp->inp_fport));
        return;
    } 
    // Handle unknown address types
    else {
        snprintf(buffer, buffer_len, "*.*");
        return;
    }
}