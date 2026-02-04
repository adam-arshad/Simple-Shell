#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Start - only one program running\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("CHILD: I'm the clone!\n");
    } else {
        printf("PARENT: I made a clone with ID %d\n", pid);
    }
    
    printf("Both parent and child print this line\n");
    
    return 0;
}
