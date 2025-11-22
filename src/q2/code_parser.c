#include "code_parser.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int is_identifier_char(char c) {
    return (c == '_' ||
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9'));
}

//check if the line is commented
static int is_commented_line(const char *line) {
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t') i++;
    if (line[i] == '/' && line[i + 1] == '/')
        return 1;
    return 0;
}

//search for function call in the line
static int contains_function_call(const char *line, const char *name) {
    size_t n = strlen(name);
    int in_string = 0;  //is

    for (size_t i = 0; line[i] != '\0'; ++i) {
        char c = line[i];

        // check for string literals " ... "
        if (c == '"' && (i == 0 || line[i - 1] != '\\')) {
            in_string = !in_string;
            continue;
        }

        if (in_string) {
            //"... function name ... " -- ignore
            continue;
        }

        // check for function name
        if (strncmp(&line[i], name, n) == 0) {
            char before = (i == 0) ? ' ' : line[i - 1];
            char after = line[i + n];

            if (!is_identifier_char(before) && !is_identifier_char(after)) {
                const char *q = &line[i + n];
                while (*q == ' ' || *q == '\t') q++;
                if (*q == '(') {
                    return 1;
                }
            }
        }
    }
    return 0;
}


// Add a new issue to the issues array
static void add_issue(CodeIssue **issues, int *count, int *capacity,
                      IssueType type, const char *func_name, int line) {
    if (*capacity == 0) {
        *capacity = 8;
        *issues = (CodeIssue *)malloc(sizeof(CodeIssue) * (size_t)(*capacity));
        if (!*issues) {
            *capacity = 0;
            return;
        }
    } else if (*count >= *capacity) {
        *capacity *= 2;
        CodeIssue *tmp = (CodeIssue *)realloc(*issues, sizeof(CodeIssue) * (size_t)(*capacity));
        if (!tmp) {
            return;
        }
        *issues = tmp;
    }

    CodeIssue *ci = &(*issues)[*count];
    ci->type = type;
    ci->line = line;
    strncpy(ci->function_name, func_name, sizeof(ci->function_name) - 1);
    ci->function_name[sizeof(ci->function_name) - 1] = '\0';
    (*count)++;
}

/* -------- BASIC ISSUES: UNSAFE FUNCTIONS -------- */

static void detect_basic_issues_in_line(const char *line, int line_no,
                                        CodeIssue **issues, int *count, int *capacity) {
    if (is_commented_line(line)) return;

    if (contains_function_call(line, "strcpy")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_STRCPY, "strcpy()", line_no);
    }
    if (contains_function_call(line, "strcat")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_STRCAT, "strcat()", line_no);
    }
    if (contains_function_call(line, "sprintf")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_SPRINTF, "sprintf()", line_no);
    }
    if (contains_function_call(line, "gets")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_GETS, "gets()", line_no);
    }

    //check for scanf usage
    const char *p = strstr(line, "scanf");
    if (p && !is_identifier_char(p[-1]) && !is_identifier_char(p[5])) {
        if (contains_function_call(line, "scanf")) {
            add_issue(issues, count, capacity, ISSUE_UNSAFE_SCANF, "scanf()", line_no);
        }
    }
    //time-related unsafe functions
    if (contains_function_call(line, "gmtime")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_GMTIME, "gmtime()", line_no);
    }
    if (contains_function_call(line, "localtime")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_LOCALTIME, "localtime()", line_no);
    }
    if (contains_function_call(line, "ctime")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_CTIME, "ctime()", line_no);
    }
    if (contains_function_call(line, "asctime")) {
        add_issue(issues, count, capacity, ISSUE_UNSAFE_ASCTIME, "asctime()", line_no);
    }
}
// Analyze a file for basic unsafe function usage
int analyze_file_basic(const char *filename, CodeIssue **issues_out, int *issue_count_out) {
    *issues_out = NULL;
    *issue_count_out = 0;

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }

    char line[1024];
    int line_no = 0;
    int capacity = 0;
    CodeIssue *issues = NULL;

    while (fgets(line, sizeof(line), f)) {
        line_no++;
        detect_basic_issues_in_line(line, line_no, &issues, issue_count_out, &capacity);
    }

    fclose(f);
    *issues_out = issues;
    return 0;
}

