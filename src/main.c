#include "../include/common.h"
#include "../include/tcp.h"
#include "../include/udp.h"

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", prog_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -t: Show only TCP connections\n");
    fprintf(stderr, "  -u: Show only UDP connections\n");
    fprintf(stderr, "  -a: Show all (TCP and UDP) connections (default)\n");
    fprintf(stderr, "  -n: Don't resolve hostnames (numeric output only)\n");
    fprintf(stderr, "  -p: Show process information (PID/Program name)\n");
    fprintf(stderr, "  -f <addr>: Filter by address\n");
    fprintf(stderr, "  -P <port>: Filter by port number\n");
    fprintf(stderr, "  -h: Show this help message\n");
    fprintf(stderr, "  -v: Show version information\n");
}

void print_version() {
    printf("ntool version %s\n", NTOOL_VERSION);
    printf("A small network tool for the terminal\n");
    printf("Copyright (c) 2025\n");
}

int main(int argc, char *argv[]) {
    ntool_config_t config = {
        .show_tcp = true,
        .show_udp = true,
        .numeric_only = false,
        .show_process_info = false,
        .filter_addr = NULL,
        .filter_port = -1
    };
    int opt;

    // If arguments are provided, default to showing nothing unless specified
    if (argc > 1) {
        config.show_tcp = false;
        config.show_udp = false;
    }

    while ((opt = getopt(argc, argv, "tuanpf:P:hv")) != -1) {
        switch (opt) {
            case 't':
                config.show_tcp = true;
                break;
            case 'u':
                config.show_udp = true;
                break;
            case 'a':
                config.show_tcp = true;
                config.show_udp = true;
                break;
            case 'n':
                config.numeric_only = true;
                break;
            case 'p':
                config.show_process_info = true;
                break;
            case 'f':
                config.filter_addr = optarg;
                break;
            case 'P':
                config.filter_port = atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            default: /* '?' */
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // If after parsing options, neither is selected (e.g., only invalid options given),
    // revert to default (show all) or handle as error. Let's default to all.
    if (!config.show_tcp && !config.show_udp && argc > 1) {
        fprintf(stderr, "No valid display option selected. Defaulting to show all.\n");
        config.show_tcp = true;
        config.show_udp = true;
    }

    int result = NTOOL_ERROR_NONE;

    if (config.show_tcp) {
        result = print_tcp_connections(&config);
        if (result != NTOOL_ERROR_NONE) {
            return result;
        }
    }
    
    if (config.show_udp) {
        // Add a newline if both are printed for separation
        if (config.show_tcp) {
            printf("\n");
        }
        result = print_udp_connections(&config);
        if (result != NTOOL_ERROR_NONE) {
            return result;
        }
    }

    return EXIT_SUCCESS;
}