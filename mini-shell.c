#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("เปิด mini-terminal และรัน sh...\n");

    // fork เพื่อสร้าง process ใหม่
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // child process: เรียก bash
        execlp("sh", "sh", NULL);

        // ถ้า execlp ล้มเหลว
        perror("execlp failed");
        return 1;
    } else {
        // parent process: รอ child process จบ
        wait(NULL);
        printf("sh process จบแล้ว\n");
    }

    return 0;
}
