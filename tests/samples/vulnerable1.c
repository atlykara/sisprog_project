#include <stdio.h>
#include <string.h>

void foo(char *user_input) {
    char buf1[16];
    char buf2[32];
    char out[64];

    gets(buf1);                // gets()
    strcpy(buf2, user_input);  // strcpy()
    strcat(buf2, " suffix");   // strcat()
    sprintf(out, "%s %s", buf1, buf2); // sprintf()
}

int main(void) {
    char name[32];
    scanf("%s", name); // scanf()
    foo(name);
    return 0;
}
