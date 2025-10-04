#ifndef NTOOL_H
#define NTOOL_H

#include <stdbool.h>

// Command line options structure
struct ntool_options {
    bool tcp;           // -t: TCP connections
    bool udp;           // -u: UDP connections
    bool numeric;       // -n: numeric output (no DNS resolution)
    bool listening;     // -l: listening sockets
    bool all;           // -a: all sockets
    bool program;       // -p: show program name
};

// Function declarations
int parse_args(int argc, char *argv[], struct ntool_options *opts);
int collect_network_info(struct ntool_options *opts);

#endif // NTOOL_H
