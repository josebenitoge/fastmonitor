#include <stdio.h>
#include <string.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_GREEN   "\x1b[32m"
int main() {
    FILE *file = fopen("fastmonitor.log", "r");
    if (!file) {
        perror("'fastmonitor.log' not found");
        return 1;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "ERROR")) {
            printf(COLOR_RED "%s" COLOR_RESET, line);
        } else if (strstr(line, "WARN")) {
            printf(COLOR_YELLOW "%s" COLOR_RESET, line);
        } else if (strstr(line, "INFO")) {
            printf(COLOR_GREEN "%s" COLOR_RESET, line);
        } else {
            printf("%s", line);
        }
    }
    fclose(file);
    return 0;
}