#include "security_checker.h"
#include "code_parser.h"
#include "recommender.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>

static const char *issue_type_to_string(const CodeIssue *issue) {
    return issue->function_name; // Return function name as string representation
}

// -s mod
void run_scan_mode(char **files, int file_count) {
    int total = 0;

    for (int i = 0; i < file_count; ++i) {
        const char *fname = files[i];
        CodeIssue *issues = NULL;
        int count = 0;

        if (analyze_file_basic(fname, &issues, &count) != 0) {
            fprintf(stderr, "Failed to analyze %s\n", fname);
            continue;
        }

        printf("File: %s\n", fname);
        for (int j = 0; j < count; ++j) {
            printf("Line %d: %s used\n", issues[j].line,
                   issue_type_to_string(&issues[j]));
        }
        printf("\n");
        total += count;
        free(issues);
    }

    printf("Total %d security issues found.\n", total);
}

// -r mod
void run_recommendation_mode(char **files, int file_count) {
    for (int i = 0; i < file_count; ++i) {
        const char *fname = files[i];
        CodeIssue *issues = NULL;
        int count = 0;

        if (analyze_file_basic(fname, &issues, &count) != 0) {
            fprintf(stderr, "Failed to analyze %s\n", fname);
            continue;
        }

        printf("File: %s\n\n", fname);

        for (int j = 0; j < count; ++j) {
            const CodeIssue *iss = &issues[j];
            printf("Line %d: %s used\n", iss->line, issue_type_to_string(iss));
            print_recommendation(iss);
        }

        if (count == 0) {
            printf("No unsafe functions detected in %s.\n\n", fname);
        }

        free(issues);
    }
}

// -x mod
void run_extended_mode(char **files, int file_count) {
    int total = 0;

    for (int i = 0; i < file_count; ++i) {
        const char *fname = files[i];
        CodeIssue *issues = NULL;
        int count = 0;

        if (analyze_file_extended(fname, &issues, &count) != 0) {
            fprintf(stderr, "Failed to analyze %s\n", fname);
            continue;
        }

        printf("File: %s\n", fname);
        for (int j = 0; j < count; ++j) {
            const CodeIssue *iss = &issues[j];
            switch (iss->type) {
            case ISSUE_FORMAT_STRING:
                printf("Line %d: Potential format string vulnerability in %s\n",
                       iss->line, iss->function_name);
                break;
            case ISSUE_COMMAND_INJECTION:
                printf("Line %d: Potential command injection via %s\n",
                       iss->line, iss->function_name);
                break;
            case ISSUE_INTEGER_OVERFLOW:
                printf("Line %d: Potential integer overflow before %s\n",
                       iss->line, iss->function_name);
                break;
            default:
                printf("Line %d: Security issue in %s\n",
                    iss->line, iss->function_name);
                break;
            }
        }
        printf("\n");
        total += count;
        free(issues);
    }

    printf("Total %d extended security issues found.\n", total);
}
