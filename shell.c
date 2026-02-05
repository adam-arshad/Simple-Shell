// m8
#include <unistd.h>     // fork, execl
#include <sys/types.h>  // pid_t datatype
#include <sys/wait.h>   // wait
#include <errno.h>      // errno
#include <stdio.h>      // printf
#include <string.h>     // string comparison/functions

#define DELIMITERS " \t|><&;\n"

void tokenize(char *input);

int main(void) {
    char input[512];
    int n = 512;

    while(1) {
        printf("shellOS> ");

        if (fgets(input, n, stdin) == NULL) {
            break;
        }
        if (strcmp(input, "exit\n") == 0) {
            break;
        }

        tokenize(input);
    }
    return 0;
}

void tokenize(char *input) {
    char* tokens[50];  // As per assignment: "Fifty tokens will be a reasonable number"
    int i = 0;
    
    char* token = strtok(input, DELIMITERS);

    while(token != NULL && i < 50) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS);
    }
    tokens[i] = NULL;  // NULL-terminate as required by exec functions
    
    // Now execute if we have a command
    if (tokens[0] != NULL) {
        pid_t p = fork();
        
        if (p < 0) {
            printf("Error forking!\n");
            return;
        }
        else if (p == 0) {
            // Child - using execvp as assignment says "choose appropriate variant that uses PATH"
            if (execvp(tokens[0], tokens) == -1) {
                int n = errno;
                perror("Oh oh, there was trouble");
                printf("Error number %d\n", errno);
            }
        }
        else if (p > 0) {
            wait(NULL);
        }
    }
}
