#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define MAX_LINE    512

int check_monitor_process(char *out_pid, size_t pid_size, int verbose) {
    FILE *fp;
    char line[MAX_LINE];

    fp = popen("pgrep -f fastmonitor-start", "r");
    if (!fp) {
        if (verbose) printf("Status: Error checking process\n");
        return -1;
    }

    if (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0;
        strncpy(out_pid, line, pid_size - 1);
        pclose(fp);
        if (verbose) {
            printf("Status: " COLOR_GREEN "Running" COLOR_RESET " (PID: %s)\n", out_pid);
        }
        return 1;
    }

    pclose(fp);
    if (verbose) {
        printf("Status: Not running\n");
    }
    return 0;
}

int run_monitor_conf(int verbose) {
    FILE *fp;
    char line[MAX_LINE];
    int result = 0;

    fp = popen("./commands/fastmonitor-conf verbose", "r");
    if (!fp) {
        if (verbose) fprintf(stderr, "Error: Unable to run fastmonitor-conf\n");
        return 2;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (verbose) printf("%s", line);
    }

    int exit_code = pclose(fp);
    if (WIFEXITED(exit_code)) {
        result = WEXITSTATUS(exit_code);
    } else {
        result = 2;
    }

    return result;
}

int main(int argc, char *argv[]) {
    int verbose = argc > 1;
    char pid[16] = {0};

    int monitor_status = check_monitor_process(pid, sizeof(pid), verbose);
    int conf_status = run_monitor_conf(verbose);

    if (conf_status != 0) return 2;
    if (monitor_status == 1) return 0;
    return 1;
}
