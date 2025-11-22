#ifndef MEMORY_ANALYZER_H
#define MEMORY_ANALYZER_H
#include <stddef.h>

typedef struct {
    unsigned long start;
    unsigned long end;
    char perms[5];
    char pathname[256];
} MemoryMapping;

typedef struct {
    unsigned long start;
    size_t size;
} SegmentInfo;

typedef struct {
    char name[128];
    unsigned long start;
    unsigned long end;
    size_t size_kb;
    char perms[5];
} LibraryInfo;

typedef struct {
    size_t vm_size_kb;
    size_t vm_rss_kb;
    size_t vm_data_kb;
    size_t vm_stk_kb;
    size_t vm_exe_kb;
} VmStats;

int analyze_main_segments(void);
int list_all_segments(void);
int analyze_virtual_physical_memory(void);
int monitor_memory_usage(int interval_seconds);

#endif // MEMORY_ANALYZER_H
