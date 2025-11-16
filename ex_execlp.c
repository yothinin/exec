#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> // สำหรับ wait()

int main() {
    printf("Parent: PID ของผมคือ %d\n", getpid());

    // 1. fork(): สร้างกระบวนการลูก
    pid_t pid = fork();

    if (pid == 0) {
        // --- อยู่ในกระบวนการลูก (Child Process) ---
        printf("Child: ผมเกิดมาแล้ว PID คือ %d\n", getpid());
        printf("Child: ผมกำลังจะแปลงร่างเป็นโปรแกรม 'ls'...\n");

        // 2. exec(): แทนที่โปรแกรมในกระบวนการลูก
        execlp("ls", "ls", NULL);

        // ถ้าโค้ดบรรทัดนี้ทำงานได้ แสดงว่า exec มีปัญหา
        perror("exec failed");
    } else {
        // --- อยู่ในกระบวนการแม่ (Parent Process) ---
        printf("Parent: ผมสร้าง child เสร็จแล้ว PID ลูกคือ %d\n", pid);
        printf("Parent: ผมจะรอจนกว่า child จะทำงานเสร็จ...\n");

        // wait(): รอให้กระบวนการลูกจบการทำงาน
        wait(NULL);

        printf("Parent: child ทำงานเสร็จแล้ว กลับมาที่ parent ครับ\n");
    }

    return 0;
}
