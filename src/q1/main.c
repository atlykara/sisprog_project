#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory_analyzer.h"
#include "leak_detector.h"

static void print_usage(const char *prog) {
    printf("Usage:\n");
    printf("  %s                # Basic analysis\n", prog);
    printf("  %s --monitor N    # Memory monitoring (update every N seconds)\n", prog);
    printf("  %s --leak-check   # Leak testing demo\n", prog);
    printf("  %s --all          # Run all features\n", prog);
}

static void run_leak_demo(void) {
    printf("### Leak Detection ###\n");
    char *p1 = MALLOC(100);
    char *p2 = MALLOC(200);
    if (!p1 || !p2) {
        printf("Allocation failed in leak demo.\n");
        return;
    }

    //p1 free, p2 intentionally leak
    FREE(p1);
    // p2 intentionally NOT freed

    print_leak_report();
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        //Basic usage: just segments + VM
        if (analyze_main_segments() != 0) return EXIT_FAILURE;
        if (list_all_segments() != 0) return EXIT_FAILURE;
        if (analyze_virtual_physical_memory() != 0) return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }

    if (argc >= 2) {
        if (strcmp(argv[1], "--monitor") == 0) {
            if (argc < 3) {
                fprintf(stderr, "--monitor requires an interval in seconds\n");
                print_usage(argv[0]);
                return EXIT_FAILURE;
            }
            int interval = atoi(argv[2]);
            return monitor_memory_usage(interval);
        } else if (strcmp(argv[1], "--leak-check") == 0) {
            run_leak_demo();
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "--all") == 0) {
            if (analyze_main_segments() != 0) return EXIT_FAILURE;
            if (list_all_segments() != 0) return EXIT_FAILURE;
            if (analyze_virtual_physical_memory() != 0) return EXIT_FAILURE;
            run_leak_demo();
            return EXIT_SUCCESS;
        } else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    print_usage(argv[0]);
    return EXIT_FAILURE;
}
