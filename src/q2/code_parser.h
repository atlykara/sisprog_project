#ifndef CODE_PARSER_H
#define CODE_PARSER_H

typedef enum {
    ISSUE_UNSAFE_STRCPY,
    ISSUE_UNSAFE_STRCAT,
    ISSUE_UNSAFE_SPRINTF,
    ISSUE_UNSAFE_GETS,
    ISSUE_UNSAFE_SCANF,
    ISSUE_UNSAFE_GMTIME,
    ISSUE_UNSAFE_LOCALTIME,
    ISSUE_UNSAFE_CTIME,
    ISSUE_UNSAFE_ASCTIME,
    ISSUE_FORMAT_STRING,
    ISSUE_COMMAND_INJECTION,
    ISSUE_INTEGER_OVERFLOW
} IssueType;

typedef struct {
    IssueType type;
    char function_name[32];
    int line;
} CodeIssue;

// Basic checks (unsafe functions)
int analyze_file_basic(const char *filename, CodeIssue **issues_out, int *issue_count_out);

// Extended checks (format strings, command injection, integer overflows)
int analyze_file_extended(const char *filename, CodeIssue **issues_out, int *issue_count_out);

#endif // CODE_PARSER_H
