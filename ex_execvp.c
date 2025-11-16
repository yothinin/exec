#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CMD 1024
#define MAX_ARGS 64

int main() {
    char cmdline[MAX_CMD];

    while (1) {
        printf("myshell$ ");
        fflush(stdout);

        // อ่าน input จากผู้ใช้
        if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
            break; // ctrl+d
        }

        // เอา \n ออก
        cmdline[strcspn(cmdline, "\n")] = '\0';

        // แยกคำสั่งเป็น token
        char *argv[MAX_ARGS];
        int argc = 0;

        char *token = strtok(cmdline, " ");
        while (token != NULL && argc < MAX_ARGS - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL; // ต้องปิดท้ายด้วย NULL

        // ถ้าผู้ใช้ไม่พิมพ์อะไร ให้ continue
        if (argc == 0)
            continue;

        // fork process ลูก
        pid_t pid = fork();

        if (pid == 0) {
            // child: เรียก execvp
            execvp(argv[0], argv);
            perror("execvp"); // ถ้า execvp fail
            exit(1);
        } else if (pid > 0) {
            // parent: รอ child ทำงานเสร็จ
            wait(NULL);
        } else {
            perror("fork");
        }
    }

    return 0;
}
