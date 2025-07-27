#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COLOR_RED   "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define MAX_LINE    256

int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

int is_valid_users_line(const char *line) {
    return starts_with(line, "users:[") && strchr(line, ']') != NULL;
}

void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

int main(int argc, char *argv[]) {
    FILE *file = fopen("fastmonitor.conf", "r");
    if (!file) {
        fprintf(stderr, COLOR_RED "ERROR: Configuration file 'fastmonitor.conf' not found.\n" COLOR_RESET);
        return 1;
    }

    char line[MAX_LINE];
    int valid_polling = 0, valid_users = 0;
    int polling_value = -1;
    char users_line[MAX_LINE] = {0};

    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        trim(line);

        if (starts_with(line, "polling:")) {
            char *number = line + strlen("polling:");
            while (isspace(*number)) number++;
            if (*number && strspn(number, "0123456789") == strlen(number)) {
                polling_value = atoi(number);
                valid_polling = 1;
            } else {
                valid_polling = 0;
            }
        } else if (is_valid_users_line(line)) {
            strncpy(users_line, line, sizeof(users_line) - 1);
            valid_users = 1;
        }
    }

    fclose(file);

    if (argc > 1) {
        printf("📘 FastMonitor Configuration Check\n");

        if (valid_polling && valid_users) {
            printf(COLOR_GREEN "✓ Configuration is valid.\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "✗ Configuration is invalid.\n" COLOR_RESET);
        }

        if (valid_polling) {
            printf("• Polling interval: %d seconds\n", polling_value);
        } else {
            printf(COLOR_RED "ERROR: Invalid or missing polling value.\n" COLOR_RESET);
        }

        if (valid_users) {
            char *start = strchr(users_line, '[');
            char *end = strchr(users_line, ']');
            if (start && end && end > start) {
                *end = '\0';
                printf("• Authorized users:");
                char *token = strtok(start + 1, ",");
                while (token) {
                    trim(token);
                    printf(" %s", token);
                    token = strtok(NULL, ",");
                    if (token) printf(",");
                }
                printf("\n");
            }
        } else {
            printf(COLOR_RED "ERROR: Invalid or missing users list.\n" COLOR_RESET);
        }

    } else {
        // Silent mode: only report errors
        if (!valid_polling) {
            fprintf(stderr, COLOR_RED "ERROR: Invalid or missing polling value.\n" COLOR_RESET);
            return 1;
        }
        if (!valid_users) {
            fprintf(stderr, COLOR_RED "ERROR: Invalid or missing users list.\n" COLOR_RESET);
            return 1;
        }
        // If valid, remain silent
    }

    return 0;
}
