#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
    FILE *fp = fopen("monitor.pid", "r");
    if (!fp) {
        printf("No se encontró monitor.pid. ¿Está el monitor en ejecución?\n");
        return 1;
    }

    int pid;
    if (fscanf(fp, "%d", &pid) != 1) {
        printf("No se pudo leer el PID.\n");
        fclose(fp);
        return 1;
    }

    fclose(fp);

    if (kill(pid, SIGTERM) == 0) {
        printf("Proceso monitor (%d) detenido.\n", pid);
        remove("monitor.pid");
    } else {
        perror("Error al detener el proceso");
    }

    return 0;
}

