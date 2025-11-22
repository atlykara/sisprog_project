#ifndef PROC_PARSER_H
#define PROC_PARSER_H
#include "memory_analyzer.h"

int parse_proc_maps_for_segments(SegmentInfo *text,
                                 SegmentInfo *data,
                                 SegmentInfo *bss,
                                 SegmentInfo *heap,
                                 SegmentInfo *stack);

int parse_proc_maps_libraries(LibraryInfo **libs_out, int *lib_count,
                              LibraryInfo *vdso_out, int *has_vdso);

int parse_proc_status_vm(VmStats *stats);

#endif // PROC_PARSER_H
