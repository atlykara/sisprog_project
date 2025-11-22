#define _GNU_SOURCE
#include "proc_parser.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static int parse_address_range(const char *range, unsigned long *start, unsigned long *end) {
//    return sscanf(range, "%lx-%lx", start, end) == 2;
//}

int parse_proc_maps_for_segments(SegmentInfo *text,
                                 SegmentInfo *data,
                                 SegmentInfo *bss,
                                 SegmentInfo *heap,
                                 SegmentInfo *stack)
{
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) {
        perror("fopen /proc/self/maps");
        return -1;
    }

    char line[512];
    char exe_path[256] = {0};
    //ssize_t len;
    // Get executable path from /proc/self/exe
    while (fgets(line, sizeof(line), f)) {
        unsigned long s, e;
        char perms[5], dev[12], map_path[256] = {0};
        unsigned long offset;
        unsigned long inode;

        int n = sscanf(line, "%lx-%lx %4s %lx %11s %lu %255[^\n]",
                       &s, &e, perms, &offset, dev, &inode, map_path);
        if (n >= 6 && strstr(map_path, "/") && strstr(perms, "x")) {
            safe_strncpy(exe_path, map_path, sizeof(exe_path));
            break;
        }
    }

    rewind(f);

    memset(text, 0, sizeof(*text));
    memset(data, 0, sizeof(*data));
    memset(bss, 0, sizeof(*bss));
    memset(heap, 0, sizeof(*heap));
    memset(stack, 0, sizeof(*stack));

    int data_found = 0;

    while (fgets(line, sizeof(line), f)) {
        unsigned long s, e;
        char perms[5], dev[12], map_path[256] = {0};
        unsigned long offset;
        unsigned long inode;

        int n = sscanf(line, "%lx-%lx %4s %lx %11s %lu %255[^\n]",
                       &s, &e, perms, &offset, dev, &inode, map_path);
        if (n < 6) continue;

        /* Heap */
        if (strstr(map_path, "[heap]")) {
            heap->start = s;
            heap->size = (size_t)(e - s);
        }
        /* Stack */
        if (strstr(map_path, "[stack]")) {
            stack->start = s;
            stack->size = (size_t)(e - s);
        }

        /* Text segment: executable + r-xp */
        if (exe_path[0] && strstr(map_path, exe_path) && perms[0] == 'r' && perms[2] == 'x') {
            text->start = s;
            text->size = (size_t)(e - s);
        }

        /* Data/BSS: executable + rw- */
        if (exe_path[0] && strstr(map_path, exe_path) && perms[0] == 'r' && perms[1] == 'w') {
            if (!data_found) {
                data->start = s;
                data->size = (size_t)(e - s);
                data_found = 1;
            } else {
                bss->start = s;
                bss->size = (size_t)(e - s);
            }
        }
    }

    fclose(f);
    return 0;
}

int parse_proc_maps_libraries(LibraryInfo **libs_out, int *lib_count,
                              LibraryInfo *vdso_out, int *has_vdso)
{
    *libs_out = NULL;
    *lib_count = 0;
    *has_vdso = 0;

    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) {
        perror("fopen /proc/self/maps");
        return -1;
    }

    char line[512];
    int capacity = 8;
    LibraryInfo *libs = malloc(sizeof(LibraryInfo) * (size_t)capacity);
    if (!libs) {
        fclose(f);
        return -1;
    }

    while (fgets(line, sizeof(line), f)) {
        unsigned long s, e;
        char perms[5], dev[12], map_path[256] = {0};
        unsigned long offset;
        unsigned long inode;

        int n = sscanf(line, "%lx-%lx %4s %lx %11s %lu %255[^\n]",
                       &s, &e, perms, &offset, dev, &inode, map_path);
        if (n < 6) continue;

        size_t size_kb = (size_t)((e - s) / 1024);

        if (strstr(map_path, "[vdso]")) {
            if (vdso_out) {
                memset(vdso_out, 0, sizeof(*vdso_out));
                vdso_out->start = s;
                vdso_out->end = e;
                vdso_out->size_kb = size_kb;
                safe_strncpy(vdso_out->name, "[vdso]", sizeof(vdso_out->name));
                safe_strncpy(vdso_out->perms, perms, sizeof(vdso_out->perms));
                *has_vdso = 1;
            }
            continue;
        }

        if (strstr(map_path, ".so")) {
            if (*lib_count >= capacity) {
                capacity *= 2;
                LibraryInfo *new_libs = realloc(libs, sizeof(LibraryInfo) * (size_t)capacity);
                if (!new_libs) {
                    free(libs);
                    fclose(f);
                    return -1;
                }
                libs = new_libs;
            }
            LibraryInfo *info = &libs[*lib_count];
            memset(info, 0, sizeof(*info));
            info->start = s;
            info->end = e;
            info->size_kb = size_kb;
            safe_strncpy(info->name, map_path, sizeof(info->name));
            safe_strncpy(info->perms, perms, sizeof(info->perms));
            (*lib_count)++;
        }
    }

    fclose(f);
    *libs_out = libs;
    return 0;
}

int parse_proc_status_vm(VmStats *stats) {
    memset(stats, 0, sizeof(*stats));

    FILE *f = fopen("/proc/self/status", "r");
    if (!f) {
        perror("fopen /proc/self/status");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (starts_with(line, "VmSize:")) {
            sscanf(line, "VmSize: %zu kB", &stats->vm_size_kb);
        } else if (starts_with(line, "VmRSS:")) {
            sscanf(line, "VmRSS: %zu kB", &stats->vm_rss_kb);
        } else if (starts_with(line, "VmData:")) {
            sscanf(line, "VmData: %zu kB", &stats->vm_data_kb);
        } else if (starts_with(line, "VmStk:")) {
            sscanf(line, "VmStk: %zu kB", &stats->vm_stk_kb);
        } else if (starts_with(line, "VmExe:")) {
            sscanf(line, "VmExe: %zu kB", &stats->vm_exe_kb);
        }
    }

    fclose(f);
    return 0;
}
