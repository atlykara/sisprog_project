#include "leak_detector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Allocation {
    void *ptr;
    size_t size;
    const char *file;
    int line;
    int freed;
    struct Allocation *next;
} Allocation;

static Allocation *alloc_list = NULL;
static size_t total_allocs = 0;
static size_t total_freed = 0;

static void add_allocation(void *ptr, size_t size, const char *file, int line) {
    Allocation *node = (Allocation *)malloc(sizeof(Allocation));
    if (!node) return;
    node->ptr = ptr;
    node->size = size;
    node->file = file;
    node->line = line;
    node->freed = 0;
    node->next = alloc_list;
    alloc_list = node;
    total_allocs++;
}

static Allocation *find_allocation(void *ptr) {
    Allocation *cur = alloc_list;
    while (cur) {
        if (cur->ptr == ptr) return cur;
        cur = cur->next;
    }
    return NULL;
}

void *tracked_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "tracked_malloc: allocation failed (%zu bytes) at %s:%d\n",
                size, file, line);
        return NULL;
    }
    add_allocation(ptr, size, file, line);
    return ptr;
}

void tracked_free(void *ptr, const char *file, int line) {
    (void)file;
    (void)line;

    if (!ptr) return;

    Allocation *alloc = find_allocation(ptr);
    if (!alloc) {
        fprintf(stderr, "tracked_free: attempt to free unknown pointer %p at %s:%d\n",
                ptr, file, line);
        free(ptr);
        return;
    }

    if (alloc->freed) {
        fprintf(stderr, "tracked_free: double free detected for %p (originally at %s:%d)\n",
                ptr, alloc->file, alloc->line);
        return;
    }

    alloc->freed = 1;
    total_freed++;
    free(ptr);
}

void print_leak_report(void) {
    printf("Memory Leak Report:\n");
    printf("  Total allocations : %zu\n", total_allocs);
    printf("  Total freed       : %zu\n", total_freed);
    printf("  Leaked blocks     : %zu\n", total_allocs - total_freed);

    if (total_allocs == total_freed) {
        printf("\n  No memory leaks detected.\n");
        return;
    }

    size_t index = 1;
    Allocation *cur = alloc_list;
    while (cur) {
        if (!cur->freed) {
            printf("\n  Block #%zu: %zu bytes at %p\n", index, cur->size, cur->ptr);
            printf("    Allocated at: %s:%d\n", cur->file, cur->line);
            index++;
        }
        cur = cur->next;
    }
}
