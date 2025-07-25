#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PID_FILE "fastmonitor.pid"
#define LOG_FILE "fastmonitor.log"

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void print_usage() {
    printf("Uso: fastmonitor [start | stop | status | log]\n");
}

void ensure_log_file_exists() {
    if (!file_exists(LOG_FILE)) {
        int fd = open(LOG_FILE, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) close(fd);
        else perror("Error: 'fastmonitor.log' not created");
    }
}

int main(int argc, char *argv[]) {
    //Verificar privilegios de root
    if (geteuid() != 0) {
        fprintf(stderr, "Permission denied.\n");
        return 1;
    }

    // Asegurar que el log existe
    ensure_log_file_exists();

    if (argc != 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "start") == 0) {
        if (file_exists(PID_FILE)) {
            printf("Restarting...\n");
            system("./fastmonitor-stop");
        } else {
            printf("Starting...\n");
        }

        int result = system("sudo ./fastmonitor-start &");
        if (result == -1) {
            perror("Error: Start error");
            return 1;
        } else {
            printf("FastMonitor started\n");
            return 0;
        }

    } else if (strcmp(argv[1], "stop") == 0) {
        system("sudo ./fastmonitor-stop");

    } else if (strcmp(argv[1], "status") == 0) {
        system("./fastmonitor-status");

    } else if (strcmp(argv[1], "log") == 0) {
        system("./fastmonitor-log");

    } else {
        print_usage();
        return 1;
    }

    return 0;
}
