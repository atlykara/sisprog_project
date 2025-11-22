#ifndef LEAK_DETECTOR_H
#define LEAK_DETECTOR_H
#include <stddef.h>

void *tracked_malloc(size_t size, const char *file, int line);
void tracked_free(void *ptr, const char *file, int line);
void print_leak_report(void);

#define MALLOC(size) tracked_malloc((size), __FILE__, __LINE__)
#define FREE(ptr)    tracked_free((ptr), __FILE__, __LINE__)

#endif // LEAK_DETECTOR_H
