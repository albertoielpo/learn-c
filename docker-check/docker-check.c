/**
 * This program is just an util for the script "docker" which is a wrap of docker it self.
 * It warns the programmer to enforce a specific user
 * Both this program and docker must be deployed inside /usr/local/bin/ with has priority over /usr/bin
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static pid_t get_ppid(pid_t pid) {
    char path[64];
    char line[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    while (fgets(line, sizeof(line), f)) {
        pid_t ppid;
        if (sscanf(line, "PPid: %d", &ppid) == 1) {
            fclose(f);
            return ppid;
        }
    }
    fclose(f);
    return -1;
}

int main(void) {
    char path[64];
    char buf[4096];
    size_t len, i;

    // Skip two levels: docker-check → wrapper → actual caller
    pid_t wrapper_pid = getppid();
    pid_t caller_pid = get_ppid(wrapper_pid);

    snprintf(path, sizeof(path), "/proc/%d/cmdline", caller_pid);
    FILE *f = fopen(path, "r");
    if (!f)
        return 1; // cannot read → assume interactive

    len = fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    buf[len] = '\0';

    // Skip shell name (arg0)
    i = 0;
    while (i < len && buf[i] != '\0')
        i++;
    i++;

    // arg1 is a file on disk → called from a script → exit 0 (pass through)
    if (i < len && buf[i] != '\0' && access(buf + i, F_OK) == 0)
        return 0;

    return 1; // interactive human → show warning
}
