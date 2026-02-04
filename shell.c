#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    char input[512];
    char *tokens[50];
    int token_count;
    char *token;
    pid_t pid;
    
    while(1) {
        printf("myshell> ");
        
        if (fgets(input, 512, stdin) == NULL) {
            printf("\nGoodbye!\n");
            break;
        }
        
        int len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
        
        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n");
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        token_count = 0;
        token = strtok(input, " \t");
        
        while (token != NULL && token_count < 50) {
            tokens[token_count] = token;
            token_count++;
            token = strtok(NULL, " \t");
        }
        
        tokens[token_count] = NULL;
        
        if (token_count == 0) {
            continue;
        }
        
        pid = fork();
        
        if (pid == 0) {
            execvp(tokens[0], tokens);
            printf("Command not found: %s\n", tokens[0]);
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            printf("Error: fork failed\n");
        }
    }
    
    return 0;
}
