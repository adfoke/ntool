#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h> // For getopt

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/tcp_var.h> // For xtcpcb
#include <netinet/udp.h>
#include <netinet/udp_var.h> // For xinpcb
#include <netinet/tcp_fsm.h> // For TCP states constants/definitions
// Removed: #include <netinet6/in6_pcb.h>
#include <arpa/inet.h>

// TCP states array (consistent with tcp_fsm.h)
const char *tcp_states[] = {
    "CLOSED",       "LISTEN",       "SYN_SENT",     "SYN_RECEIVED",
    "ESTABLISHED",  "CLOSE_WAIT",   "FIN_WAIT_1",   "CLOSING",
    "LAST_ACK",     "FIN_WAIT_2",   "TIME_WAIT",
};

// Helper to format IPv4/IPv6 addresses and ports
void format_address(struct inpcb *inp, bool local, char *buffer, size_t buffer_len) {
    char addr_str[INET6_ADDRSTRLEN];
    void *addr_ptr = NULL;
    int port = 0;

    if (inp->inp_vflag & INP_IPV4) {
        addr_ptr = local ? (void *)&inp->inp_laddr : (void *)&inp->inp_faddr;
        port = local ? inp->inp_lport : inp->inp_fport;
        inet_ntop(AF_INET, addr_ptr, addr_str, sizeof(addr_str));
    // } else if (inp->inp_vflag & INP_IPV6) {
    //     // Temporarily disabling IPv6 due to structure issues
    //     snprintf(buffer, buffer_len, "[IPv6 Addr]:%d", ntohs(local ? inp->inp_lport : inp->inp_fport));
    //     return;
    } else {
        // Handle non-IPv4 cases (including temporarily disabled IPv6)
        snprintf(buffer, buffer_len, "*.*"); // Or indicate unknown type
        return;
    }

    snprintf(buffer, buffer_len, "%s:%d", addr_str, ntohs(port));
}

// Function to print TCP connections
void print_tcp_connections() {
    char *buf = NULL;
    size_t len = 0;
    int name[] = { CTL_NET, PF_INET, IPPROTO_TCP, TCPCTL_PCBLIST }; // Name for sysctl

    printf("Active Internet connections (TCP)\n");
    printf("%-25s %-25s %-15s\n", "Local Address", "Foreign Address", "State");

    // Get the size of the buffer needed
    if (sysctl(name, 4, NULL, &len, NULL, 0) < 0) {
        perror("sysctl (get size) TCP failed");
        return;
    }

    // Allocate the buffer
    buf = malloc(len);
    if (buf == NULL) {
        perror("malloc failed");
        return;
    }

    // Get the actual connection data
    if (sysctl(name, 4, buf, &len, NULL, 0) < 0) {
        perror("sysctl (get data) TCP failed");
        free(buf);
        return;
    }

    // Iterate through the connection structures
    // The buffer contains a series of xtcpcb structures, but the first one
    // is a template/header, so we skip it. The actual layout is tricky.
    // We need to step through based on the size of the structures.
    char *end = buf + len;
    struct xtcpcb *xtp = (struct xtcpcb *)buf;

    // Skip the header structure (xtp points to the first actual entry after this loop)
    // This assumes the first entry's ki_len is the size of the header.
    // A safer approach might involve checking ki_structsize if available.
    xtp = (struct xtcpcb *)((char *)xtp + xtp->xt_len);


    while ((char *)xtp < end && xtp->xt_len > 0) {
        struct tcpcb *tp = &xtp->xt_tp;
        struct inpcb *inp = &xtp->xt_inp;
        char local_addr[INET6_ADDRSTRLEN + 6]; // Address + :port
        char foreign_addr[INET6_ADDRSTRLEN + 6];

        format_address(inp, true, local_addr, sizeof(local_addr));
        format_address(inp, false, foreign_addr, sizeof(foreign_addr));

        // Use the size of the tcp_states array for bounds checking
        const size_t num_tcp_states = sizeof(tcp_states) / sizeof(tcp_states[0]);
        const char *state_str = (tp->t_state >= 0 && tp->t_state < num_tcp_states) ? tcp_states[tp->t_state] : "UNKNOWN";

        printf("%-25s %-25s %-15s\n", local_addr, foreign_addr, state_str);

        // Move to the next structure
        xtp = (struct xtcpcb *)((char *)xtp + xtp->xt_len);
    }


    free(buf);
}


