#include <stdio.h>

/**
 * @brief Read from stdin example
 */
int main(int argc, char const *argv[]) {
    printf("====================================\n");
    char buff[1024];
    int ii = 0; // line number
    while (fgets(buff, sizeof(buff), stdin) != NULL) {
        // buff does contain line content until \0
        printf("%d %s", ++ii, buff);
    }
    printf("====================================\n");
    return 0;
}