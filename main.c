#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ntool.h"

int main(int argc, char *argv[]) {
    struct ntool_options opts;

    // Parse command line arguments
    if (parse_args(argc, argv, &opts) != 0) {
        return EXIT_FAILURE;
    }

    // Collect and display network information
    if (collect_network_info(&opts) != 0) {
        fprintf(stderr, "Error collecting network information\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
