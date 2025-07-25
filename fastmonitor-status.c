#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"
#define MAX_CMD     256
#define MAX_LINE    512

void check_monitor_process() {
    FILE *fp;
    char path[MAX_LINE];
    char pid[16] = {0};
    int running = 0;

    // Buscar el proceso por nombre exacto
    fp = popen("pgrep -x monitor_start", "r");
    if (fp == NULL) {
        printf("Error al verificar el proceso.\n");
        return;
    }

    if (fgets(pid, sizeof(pid), fp) != NULL) {
        running = 1;
        pid[strcspn(pid, "\n")] = 0; // Eliminar salto de línea
    }

    pclose(fp);

    if (running) {
        printf(COLOR_GREEN "Running" COLOR_RESET " (PID: %s)\n", pid);
    } else {
        printf("Off\n");
    }
}

void run_monitor_conf() {
    FILE *fp;
    char line[MAX_LINE];

    fp = popen("./monitor_conf", "r");
    if (!fp) {
        printf("Error al ejecutar monitor_conf\n");
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line); // monitor_conf ya incluye color
    }

    pclose(fp);
}

int main() {
    check_monitor_process();
    run_monitor_conf();
    return 0;
}

