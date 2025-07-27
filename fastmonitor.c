#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define LOG_FILE "fastmonitor.log"

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

void ensure_log_file_exists() {
    if (!file_exists(LOG_FILE)) {
        int fd = open(LOG_FILE, O_CREAT | O_WRONLY, 0644);
        if (fd >= 0) {
            close(fd);
        } else {
            perror("Error: Unable to create 'fastmonitor.log'");
        }
    }
}

void print_usage() {
    printf("Usage: fastmonitor [start | stop | status | log]\n");
}

int main(int argc, char *argv[]) {
    if (geteuid() != 0) {
        fprintf(stderr, "Error: Administrator privileges are required to execute Fast Monitor.\n");
        return EXIT_FAILURE;
    }

    ensure_log_file_exists();

    if (argc != 2) {
        print_usage();
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "start") == 0) {
        int status_code = system("./commands/fastmonitor-status > /dev/null");

        int code = 2;
        if (WIFEXITED(status_code)) {
            code = WEXITSTATUS(status_code);
        }

        if (code == 2) {
            fprintf(stderr, "Error: Configuration error. Cannot start Fast Monitor.\n");
            return EXIT_FAILURE;
        }

        if (code == 0) {
            printf("Fast Monitor is running. Restarting service...\n");
            system("./commands/fastmonitor-stop");
        } else if (code == 1) {
            printf("Fast Monitor is not running. Starting service...\n");
        }

        int result = system("sudo ./commands/fastmonitor-start &");
        if (result == -1) {
            perror("Error: Failed to start Fast Monitor");
            return EXIT_FAILURE;
        } else {
            printf("Fast Monitor has been started successfully.\n");
            return EXIT_SUCCESS;
        }

    } else if (strcmp(argv[1], "stop") == 0) {
        printf("Stopping Fast Monitor...\n");
        system("sudo ./commands/fastmonitor-stop");

    } else if (strcmp(argv[1], "status") == 0) {
        system("./commands/fastmonitor-status verbose");

    } else if (strcmp(argv[1], "log") == 0) {
        system("./commands/fastmonitor-log");

    } else {
        print_usage();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
