#include <stdio.h>
#include <unistd.h>

int main() {
    char *envp[] = {
        "TEST=123",
        "APP_MODE=DEMO",
        "PATH=/usr/bin:/bin",
        NULL
    };

    char *argv[] = { "echo", "Hello!", NULL };

    execve("/bin/echo", argv, envp);
    perror("execve");
    return 1;
}
