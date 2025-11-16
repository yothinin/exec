#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD 128
#define MAX_ARGS 16

int main() {
    char line[MAX_CMD];

    // กำหนด environment variables ของโปรแกรมที่จะรัน
    char *envp[] = {
        "GREETING=HelloWorld",
        "APP_MODE=DEMO",
        "PATH=/usr/bin:/bin", // PATH สำหรับ execve
        NULL
    };

    while (1) {
        printf("myshell$ ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // ลบ newline
        line[strcspn(line, "\n")] = 0;

        // ออกจาก shell
        if (strcmp(line, "exit") == 0) break;

        // แยก command เป็น argv
        char *argv[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < MAX_ARGS-1) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        } else if (pid == 0) {
            // Child process -> รัน command ด้วย execve พร้อม envp
            char path[256];

            // ถ้า command ไม่ใช่ path เต็ม ให้ลอง /bin/<cmd>
            snprintf(path, sizeof(path), "/bin/%s", argv[0]);

            execve(path, argv, envp);

            // ถ้าไม่เจอ /bin/<cmd> ก็ลอง /usr/bin/<cmd>
            snprintf(path, sizeof(path), "/usr/bin/%s", argv[0]);
            execve(path, argv, envp);

            perror("execve"); // ถ้าไม่เจอไฟล์
            exit(1);
        } else {
            // Parent process -> รอ child เสร็จ
            wait(NULL);
        }
    }

    printf("Bye!\n");
    return 0;
}
