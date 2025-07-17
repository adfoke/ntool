#include "../../include/tcp.h"
#include <libproc.h>

/**
 * @brief TCP状态数组
 * 
 * 与tcp_fsm.h中的状态定义一致，用于将数字状态转换为可读字符串
 */
const char *tcp_states[] = {
    "CLOSED",       "LISTEN",       "SYN_SENT",     "SYN_RECEIVED",
    "ESTABLISHED",  "CLOSE_WAIT",   "FIN_WAIT_1",   "CLOSING",
    "LAST_ACK",     "FIN_WAIT_2",   "TIME_WAIT",
};

/**
 * @brief 获取TCP连接的进程信息
 * 
 * 尝试获取与TCP连接关联的进程ID和进程名
 * 
 * @param xtp 指向xtcpcb结构体的指针，包含TCP连接信息
 * @param proc_info 输出缓冲区，用于存储进程信息
 * @param proc_info_len 缓冲区长度
 * @return 成功返回0，失败返回错误代码
 */
int get_tcp_process_info(struct xtcpcb *xtp, char *proc_info, size_t proc_info_len) {
    // 在macOS上，获取进程信息需要额外的API调用
    // 这里我们简化实现，只显示一个占位符
    // 实际实现可能需要使用libproc库的proc_pidinfo等函数
    
    snprintf(proc_info, proc_info_len, "-");
    
    // 注意：完整实现可能类似于以下代码（需要适当修改）：
    /*
    int pid = ... // 从连接中获取PID
    if (pid > 0) {
        char path[PROC_PIDPATHINFO_MAXSIZE];
        if (proc_pidpath(pid, path, sizeof(path)) > 0) {
            char *name = strrchr(path, '/');
            if (name) name++;
            else name = path;
            snprintf(proc_info, proc_info_len, "%d/%s", pid, name);
            return 0;
        }
    }
    */
    
    return 0;
}

/**
 * @brief 检查TCP连接是否匹配过滤条件
 * 
 * 根据配置中的过滤条件（地址或端口）检查TCP连接是否匹配
 * 
 * @param xtp 指向xtcpcb结构体的指针，包含TCP连接信息
 * @param config 配置结构体，包含过滤条件
 * @return 如果连接匹配过滤条件返回true，否则返回false
 */
bool tcp_connection_matches_filter(struct xtcpcb *xtp, const ntool_config_t *config) {
    struct inpcb *inp = &xtp->xt_inp;
    
    // If no filter is set, match everything
    if (!config->filter_addr && config->filter_port <= 0) {
        return true;
    }
    
    // Check port filter
    if (config->filter_port > 0) {
        if (ntohs(inp->inp_lport) == config->filter_port || 
            ntohs(inp->inp_fport) == config->filter_port) {
            return true;
        }
        return false;
    }
    
    // Check address filter (simplified, would need proper implementation)
    if (config->filter_addr) {
        char local_addr[INET6_ADDRSTRLEN + 6];
        char foreign_addr[INET6_ADDRSTRLEN + 6];
        
        format_address(inp, true, true, local_addr, sizeof(local_addr));
        format_address(inp, false, true, foreign_addr, sizeof(foreign_addr));
        
        if (strstr(local_addr, config->filter_addr) || 
            strstr(foreign_addr, config->filter_addr)) {
            return true;
        }
        return false;
    }
    
    return true;
}

/**
 * @brief 打印TCP连接信息
 * 
 * 获取并显示系统中的TCP连接信息，包括本地地址、远程地址、连接状态和可选的进程信息。
 * 支持按地址或端口过滤连接。
 * 
 * @param config 配置结构体，包含显示选项和过滤条件
 * @return 成功返回NTOOL_ERROR_NONE，失败返回相应的错误代码
 */
