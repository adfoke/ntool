#include "../../include/tcp.h"

// TCP states array (consistent with tcp_fsm.h)
const char *tcp_states[] = {
    "CLOSED",       "LISTEN",       "SYN_SENT",     "SYN_RECEIVED",
    "ESTABLISHED",  "CLOSE_WAIT",   "FIN_WAIT_1",   "CLOSING",
    "LAST_ACK",     "FIN_WAIT_2",   "TIME_WAIT",
};

int get_tcp_process_info(struct xtcpcb *xtp, char *proc_info, size_t proc_info_len) {
    // This is platform-specific and would require additional system calls
    // For macOS, we might use libproc or other APIs
    // For simplicity, we'll just indicate it's not implemented yet
    snprintf(proc_info, proc_info_len, "-");
    return 0;
}

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
        ntool_error("sysctl (get size) TCP failed", NTOOL_ERROR_SYSCTL);
        return NTOOL_ERROR_SYSCTL;
    }

    // Allocate the buffer
    buf = malloc(len);
    if (buf == NULL) {
        ntool_error("malloc failed", NTOOL_ERROR_MEMORY);
        return NTOOL_ERROR_MEMORY;
    }

    // Get the actual connection data
    if (sysctl(name, 4, buf, &len, NULL, 0) < 0) {
        ntool_error("sysctl (get data) TCP failed", NTOOL_ERROR_SYSCTL);
        free(buf);
        return NTOOL_ERROR_SYSCTL;
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