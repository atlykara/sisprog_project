#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void run_command(char *cmd) {
    system(cmd);               //command injection (unsafe)
    FILE *p = popen(cmd, "r"); //command injection (unsafe)
    if (p) pclose(p);
}

void time_unsafe_examples(void) {
    time_t t = time(NULL);

    // 1) localtime() - UNSAFE
    struct tm *lt = localtime(&t);
    if (lt) {
        printf("localtime (unsafe): %02d:%02d\n", lt->tm_hour, lt->tm_min);
    }

    // 2) gmtime() - UNSAFE
    struct tm *gm = gmtime(&t);
    if (gm) {
        printf("gmtime (unsafe): %02d:%02d\n", gm->tm_hour, gm->tm_min);
    }

    // 3) ctime() - UNSAFE
    char *ct = ctime(&t);
    if (ct) {
        printf("ctime (unsafe): %s", ct); // prints raw static buffer
    }

    // 4) asctime() - UNSAFE
    // using lt here, but it could be any struct tm*
    char *asc = asctime(lt);
    if (asc) {
        printf("asctime (unsafe): %s", asc);
    }
}

void log_time(struct tm *tm_ptr) {
    char buf[64];
    // also unsafe sprintf example
    
    sprintf(buf, "Time: %d:%d", tm_ptr->tm_hour, tm_ptr->tm_min); // sprintf() unsafe
    printf("%s\n", buf);
}

int main(int argc, char *argv[]) {
    char *user_input = (argc > 1) ? argv[1] : "ls";

    //Format string vulnerabilities (unsafe)
    printf(user_input);          //unsafe
    fprintf(stdout, user_input); //unsafe

    //Integer overflow example (unsafe)
    size_t count = 1024 * 1024;
    size_t size = 1024 * 1024;
    void *p = malloc(count * size); //potential integer overflow if not checked
    free(p);

    //Time-related unsafe examples
    time_unsafe_examples();

    //Use one of the unsafe tm* in another function
    time_t t = time(NULL);
    struct tm *lt = localtime(&t); //unsafe
    if (lt) {
        log_time(lt);
    }

    //Command injection examples (unsafe)
    run_command(user_input);

    return 0;
}