// Function to print UDP connections
void print_udp_connections() {
    char *buf = NULL;
    size_t len = 0;
    int name[] = { CTL_NET, PF_INET, IPPROTO_UDP, UDPCTL_PCBLIST }; // Name for sysctl

    printf("\nActive Internet connections (UDP)\n");
    printf("%-25s %-25s\n", "Local Address", "Foreign Address");

    if (sysctl(name, 4, NULL, &len, NULL, 0) < 0) {
        perror("sysctl (get size) UDP failed");
        return;
    }

    buf = malloc(len);
    if (buf == NULL) {
        perror("malloc failed");
        return;
    }

    if (sysctl(name, 4, buf, &len, NULL, 0) < 0) {
        perror("sysctl (get data) UDP failed");
        free(buf);
        return;
    }

    char *end = buf + len;
    struct xinpcb *xip = (struct xinpcb *)buf;

    // Skip header structure
     xip = (struct xinpcb *)((char *)xip + xip->xi_len);


    while ((char *)xip < end && xip->xi_len > 0) {
        struct inpcb *inp = &xip->xi_inp;
        char local_addr[INET6_ADDRSTRLEN + 6];
        char foreign_addr[INET6_ADDRSTRLEN + 6]; // UDP doesn't have "foreign" in the same way, often *.*

        format_address(inp, true, local_addr, sizeof(local_addr));
        // For UDP, the foreign address might be zero if not connected
        // Temporarily disabling IPv6 check
        if ((inp->inp_vflag & INP_IPV4 && inp->inp_faddr.s_addr != INADDR_ANY) ||
            // (inp->inp_vflag & INP_IPV6 && !IN6_IS_ADDR_UNSPECIFIED(&inp->inp_faddr6)) || // Disabled IPv6 check
            inp->inp_fport != 0) {
             // Only format if it's IPv4 with a valid foreign address/port
             if (inp->inp_vflag & INP_IPV4) {
                 format_address(inp, false, foreign_addr, sizeof(foreign_addr));
             } else {
                 snprintf(foreign_addr, sizeof(foreign_addr), "*.*"); // Default for non-IPv4
             }
        } else {
            snprintf(foreign_addr, sizeof(foreign_addr), "*.*");
        }


        printf("%-25s %-25s\n", local_addr, foreign_addr);

        xip = (struct xinpcb *)((char *)xip + xip->xi_len);
    }

    free(buf);
}

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-t | -u | -a]\n", prog_name);
    fprintf(stderr, "  -t: Show only TCP connections\n");
    fprintf(stderr, "  -u: Show only UDP connections\n");
    fprintf(stderr, "  -a: Show all (TCP and UDP) connections (default)\n");
}

int main(int argc, char *argv[]) {
    bool show_tcp = true;
    bool show_udp = true;
    int opt;

    // If arguments are provided, default to showing nothing unless specified
    if (argc > 1) {
        show_tcp = false;
        show_udp = false;
    }

    while ((opt = getopt(argc, argv, "tua")) != -1) {
        switch (opt) {
            case 't':
                show_tcp = true;
                break;
            case 'u':
                show_udp = true;
                break;
            case 'a':
                show_tcp = true;
                show_udp = true;
                break;
            default: /* '?' */
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // If after parsing options, neither is selected (e.g., only invalid options given),
    // revert to default (show all) or handle as error. Let's default to all.
    if (!show_tcp && !show_udp && argc > 1) {
         fprintf(stderr, "No valid display option selected. Defaulting to show all.\n");
         show_tcp = true;
         show_udp = true;
         // Alternatively, could exit with usage error:
         // print_usage(argv[0]);
         // return EXIT_FAILURE;
    }


    if (show_tcp) {
        print_tcp_connections();
    }
    if (show_udp) {
        // Add a newline if both are printed for separation
        if (show_tcp) {
            printf("\n");
        }
        print_udp_connections();
    }

    return 0;
}
