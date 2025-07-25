#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define COLOR_RED   "\033[1;31m"
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void show_usage() {
    printf(COLOR_RED "Uso incorrecto.\n" COLOR_RESET);
    printf("Uso: ./monitor [start | stop | status | log]\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        show_usage();
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "start") == 0) {
        if (file_exists("monitor.pid")) {
            printf(COLOR_RED "Error: monitor ya está en ejecución (monitor.pid existe).\n" COLOR_RESET);
            return 1;
        }
        // Ejecutar monitor_start en segundo plano
        if (fork() == 0) {
            execl("./monitor_start", "monitor_start", NULL);
            perror("Error al ejecutar monitor_start");
            exit(1);
        } else {
            printf(COLOR_GREEN "Monitor iniciado en segundo plano.\n" COLOR_RESET);
        }

    } else if (strcmp(cmd, "stop") == 0) {
        if (!file_exists("monitor.pid")) {
            printf(COLOR_RED "Error: no hay proceso monitor en ejecución (monitor.pid no existe).\n" COLOR_RESET);
            return 1;
        }
        system("./monitor_stop");

    } else if (strcmp(cmd, "status") == 0) {
        system("./monitor_status");

    } else if (strcmp(cmd, "log") == 0) {
        system("./monitor_log");

    } else {
        show_usage();
        return 1;
    }

    return 0;
}

