#ifndef NTOOL_UDP_H
#define NTOOL_UDP_H

#include "common.h"
#include <netinet/udp.h>
#include <netinet/udp_var.h>

// Function to print UDP connections
int print_udp_connections(const ntool_config_t *config);

// Get process information for a UDP connection (if supported)
int get_udp_process_info(struct xinpcb *xip, char *proc_info, size_t proc_info_len);

// Check if a UDP connection matches the filter criteria
bool udp_connection_matches_filter(struct xinpcb *xip, const ntool_config_t *config);

#endif // NTOOL_UDP_H