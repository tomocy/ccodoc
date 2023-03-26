#include "platform.h"

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void run_cmd(const char* path, const char** args)
{
#if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_MACOS
    int child_pid = fork();
    if (child_pid == -1) {
        return;
    }

    if (child_pid == 0) {
        int gchild_pid = fork();
        if (gchild_pid == -1) {
            exit(1);
            return;
        }

        if (gchild_pid == 0) {
            execv(path, (char* const*)args);

            exit(1);
            return;
        }

        exit(0);
        return;
    }

    int status = 0;
    waitpid(child_pid, &status, 0);
#endif
}
