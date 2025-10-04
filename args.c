#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ntool.h"

int parse_args(int argc, char *argv[], struct ntool_options *opts) {
    int opt;

    // Initialize options to default values
    opts->tcp = false;
    opts->udp = false;
    opts->numeric = false;
    opts->listening = false;
    opts->all = false;
    opts->program = false;

    // Define long options (none for now)
    static struct option long_options[] = {
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "tunlap", long_options, NULL)) != -1) {
        switch (opt) {
            case 't':
                opts->tcp = true;
                break;
            case 'u':
                opts->udp = true;
                break;
            case 'n':
                opts->numeric = true;
                break;
            case 'l':
                opts->listening = true;
                break;
            case 'a':
                opts->all = true;
                break;
            case 'p':
                opts->program = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-t] [-u] [-n] [-l] [-a] [-p]\n", argv[0]);
                fprintf(stderr, "  -t: TCP connections\n");
                fprintf(stderr, "  -u: UDP connections\n");
                fprintf(stderr, "  -n: numeric output\n");
                fprintf(stderr, "  -l: listening sockets\n");
                fprintf(stderr, "  -a: all sockets\n");
                fprintf(stderr, "  -p: show program name\n");
                return -1;
        }
    }

    // If neither -t nor -u specified, show both by default
    if (!opts->tcp && !opts->udp) {
        opts->tcp = true;
        opts->udp = true;
    }

    return 0;
}
