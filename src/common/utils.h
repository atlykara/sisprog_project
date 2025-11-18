#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void trim_newline(char *s);
void safe_strncpy(char *dest, const char *src, size_t dest_size);
char *read_entire_file(const char *path);
int starts_with(const char *str, const char *prefix);

#endif // UTILS_H
