#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

static void safe_input_example(void) {
    char buf[32];
    printf("Enter your name: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\n")] = '\0';      // remove newline
        printf("Hello %s\n", buf);           // safe: constant format string
    }
}

static void safe_string_example(void) {
    char dest[32];

    // Safe copy (bounded)
    strncpy(dest, "fixed string", sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';

    // Safe concatenation (bounded)
    strncat(dest, " tail", sizeof(dest) - strlen(dest) - 1);
}

static void safe_allocation_example(void) {
    size_t count = 100;
    size_t element_size = sizeof(int);

    /* Overflow check BEFORE multiplication */
    if (count > SIZE_MAX / element_size) {
        // would overflow, handle error (log, return, etc.)
        return;
    }

    /* Multiply into a separate variable */
    size_t total_size = count * element_size;

    int *arr = (int *)malloc(total_size);
    if (arr) {
        /* use arr safely */
        free(arr);
    }
}


// gmtime(), localtime(), ctime(), asctime()
static void safe_time_examples(void) {
    time_t t = time(NULL);

    struct tm lt_buf;
    struct tm gm_buf;
    char cbuf[26];  // ctime_r typically needs at least 26 bytes
    char abuf[26];  // asctime_r typically needs at least 26 bytes

    // 1) localtime_r - safe alternative to localtime()
    if (localtime_r(&t, &lt_buf)) {
        char tbuf[64];
        if (strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", &lt_buf)) {
            printf("localtime_r (safe): %s\n", tbuf);
        }
    }

    // 2) gmtime_r - safe alternative to gmtime()
    if (gmtime_r(&t, &gm_buf)) {
        char tbuf[64];
        if (strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S (UTC)", &gm_buf)) {
            printf("gmtime_r (safe): %s\n", tbuf);
        }
    }

    // 3) ctime_r - safe alternative to ctime()
    if (ctime_r(&t, cbuf)) {
        printf("ctime_r (safe): %s", cbuf);
    }

    // 4) asctime_r - safe alternative to asctime()
    if (asctime_r(&lt_buf, abuf)) {
        printf("asctime_r (safe): %s", abuf);
    }
}

int main(void) {
    safe_input_example();
    safe_string_example();
    safe_allocation_example();
    safe_time_examples();
    return 0;
}
