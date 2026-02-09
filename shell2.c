// temp group name: m8
#include <unistd.h> // fork, excel, execvp
#include <sys/types.h> // pid_t datatype
#include <sys/wait.h> // wait
#include <errno.h> // errno

#include <stdio.h> // printf, getf
#include <string.h> // string comparioson, string into token
#include <stdlib.h> // get enviroment
#include <unistd.h> // change working directory

#define DELIMITERS " \t|><&;\n" // all things that would split a string into multiple tokens
#define MAX_LINE_LEN 512 // the maximum number of charaters in a string
#define MAX_TOCKEN_NUM 50 // the maximum number of tokens (max words)

// declare functions so that main functions know that they exist
void tokenize(char *input, char *tokens[]); 
void printToken(char *token);
void execute(char *tokens[]);

int main(void) {

        // declare variables
        char input[MAX_LINE_LEN];
        char *tokens[MAX_TOCKEN_NUM];

        // infinite loop
        while(1) {
                // print shell prompt
                printf("shellOS> ");

                // read user input
                if (fgets(input, MAX_LINE_LEN, stdin) == NULL) {
                        break;
                }

                // exit command
                if (strcmp(input, "exit\n") == 0) {
                        break;
                }

                // call functions to split string into tokens
                tokenize(input, tokens);

                // call function to run command (dosent allow an empty input)
                if (tokens[0] != NULL) {
                        execute(tokens);
                }
        }

        return 0;
}

void tokenize(char *input, char *tokens[]) {

        int i = 0;

        // split string into tokens wherever there is one of these: \t | > < & ; \n 
        char *token = strtok(input, DELIMITERS);

        // store string into array of tokens
        while(token != NULL && i < MAX_TOCKEN_NUM) {
                tokens[i] = token;
                printToken(token);
                i++;
                token = strtok(NULL, DELIMITERS);
        }

        // make very last token equal NULL
        tokens[i] = NULL;
}

void printToken(char *token){
        printf("%s\n", token);
}

void execute(char *tokens[]){
        // fork the program
        pid_t p = fork();

        // fork function did not work
        if (p < 0) {
                perror("fork failed");
        }
        // child program 
        else if (p == 0) {
                execvp(tokens[0], tokens); // terminates itself if sucessfully run
                perror("execvp failed");
                _exit(1); // exit with error code 1
        }
        // parent program waits for child program to end
        else {
                wait(NULL);
        }
}