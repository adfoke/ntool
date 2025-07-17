#ifndef NTOOL_TCP_H
#define NTOOL_TCP_H

#include "common.h"
#include <netinet/tcp.h>
#include <netinet/tcp_var.h>
#include <netinet/tcp_fsm.h>

// Function to print TCP connections
int print_tcp_connections(const ntool_config_t *config);

// Get process information for a TCP connection (if supported)
int get_tcp_process_info(struct xtcpcb *xtp, char *proc_info, size_t proc_info_len);

// Check if a TCP connection matches the filter criteria
bool tcp_connection_matches_filter(struct xtcpcb *xtp, const ntool_config_t *config);

#endif // NTOOL_TCP_H