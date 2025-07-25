#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define CONFIG_FILE "monitor.conf"
#define LOG_FILE "monitor.log"
#define PID_FILE "monitor.pid"
#define MAX_USERS 100

char *allowed_users[MAX_USERS];
int allowed_user_count = 0;
int polling_interval = 10;

void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\n') str++;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n')) *end-- = '\0';
}

int load_config() {
    FILE *file = fopen(CONFIG_FILE, "r");
    if (!file) return -1;

    char line[256];
    int in_block = 0;

    while (fgets(line, sizeof(line), file)) {
        trim(line);
        if (strcmp(line, "#start") == 0) {
            in_block = 1;
            continue;
        } else if (strcmp(line, "#end") == 0) {
            break;
        }

        if (in_block) {
            if (strncmp(line, "polling:", 8) == 0) {
                polling_interval = atoi(line + 8);
            } else if (strncmp(line, "users:[", 7) == 0) {
                char *start = line + 7;
                char *end = strchr(start, ']');
                if (end) *end = '\0';

                char *token = strtok(start, ",");
                while (token && allowed_user_count < MAX_USERS) {
                    trim(token);
                    allowed_users[allowed_user_count++] = strdup(token);
                    token = strtok(NULL, ",");
                }
            }
        }
    }

    fclose(file);
    return 0;
}

int is_allowed_user(const char *user) {
    for (int i = 0; i < allowed_user_count; i++) {
        if (strcmp(user, allowed_users[i]) == 0)
            return 1;
    }
    return 0;
}

void log_intruder(const char *user) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';

    fprintf(log, "[%s] Usuario no permitido detectado: %s\n", timestamp, user);
    fclose(log);
}

void kick_user(const char *user) {
    char command[128];
    snprintf(command, sizeof(command), "pkill -KILL -u %s", user);
    system(command);
}

void check_sessions() {
    FILE *fp = popen("who | awk '{print $1}'", "r");
    if (!fp) return;

    char user[64];
    while (fgets(user, sizeof(user), fp)) {
        trim(user);
        if (!is_allowed_user(user)) {
            log_intruder(user);
            kick_user(user);
        }
    }

    pclose(fp);
}

int main() {
    // Ejecutar monitor_conf
    system("./monitor_conf");

    if (load_config() != 0) {
        fprintf(stderr, "\x1b[31m[ERROR]\x1b[0m No se pudo cargar el archivo de configuración.\n");
        return 1;
    }

    FILE *pid = fopen(PID_FILE, "w");
    if (pid) {
        fprintf(pid, "%d\n", getpid());
        fclose(pid);
    }

    while (1) {
        check_sessions();
        sleep(polling_interval);
    }

    return 0;
}

