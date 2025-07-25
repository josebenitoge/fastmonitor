#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COLOR_RED   "\033[1;31m"
#define COLOR_RESET "\033[0m"
#define MAX_LINE    256

int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

int is_valid_users_line(const char *line) {
    return starts_with(line, "users:[") && strchr(line, ']') != NULL;
}

int main() {
    FILE *file = fopen("fastmonitor.conf", "r");
    if (!file) {
        printf(COLOR_RED "ERROR: 'fastmonitor.conf' not found.\n" COLOR_RESET);
        return 1;
    }

    char line[MAX_LINE];
    int found_start = 0, found_polling = 0, found_users = 0, found_end = 0;

    while (fgets(line, sizeof(line), file)) {
        // Eliminar salto de línea
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "#start") == 0) {
            found_start = 1;
        } else if (starts_with(line, "polling:")) {
            char *number = line + strlen("polling:");
            while (isspace(*number)) number++;
            if (*number && strspn(number, "0123456789") == strlen(number)) {
                found_polling = 1;
            } else {
                printf(COLOR_RED "ERROR: Polling value not valid.\n" COLOR_RESET);
                fclose(file);
                return 1;
            }
        } else if (is_valid_users_line(line)) {
            found_users = 1;
        } else if (strcmp(line, "#end") == 0) {
            found_end = 1;
        }
    }

    fclose(file);

    if (!found_start) {
        printf(COLOR_RED "ERROR: Missing '#start' line.\n" COLOR_RESET);
        return 1;
    }
    if (!found_polling) {
        printf(COLOR_RED "ERROR: Line 'polling:<int>'.\n" COLOR_RESET);
        return 1;
    }
    if (!found_users) {
        printf(COLOR_RED "ERROR: Line 'users:[...]'.\n" COLOR_RESET);
        return 1;
    }
    if (!found_end) {
        printf(COLOR_RED "ERROR: Missing '#end' line.\n" COLOR_RESET);
        return 1;
    }

    //printf("Archivo de configuración válido ✅\n");
    return 0;
}