/* -------- EXTENDED ISSUES -------- */

static void detect_format_string(const char *line, int line_no,
                                 CodeIssue **issues, int *count, int *capacity) {
    if (is_commented_line(line)) return;

    /* printf */
    const char *p = strstr(line, "printf");
    if (p && !strstr(line, "fprintf")) {
        const char *q = strchr(p, '(');
        if (q) {
            q++;
            while (*q == ' ' || *q == '\t') q++;
            if (*q != '"' && *q != '\0' && *q != ')') {
                add_issue(issues, count, capacity, ISSUE_FORMAT_STRING, "printf()", line_no);
            }
        }
    }

    /* fprintf */
    p = strstr(line, "fprintf");
    if (p) {
        const char *q = strchr(p, '(');
        if (q) {
            //move past '('
            q++;
            //skip first argument
            int paren_level = 1;
            while (*q && paren_level > 0) {
                if (*q == '(') paren_level++;
                else if (*q == ')') paren_level--;
                else if (*q == ',' && paren_level == 1) {
                    q++; // move past ','
                    while (*q == ' ' || *q == '\t') q++;
                    if (*q != '"' && *q != '\0' && *q != ')') {
                        add_issue(issues, count, capacity, ISSUE_FORMAT_STRING, "fprintf()", line_no);
                    }
                    break;
                }
                q++;
            }
        }
    }
}

static void detect_command_injection(const char *line, int line_no,
                                     CodeIssue **issues, int *count, int *capacity) {
    if (is_commented_line(line)) return;

    if (contains_function_call(line, "system")) {
        add_issue(issues, count, capacity, ISSUE_COMMAND_INJECTION, "system()", line_no);
    }
    if (contains_function_call(line, "popen")) {
        add_issue(issues, count, capacity, ISSUE_COMMAND_INJECTION, "popen()", line_no);
    }
}

static void detect_integer_overflow(const char *line, int line_no,
                                    CodeIssue **issues, int *count, int *capacity) {
    if (is_commented_line(line)) return;

    // Look for malloc calls with multiplication in the argument
    const char *p = strstr(line, "malloc");
    if (!p) return;

    const char *open = strchr(p, '(');
    const char *close = strchr(p, ')');
    if (!open || !close || close <= open) return;

    char buf[256];
    size_t len = (size_t)(close - open - 1);
    if (len >= sizeof(buf)) len = sizeof(buf) - 1;
    memcpy(buf, open + 1, len);
    buf[len] = '\0';

    if (strchr(buf, '*') && !strstr(line, "SIZE_MAX")) {
        add_issue(issues, count, capacity, ISSUE_INTEGER_OVERFLOW, "malloc()", line_no);
    }
}

static void detect_extended_issues_in_line(const char *line, int line_no,
                                           CodeIssue **issues, int *count, int *capacity) {
    detect_format_string(line, line_no, issues, count, capacity);
    detect_command_injection(line, line_no, issues, count, capacity);
    detect_integer_overflow(line, line_no, issues, count, capacity);
}

int analyze_file_extended(const char *filename, CodeIssue **issues_out, int *issue_count_out) {
    *issues_out = NULL;
    *issue_count_out = 0;

    FILE *f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }

    char line[1024];
    int line_no = 0;
    int capacity = 0;
    CodeIssue *issues = NULL;

    while (fgets(line, sizeof(line), f)) {
        line_no++;
        detect_extended_issues_in_line(line, line_no, &issues, issue_count_out, &capacity);
    }

    fclose(f);
    *issues_out = issues;
    return 0;
}
