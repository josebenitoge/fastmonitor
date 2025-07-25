#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    FILE *fp = fopen("fastmonitor.pid", "r");
    if (!fp) {
	//perror("Stop error: Unexpected state");
        return 1;
    }

    int pid;
    if (fscanf(fp, "%d", &pid) != 1) {
        fclose(fp);
	perror("Stop error: Corrupted state");
        return 1;
    }

    fclose(fp);

    if (kill(pid, SIGTERM) == 0) {
        printf("FastMonitor (%d) stopped.\n", pid);
        remove("fastmonitor.pid");
    } else {
        perror("Error. Couldn't stop Fast Monitor");
    }

    return 0;
}

