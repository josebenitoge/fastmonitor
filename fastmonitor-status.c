#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define MAX_LINE    512

void check_monitor_process() {
    FILE *pid_file = fopen("fastmonitor.pid", "r");
    char pid[16] = {0};

    if (!pid_file) {
        printf("Status: Off\n");
        return;
    }

    if (!fgets(pid, sizeof(pid), pid_file)) {
        printf("Status: Off. Process corrupted\n");
        fclose(pid_file);
        return;
    }

    pid[strcspn(pid, "\n")] = 0; // Eliminar salto de línea
    fclose(pid_file);

    char cmd[MAX_LINE];
    snprintf(cmd, sizeof(cmd), "ps -p %s > /dev/null", pid);

    if (system(cmd) == 0) {
        printf("Status: " COLOR_GREEN "Running" COLOR_RESET " (PID: %s)\n", pid);
    } else {
        printf("Status: Off. Unexpected state\n");
    }
}

void run_monitor_conf() {
    FILE *fp;
    char line[MAX_LINE];

    fp = popen("./fastmonitor-conf", "r");
    if (!fp) {
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line); // monitor-conf ya incluye color
    }

    pclose(fp);
}

int main() {
    check_monitor_process();
    run_monitor_conf();
    return 0;
}
