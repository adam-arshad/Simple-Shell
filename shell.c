// m8
#include <unistd.h> // fork, excel
#include <sys/types.h> // pid_t datatype
#include <sys/wait.h> // wait
#include <errno.h> // errno

#include <stdio.h> // printf
#include <string.h> //string comparison/functions

#define DELIMITERS " \t|><&;\n"

void tokenize(char line);

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

                tokenize (input);
        }
        return 0;
}

void tokenize(char input) {

        char* token = strtok(input, DELIMITERS);

        while(token != NULL) {
                       printf(""%s"\n", token);
                       token = strtok(NULL, DELIMITERS);
                }
}