int print_tcp_connections(const ntool_config_t *config) {
    char *buf = NULL;
    size_t len = 0;
    int name[] = { CTL_NET, PF_INET, IPPROTO_TCP, TCPCTL_PCBLIST }; // Name for sysctl
    int result = NTOOL_ERROR_NONE;

    printf("Active Internet connections (TCP)\n");
    if (config->show_process_info) {
        printf("%-25s %-25s %-15s %-15s\n", "Local Address", "Foreign Address", "State", "PID/Program");
    } else {
        printf("%-25s %-25s %-15s\n", "Local Address", "Foreign Address", "State");
    }

    // Get the size of the buffer needed
    if (sysctl(name, 4, NULL, &len, NULL, 0) < 0) {
        if (errno == EACCES || errno == EPERM) {
            ntool_error("sysctl (get size) TCP failed", NTOOL_ERROR_PERMISSION);
            return NTOOL_ERROR_PERMISSION;
        } else if (errno == ENOMEM) {
            ntool_error("sysctl (get size) TCP failed", NTOOL_ERROR_MEMORY);
            return NTOOL_ERROR_MEMORY;
        } else {
            ntool_error("sysctl (get size) TCP failed", NTOOL_ERROR_SYSCTL);
            return NTOOL_ERROR_SYSCTL;
        }
    }

    // Allocate the buffer
    buf = malloc(len);
    if (buf == NULL) {
        ntool_error("malloc failed", NTOOL_ERROR_MEMORY);
        return NTOOL_ERROR_MEMORY;
    }

    // Get the actual connection data
    if (sysctl(name, 4, buf, &len, NULL, 0) < 0) {
        if (errno == EACCES || errno == EPERM) {
            ntool_error("sysctl (get data) TCP failed", NTOOL_ERROR_PERMISSION);
            free(buf);
            return NTOOL_ERROR_PERMISSION;
        } else if (errno == ENOMEM) {
            ntool_error("sysctl (get data) TCP failed", NTOOL_ERROR_MEMORY);
            free(buf);
            return NTOOL_ERROR_MEMORY;
        } else {
            ntool_error("sysctl (get data) TCP failed", NTOOL_ERROR_SYSCTL);
            free(buf);
            return NTOOL_ERROR_SYSCTL;
        }
    }

    // Iterate through the connection structures
    char *end = buf + len;
    struct xtcpcb *xtp = (struct xtcpcb *)buf;

    // Skip the header structure
    xtp = (struct xtcpcb *)((char *)xtp + xtp->xt_len);

    while ((char *)xtp < end && xtp->xt_len > 0) {
        struct tcpcb *tp = &xtp->xt_tp;
        struct inpcb *inp = &xtp->xt_inp;
        
        // Apply filter if needed
        if (!tcp_connection_matches_filter(xtp, config)) {
            // Skip this connection if it doesn't match the filter
            xtp = (struct xtcpcb *)((char *)xtp + xtp->xt_len);
            continue;
        }
        
        char local_addr[INET6_ADDRSTRLEN + 6]; // Address + :port
        char foreign_addr[INET6_ADDRSTRLEN + 6];

        format_address(inp, true, config->numeric_only, local_addr, sizeof(local_addr));
        format_address(inp, false, config->numeric_only, foreign_addr, sizeof(foreign_addr));

        // Use the size of the tcp_states array for bounds checking
        const size_t num_tcp_states = sizeof(tcp_states) / sizeof(tcp_states[0]);
        const char *state_str = (tp->t_state >= 0 && tp->t_state < num_tcp_states) ? 
                               tcp_states[tp->t_state] : "UNKNOWN";

        if (config->show_process_info) {
            char proc_info[32] = {0};
            get_tcp_process_info(xtp, proc_info, sizeof(proc_info));
            printf("%-25s %-25s %-15s %-15s\n", local_addr, foreign_addr, state_str, proc_info);
        } else {
            printf("%-25s %-25s %-15s\n", local_addr, foreign_addr, state_str);
        }

        // Move to the next structure
        xtp = (struct xtcpcb *)((char *)xtp + xtp->xt_len);
    }

    free(buf);
    return result;
}