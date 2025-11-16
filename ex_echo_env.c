#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // กำหนด environment variables ใหม่
    char *envp[] = {
        "TEST=123",
        "APP_MODE=DEMO",
        "PATH=/usr/bin:/bin",
        NULL
    };

    // เรียกโปรแกรม printenv เพื่อพิมพ์ environment variables
    char *argv[] = { "printenv", NULL };

    // execve จะใช้ envp ใหม่แทน environment ปัจจุบัน
    execve("/usr/bin/printenv", argv, envp);

    // ถ้า execve ล้มเหลว
    perror("execve");
    return 1;
}
