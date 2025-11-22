#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "security_checker.h"

static void print_usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s -s file1.c [file2.c ...]   # Scan mode\n", prog);
    printf("  %s -r file1.c [file2.c ...]   # Recommendation mode\n", prog);
    printf("  %s -x file1.c [file2.c ...]   # Extended analysis\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    char **files = &argv[2];
    int file_count = argc - 2;

    if (strcmp(mode, "-s") == 0) {
        run_scan_mode(files, file_count);
    } else if (strcmp(mode, "-r") == 0) {
        run_recommendation_mode(files, file_count);
    } else if (strcmp(mode, "-x") == 0) {
        run_extended_mode(files, file_count);
    } else {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
