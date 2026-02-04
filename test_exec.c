#include <stdio.h>
#include <unistd.h>

int main() {
    printf("About to run ls...\n");
    
    char *args[] = {"ls", "-l", NULL};
    execvp("ls", args);
    
    printf("If you see this, exec failed!\n");
    
    return 0;
}
