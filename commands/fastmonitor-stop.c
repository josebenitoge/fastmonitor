#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_LINE 256

int main() {
    FILE *fp;
    char line[MAX_LINE];
    pid_t pid;
    fp = popen("pgrep -f fastmonitor-start", "r");
    if (!fp) return 1;
    if (fgets(line, sizeof(line), fp)) {
        pid = (pid_t)atoi(line);
        if (pid > 0) kill(pid, SIGTERM);
    }
    pclose(fp);
    return 0;
}
