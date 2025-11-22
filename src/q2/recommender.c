#include "recommender.h"
#include <stdio.h>

void print_recommendation(const CodeIssue *issue) {
    switch (issue->type) {
    case ISSUE_UNSAFE_GETS:
        printf("gets() does not check the length of the input, which can easily cause buffer overflows.\n");
        printf("It can lead to crashes, data corruption, or serious security vulnerabilities. \n");
        printf("This function was removed from the C11 standard because it is \n\n");
        printf("Suggestion: Use fgets() instead of gets().\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    gets(buffer);\n");
        printf("    // Safer:\n");
        printf("    fgets(buffer, sizeof(buffer), stdin);\n");
        printf("    buffer[strcspn(buffer, \"\\n\")] = '\\0'; // remove newline\n\n");
        break;

    case ISSUE_UNSAFE_STRCPY:
        printf("strcpy() does not check the destination buffer size.\n");
        printf("If the source string is longer than the destination, it will cause a buffer overflow.\n");
        printf("Leading to crashes, data corruption, or security vulnerabilities. This function isconsidered unsafe for handling untrusted input.\n\n");
        printf("Suggestion: Use strncpy() instead of strcpy().\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    strcpy(dest, src);\n");
        printf("    // Safer:\n");
        printf("    strncpy(dest, src, sizeof(dest) - 1);\n");
        printf("    dest[sizeof(dest) - 1] = '\\0';\n\n");
        break;

    case ISSUE_UNSAFE_STRCAT:
        printf("strcat() appends data without checking remaining space in the destination buffer.\n");
        printf("It can easily overflow the buffer if you concatenate long strings.\n\n");
        printf("Suggestion: Use strncat() or safer string-building APIs.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    strcat(dest, src);\n");
        printf("    // Safer:\n");
        printf("    strncat(dest, src, sizeof(dest) - strlen(dest) - 1);\n\n");
        break;

    case ISSUE_UNSAFE_SPRINTF:
        printf("sprintf() writes formatted data to a buffer but does not know its size.\n");
        printf("A long result will overflow the buffer.\n\n");
        printf("Suggestion: Use snprintf() and always pass the buffer size.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    sprintf(buf, \"Hello %%s\", name);\n");
        printf("    // Safer:\n");
        printf("    snprintf(buf, sizeof(buf), \"Hello %%s\", name);\n\n");
        break;

    case ISSUE_UNSAFE_SCANF:
        printf("scanf() with %%s or similar formats reads arbitrary-length input into fixed buffers.\n");
        printf("Without field width, this is vulnerable to buffer overflow.\n\n");
        printf("Suggestion: Use width specifiers or fgets() + sscanf().\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    scanf(\"%%s\", buffer);\n");
        printf("    // Safer (width limited):\n");
        printf("    scanf(\"%%31s\", buffer); // if buffer is 32 bytes\n");
        printf("    // Or:\n");
        printf("    fgets(buffer, sizeof(buffer), stdin);\n");
        printf("    sscanf(buffer, \"%%31s\", buffer);\n\n");
        break;

        case ISSUE_UNSAFE_GMTIME:
        printf("gmtime() returns a pointer to a static, shared struct tm.\n");
        printf("It is not thread-safe and the result can be overwritten by later calls.\n\n");
        printf("Suggestion: Use gmtime_r() and provide your own struct tm buffer.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    struct tm *tm_ptr = gmtime(&t);\n");
        printf("\n");
        printf("    // Safe (POSIX):\n");
        printf("    struct tm tm_buf;\n");
        printf("    struct tm *tm_ptr = gmtime_r(&t, &tm_buf);\n\n");
        break;

    case ISSUE_UNSAFE_LOCALTIME:
        printf("localtime() returns a pointer to a static, shared struct tm.\n");
        printf("It is not thread-safe and the result can be overwritten by later calls.\n\n");
        printf("Suggestion: Use localtime_r() and provide your own struct tm buffer.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    struct tm *tm_ptr = localtime(&t);\n");
        printf("\n");
        printf("    // Safe (POSIX):\n");
        printf("    struct tm tm_buf;\n");
        printf("    struct tm *tm_ptr = localtime_r(&t, &tm_buf);\n\n");
        break;

    case ISSUE_UNSAFE_CTIME:
        printf("ctime() returns a pointer to a static, shared string buffer.\n");
        printf("It is not thread-safe and the result can be overwritten by later calls.\n\n");
        printf("Suggestion: Use ctime_r() and provide your own character buffer.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    char *s = ctime(&t);\n");
        printf("\n");
        printf("    // Safe (POSIX):\n");
        printf("    char buf[26]; // ctime_r() typically needs at least 26 bytes\n");
        printf("    char *s = ctime_r(&t, buf);\n");
        printf("    if (s) {\n");
        printf("        // use buf safely\n");
        printf("    }\n\n");
        break;

    case ISSUE_UNSAFE_ASCTIME:
        printf("asctime() returns a pointer to a static, shared string buffer.\n");
        printf("It is not thread-safe and the result can be overwritten by later calls.\n\n");
        printf("Suggestion: Use asctime_r() and provide your own character buffer.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    char *s = asctime(tm_ptr);\n");
        printf("\n");
        printf("    // Safe (POSIX):\n");
        printf("    char buf[26]; // asctime_r() typically needs at least 26 bytes\n");
        printf("    char *s = asctime_r(tm_ptr, buf);\n");
        printf("    if (s) {\n");
        printf("        // use buf safely\n");
        printf("    }\n\n");
        break;


    case ISSUE_FORMAT_STRING:
        printf("Passing user-controlled data as the format string to printf-family functions is dangerous.\n");
        printf("An attacker can inject %%n or other format specifiers and read or write memory.\n\n");
        printf("Suggestion: Always use a constant format string and pass user data as arguments.\n");
        printf("  Example:\n");
        printf("    // Unsafe:\n");
        printf("    printf(user_input);\n");
        printf("    // Safer:\n");
        printf("    printf(\"%%s\", user_input);\n\n");
        break;

    case ISSUE_COMMAND_INJECTION:
        printf("system()/popen() execute shell commands. If the command is built from user input,\n");
        printf("the user can inject extra shell syntax (e.g., '; rm -rf /').\n\n");
        printf("Suggestion: Avoid passing raw strings to the shell. Use execve()/posix_spawn() with\n");
        printf("fixed argument arrays instead of shell command strings.\n\n");
        break;

    case ISSUE_INTEGER_OVERFLOW:
        printf("Multiplying count * size before malloc() can overflow size_t.\n");
        printf("If overflow happens, malloc() allocates a smaller buffer than expected,\n");
        printf("and later writes can corrupt memory.\n\n");
        printf("Suggestion: Check for overflow before multiplication.\n");
        printf("  Example:\n");
        printf("    if (count > SIZE_MAX / size) {\n");
        printf("        // overflow\n");
        printf("        return NULL;\n");
        printf("    }\n");
        printf("    ptr = malloc(count * size);\n\n");
        break;
    }
}
