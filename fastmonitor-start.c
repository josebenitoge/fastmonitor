#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CONFIG_FILE    "fastmonitor.conf"
#define LOG_FILE       "fastmonitor.log"
#define PID_FILE       "fastmonitor.pid"
#define MAX_USERS      100
#define LINE_BUF_SIZE  256

char *allowed_users[MAX_USERS];
int   allowed_user_count = 0;
int   polling_interval   = 10;

/* Prototipos */
void    trim(char *s);
int     load_config(void);
int     is_allowed_user(const char *user);
char   *get_country_from_ip(const char *ip);
void    log_intruder(const char *user, const char *ip);
void    kick_user(const char *user);
void    check_sessions(void);

int main(void) {
    if (geteuid() != 0) {
        fprintf(stderr, "\033[1;31m[ERROR]\033[0m Debian como root.\n");
        return EXIT_FAILURE;
    }

    /* 1) Validar config */
    system("./fastmonitor-conf");

    if (load_config() != 0) {
        fprintf(stderr, "\033[1;31m[ERROR]\033[0m No se pudo leer '%s'\n",
                CONFIG_FILE);
        return EXIT_FAILURE;
    }

    /* 2) Crear PID y log si no existen */
    { FILE *f = fopen(PID_FILE, "w");
      if (f) { fprintf(f, "%d\n", getpid()); fclose(f); } }
    { int fd = open(LOG_FILE,
                   O_CREAT|O_APPEND|O_WRONLY, 0644);
      if (fd >= 0) close(fd); }

    /* 3) Bucle principal */
    while (1) {
        check_sessions();
        sleep(polling_interval);
    }
    return EXIT_SUCCESS;
}

/* Recorta espacios/tab/nl principio y fin */
void trim(char *s) {
    char *start = s;
    while (*start==' '||*start=='\t'||*start=='\n') start++;
    char *end = start + strlen(start) -1;
    while (end>start && (*end==' '||*end=='\t'||*end=='\n'))
        *end-- = '\0';
    if (start!=s) memmove(s, start, strlen(start)+1);
}

/* Carga polling_interval y allowed_users del conf */
int load_config(void) {
    FILE *f = fopen(CONFIG_FILE, "r");
    if (!f) return -1;
    char line[LINE_BUF_SIZE];
    int in_block = 0;
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (strcmp(line,"#start")==0)     { in_block=1; continue; }
        if (strcmp(line,"#end")==0)       break;
        if (!in_block)                     continue;

        if (!strncmp(line,"polling:",8)) {
            polling_interval = atoi(line+8);
        }
        else if (!strncmp(line,"users:[",7)) {
            char *p = line+7;
            char *q = strchr(p,']'); if(q)*q='\0';
            for(char *tok=strtok(p,","); tok; tok=strtok(NULL,","))
            {
                trim(tok);
                if (allowed_user_count<MAX_USERS)
                    allowed_users[allowed_user_count++]=strdup(tok);
            }
        }
    }
    fclose(f);
    return 0;
}

/* Comprueba si user está en la whitelist */
int is_allowed_user(const char *user) {
    for (int i=0;i<allowed_user_count;i++)
        if (!strcmp(user,allowed_users[i])) return 1;
    return 0;
}

/* Llama a ipinfo.io para país de la IP */
char *get_country_from_ip(const char *ip) {
    static char country[32];
    char cmd[128];
    snprintf(cmd,sizeof(cmd),
             "curl -s https://ipinfo.io/%s/country 2>/dev/null", ip);
    FILE *fp = popen(cmd,"r");
    if (!fp) return "Unknown";
    if (!fgets(country,sizeof(country),fp)) {
        pclose(fp);
        return "Unknown";
    }
    pclose(fp);
    trim(country);
    return country;
}

/* Anota intrusión en LOG_FILE */
void log_intruder(const char *user, const char *ip) {
    FILE *logf = fopen(LOG_FILE,"a");
    if (!logf) return;
    time_t now=time(NULL);
    char ts[64];
    strftime(ts,sizeof(ts),"%Y-%m-%d %H:%M:%S",localtime(&now));
    char *country = get_country_from_ip(ip);
    fprintf(logf,"[%s] Intruder: %s | IP: %s | Country: %s\n",
            ts,user,ip,country);
    fclose(logf);
}

/* Mata al usuario */
void kick_user(const char *user) {
    char cmd[128];
    snprintf(cmd,sizeof(cmd),"pkill -KILL -u %s",user);
    system(cmd);
}

/* Revisa sesiones SSH activas y expulsa intrusos */
void check_sessions(void) {
    /* who -u: col1=user col7=host (IP) */
    FILE *fp = popen("who -u | awk '{print $1\" \"$7}'","r");
    if (!fp) return;
    char buf[LINE_BUF_SIZE], user[64], ip[64];
    while (fgets(buf,sizeof(buf),fp)) {
        trim(buf);
        if (sscanf(buf,"%63s %63s",user,ip)==2) {
            /* descartar si ip es "-" (sesión local) */
            if (strcmp(ip,"-")==0) continue;
            /* quitar paréntesis */
            if (ip[0]=='(') memmove(ip,ip+1,strlen(ip));
            size_t L=strlen(ip);
            if (L&&ip[L-1]==')') ip[L-1]='\0';

            if (!is_allowed_user(user)) {
                log_intruder(user,ip);
                kick_user(user);
            }
        }
    }
    pclose(fp);
}
