#include "memory_analyzer.h"
#include "proc_parser.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void print_segment(const char *name, const SegmentInfo *seg) {
    if (!seg || seg->start == 0 || seg->size == 0) {
        printf("%s segment: (not found)\n", name);
        return;
    }
    printf("%s segment : start = 0x%lx, size = %zu Bytes\n",
           name, seg->start, seg->size);
}

int analyze_main_segments(void) {
    SegmentInfo text, data, bss, heap, stack;
    if (parse_proc_maps_for_segments(&text, &data, &bss, &heap, &stack) != 0) {
        fprintf(stderr, "Failed to parse main segments from /proc/self/maps\n");
        return -1;
    }

    printf("### Memory Segment Analysis ###\n");
    print_segment("Data", &data);
    print_segment("BSS", &bss);
    print_segment("Heap", &heap);
    print_segment("Stack", &stack);
    print_segment("Text", &text);
    printf("\n");
    return 0;
}

int list_all_segments(void) {
    LibraryInfo *libs = NULL;
    int lib_count = 0;
    LibraryInfo vdso;
    int has_vdso = 0;

    if (parse_proc_maps_libraries(&libs, &lib_count, &vdso, &has_vdso) != 0) {
        fprintf(stderr, "Failed to list libraries from /proc/self/maps\n");
        return -1;
    }

    printf("### Shared Libraries ###\n");
    for (int i = 0; i < lib_count; ++i) {
        LibraryInfo *lib = &libs[i];
        printf("%s : 0x%lx-0x%lx (%zu KB) %s\n",
               lib->name, lib->start, lib->end, lib->size_kb, lib->perms);
    }

    if (has_vdso) {
        printf("\n### Virtual Dynamic Shared Object (VDSO) ###\n");
        printf("%s : 0x%lx-0x%lx (%zu KB) %s\n",
               vdso.name, vdso.start, vdso.end, vdso.size_kb, vdso.perms);
    }

    free(libs);
    printf("\n");
    return 0;
}

int analyze_virtual_physical_memory(void) {
    VmStats stats;
    if (parse_proc_status_vm(&stats) != 0) {
        fprintf(stderr, "Failed to read Vm stats from /proc/self/status\n");
        return -1;
    }

    printf("### Virtual vs Physical Memory ###\n");
    printf("Virtual Memory:\n");
    printf("  VmSize : %zu KB  (Total virtual memory)\n", stats.vm_size_kb);
    printf("  VmRSS  : %zu KB  (Resident in physical memory)\n", stats.vm_rss_kb);
    printf("  VmData : %zu KB  (Data segment)\n", stats.vm_data_kb);
    printf("  VmStk  : %zu KB  (Stack)\n", stats.vm_stk_kb);
    printf("  VmExe  : %zu KB  (Text/Code)\n", stats.vm_exe_kb);

    double efficiency = 0.0;
    if (stats.vm_size_kb > 0) {
        efficiency = (double)stats.vm_rss_kb / (double)stats.vm_size_kb * 100.0;
    }
    printf("\nMemory Efficiency: %.2f%% (Physical/Virtual)\n\n", efficiency);
    return 0;
}

int monitor_memory_usage(int interval_seconds) {
    if (interval_seconds <= 0) interval_seconds = 5;

    printf("### Memory Monitoring (every %d seconds) ###\n", interval_seconds);
    printf("Press Ctrl+C to stop.\n\n");

    while (1) {
        if (analyze_virtual_physical_memory() != 0) {
            return -1;
        }
        sleep((unsigned int)interval_seconds);
    }
    return 0;
}
