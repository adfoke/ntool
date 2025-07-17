#include "../../include/udp.h"

int get_udp_process_info(struct xinpcb *xip, char *proc_info, size_t proc_info_len) {
    // This is platform-specific and would require additional system calls
    // For macOS, we might use libproc or other APIs
    // For simplicity, we'll just indicate it's not implemented yet
    snprintf(proc_info, proc_info_len, "-");
    return 0;
}

bool udp_connection_matches_filter(struct xinpcb *xip, const ntool_config_t *config) {
    struct inpcb *inp = &xip->xi_inp;
    
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
        
        format_address(inp, true, true, local_addr, sizeof(local_addr));
        
        if (strstr(local_addr, config->filter_addr)) {
            return true;
        }
        
        // Only check foreign address if it's not empty
        if (inp->inp_fport != 0) {
            char foreign_addr[INET6_ADDRSTRLEN + 6];
            format_address(inp, false, true, foreign_addr, sizeof(foreign_addr));
            if (strstr(foreign_addr, config->filter_addr)) {
                return true;
            }
        }
        
        return false;
    }
    
    return true;
}

int print_udp_connections(const ntool_config_t *config) {
    char *buf = NULL;
    size_t len = 0;
    int name[] = { CTL_NET, PF_INET, IPPROTO_UDP, UDPCTL_PCBLIST }; // Name for sysctl
    int result = NTOOL_ERROR_NONE;

    printf("Active Internet connections (UDP)\n");
    if (config->show_process_info) {
        printf("%-25s %-25s %-15s\n", "Local Address", "Foreign Address", "PID/Program");
    } else {
        printf("%-25s %-25s\n", "Local Address", "Foreign Address");
    }

    // Get the size of the buffer needed
    if (sysctl(name, 4, NULL, &len, NULL, 0) < 0) {
        ntool_error("sysctl (get size) UDP failed", NTOOL_ERROR_SYSCTL);
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
        ntool_error("sysctl (get data) UDP failed", NTOOL_ERROR_SYSCTL);
        free(buf);
        return NTOOL_ERROR_SYSCTL;
    }

    // Iterate through the connection structures
    char *end = buf + len;
    struct xinpcb *xip = (struct xinpcb *)buf;

    // Skip header structure
    xip = (struct xinpcb *)((char *)xip + xip->xi_len);

    while ((char *)xip < end && xip->xi_len > 0) {
        struct inpcb *inp = &xip->xi_inp;
        
        // Apply filter if needed
        if (!udp_connection_matches_filter(xip, config)) {
            // Skip this connection if it doesn't match the filter
            xip = (struct xinpcb *)((char *)xip + xip->xi_len);
            continue;
        }
        
        char local_addr[INET6_ADDRSTRLEN + 6];
        char foreign_addr[INET6_ADDRSTRLEN + 6];

        format_address(inp, true, config->numeric_only, local_addr, sizeof(local_addr));
        
        // For UDP, the foreign address might be zero if not connected
        if ((inp->inp_vflag & INP_IPV4 && inp->inp_faddr.s_addr != INADDR_ANY) ||
            (inp->inp_vflag & INP_IPV6 && !IN6_IS_ADDR_UNSPECIFIED(&inp->in6p_faddr)) ||
            inp->inp_fport != 0) {
            format_address(inp, false, config->numeric_only, foreign_addr, sizeof(foreign_addr));
        } else {
            snprintf(foreign_addr, sizeof(foreign_addr), "*.*");
        }

        if (config->show_process_info) {
            char proc_info[32] = {0};
            get_udp_process_info(xip, proc_info, sizeof(proc_info));
            printf("%-25s %-25s %-15s\n", local_addr, foreign_addr, proc_info);
        } else {
            printf("%-25s %-25s\n", local_addr, foreign_addr);
        }

        // Move to the next structure
        xip = (struct xinpcb *)((char *)xip + xip->xi_len);
    }

    free(buf);
    return result;
}