#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "ntool.h"

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef __linux__
int collect_linux_network_info(struct ntool_options *opts);
#endif

#ifdef __APPLE__
int collect_macos_network_info(struct ntool_options *opts);
#endif

int collect_network_info(struct ntool_options *opts) {
#ifdef __linux__
    return collect_linux_network_info(opts);
#elif defined(__APPLE__)
    return collect_macos_network_info(opts);
#else
    fprintf(stderr, "Unsupported platform\n");
    return -1;
#endif
}

#ifdef __linux__
int collect_linux_network_info(struct ntool_options *opts) {
    printf("Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name\n");

    if (opts->tcp) {
        if (read_proc_net("/proc/net/tcp", "tcp", opts) != 0) {
            return -1;
        }
    }

    if (opts->udp) {
        if (read_proc_net("/proc/net/udp", "udp", opts) != 0) {
            return -1;
        }
    }

    return 0;
}

int read_proc_net(const char *filename, const char *proto, struct ntool_options *opts) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char line[1024];
    // Skip header line
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        // Parse /proc/net/tcp format
        // Format: sl local_address rem_address st tx_queue rx_queue tr tm->when retrnsmt uid timeout inode
        unsigned int sl, local_port, remote_port, st, uid, timeout, inode;
        char local_addr[9], remote_addr[9];
        unsigned long tx_queue, rx_queue;

        if (sscanf(line, "%u: %8s:%x %8s:%x %x %lx:%lx %x:%x %x %u %u",
                   &sl, local_addr, &local_port, remote_addr, &remote_port,
                   &st, &tx_queue, &rx_queue, &timeout, &timeout, &uid, &timeout, &inode) != 13) {
            continue;
        }

        // Convert hex addresses to dotted decimal
        struct in_addr local_in, remote_in;
        sscanf(local_addr, "%x", &local_in.s_addr);
        sscanf(remote_addr, "%x", &remote_in.s_addr);

        char local_str[INET_ADDRSTRLEN], remote_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &local_in, local_str, sizeof(local_str));
        inet_ntop(AF_INET, &remote_in, remote_str, sizeof(remote_str));

        // Filter based on options
        if (opts->listening && st != 0x0A) continue; // LISTEN state
        if (!opts->all && st == 0x0A) continue; // Skip LISTEN unless -a or -l

        printf("%-5s %-6lu %-6lu %-23s %-23s ",
               proto, rx_queue, tx_queue,
               opts->numeric ? local_str : resolve_address(local_str, local_port),
               opts->numeric ? remote_str : resolve_address(remote_str, remote_port));

        // Print state
        print_tcp_state(st);

        if (opts->program) {
            printf(" %u/%s", uid, "unknown"); // TODO: resolve PID/program
        }

        printf("\n");
    }

    fclose(fp);
    return 0;
}

void print_tcp_state(unsigned int state) {
    const char *states[] = {
        "UNKNOWN", "ESTABLISHED", "SYN_SENT", "SYN_RECV", "FIN_WAIT1",
        "FIN_WAIT2", "TIME_WAIT", "CLOSE", "CLOSE_WAIT", "LAST_ACK",
        "LISTEN", "CLOSING"
    };

    if (state < sizeof(states)/sizeof(states[0])) {
        printf("%-11s ", states[state]);
    } else {
        printf("UNKNOWN    ");
    }
}

const char *resolve_address(const char *ip, unsigned int port) {
    static char buffer[256];
    // TODO: implement DNS resolution
    snprintf(buffer, sizeof(buffer), "%s:%u", ip, port);
    return buffer;
}
#endif

#ifdef __APPLE__
int collect_macos_network_info(struct ntool_options *opts) {
    // macOS implementation using sysctl is complex due to kernel structures
    // For now, provide a basic implementation or suggest using system netstat
    printf("macOS network info collection requires additional implementation\n");
    printf("For now, you can use: netstat -tulpn\n");
    return 0;
}
#endif
