// m8
#include <unistd.h>     // fork, execl
#include <sys/types.h>  // pid_t datatype
#include <sys/wait.h>   // wait
#include <errno.h>      // errno
#include <stdio.h>      // printf
#include <string.h>     // string comparison/functions

#define DELIMITERS " \t|><&;\n" // defines what characters will separate tokes in input string

void tokenize(char *input); // Declares function that takes a string and processes it

int main(void) {
    char input[512];
    int n = 512; // variable for buffer size when calling fgets()

    while(1) {
        printf("shellOS> ");

        if (fgets(input, n, stdin) == NULL) { 
            break;                       // reads line of input from user
        }
        if (strcmp(input, "exit\n") == 0) {
            break; // check user typed 'exit'. if input is exactly exit then exit loop.
        }

        tokenize(input); // Calls function and passes input as the string
    }
    return 0;
}

void tokenize(char *input) {
    char* tokens[50];  // As per assignment: "Fifty tokens will be a reasonable number"
    int i = 0;
    
    char* token = strtok(input, DELIMITERS); // strtok processes input into tokens using delimiters. Returns pointer to first token

    while(token != NULL && i < 50) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, DELIMITERS); //splits a string into multiple pieces using delimiters
    }
    tokens[i] = NULL;  // NULL-terminate as required by exec functions
    
    // Now execute if we have a command
    if (tokens[0] != NULL) { // CHeck if user typed any input
        pid_t p = fork(); // Fork creates new process
                          // Returns child PID to the parent i.e. pos number. Returns 0 to the child (p == 0).
        
        if (p < 0) {
            printf("Error forking!\n");
            return; // Return -1 on error.
        }
        else if (p == 0) {
            // Child - using execvp as assignment says "choose appropriate variant that uses PATH" and passes it with tokens
            if (execvp(tokens[0], tokens) == -1) {
                int n = errno; // Stores error number
                perror("Oh oh, there was trouble"); // prints error
                printf("Error number %d\n", errno); // print actual error number
            }
        }
        else if (p > 0) { // Parent waits for child to finish
            wait(NULL); // 
        }
    } 
}
