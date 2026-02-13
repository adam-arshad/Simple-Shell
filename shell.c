// temp group name: m8
/* compile and run
        gcc shell.c -pedantic -Wall -o shell && ./shell
*/
#include <unistd.h> // fork, excel, execvp
#include <sys/types.h> // pid_t datatype
#include <sys/wait.h> // wait
#include <errno.h> // errno
#include <stdio.h> // printf, getf
#include <string.h> // string comparioson, string into token
#include <stdlib.h> // get enviroment

#define DELIMITERS " \t|><&;\n" // all things that would split a string into multiple tokens
#define MAX_LINE_LEN 512 // the maximum number of charaters in a string
#define MAX_TOCKEN_NUM 50 // the maximum number of tokens (max words)

// declare functions so that main functions know that they exist
void tokenize(char *input, char *tokens[]);
void printToken(char *token);
void execute(char *tokens[]);

// built in functions part of stage 3
void getPath(void);
void setPath(char* path);
void changeDir(char* path, char* path2);

int main(void) {
        // save the current path
        char *og_path = getenv("PATH");
        // get the home directory
        char *home = getenv("HOME");
        // set the current working directory to the home directory
        chdir(home);

        // declare variables
        char input[MAX_LINE_LEN];
        char *tokens[MAX_TOCKEN_NUM];

        // infinite loop
        while(1) {
                // print shell prompt
                printf("shellOS> ");

                // read user input and ctrl d = null
                if (fgets(input, MAX_LINE_LEN, stdin) == NULL) {
                        break;
                }

                // compares user input to 'exit'
                if (strcmp(input, "exit\n") == 0) {
                        break;
                }

                // call functions to split string into tokens
                tokenize(input, tokens);

                // call function to run command (dosent allow an empty input)
                if (tokens[0] != NULL) {
                        // check if built in command or shell command
                        if (strcmp(tokens[0], "getpath") == 0) {
                                getPath();
                        }
                        else if (strcmp(tokens[0], "setpath") == 0) {
                                setPath(tokens[1]);
                        }
                        else if(strcmp(tokens[0], "cd") == 0) { // built in command to change directory
                                changeDir(tokens[1], tokens[2]);
                        }
                        else { // any command not built-in is treated as external
                                execute(tokens);
                        }
                }
        }

        // set working directory to original
        setenv("PATH", og_path, 1);
        printf("Orignal path restored: %s\n", getenv("PATH"));
        // exites without errors
        return 0;
}

// stage one
void tokenize(char *input, char *tokens[]) {

        int i = 0;

        // split string into tokens wherever there is one of these: \t | > < & ; \n
        char *token = strtok(input, DELIMITERS);

        // store string into array of tokens
        while(token != NULL && i < MAX_TOCKEN_NUM) {
                tokens[i] = token;
                // printToken(token); //
                i++;
                token = strtok(NULL, DELIMITERS);
        }

        // make very last token equal NULL
        tokens[i] = NULL;
}

void printToken(char *token){ 
        printf("%s\n", token);
}

// stage two
void execute(char *tokens[]){
        // fork the program
        pid_t p = fork();

        // fork function did not work, e.g not enough space on computer
        if (p < 0) {
                perror("fork failed");
        }
        // child program
        else if (p == 0) {
                execvp(tokens[0], tokens); // terminates itself if sucessfully run and generates new program
                // execvp searches PATH automatically and accepts token array(echo, echo hello world)
                perror(tokens[0]); // error message with name of attemted command
                _exit(1); // exit with error code 1, terminates anyway
        }
        // parent program waits for child program to end
        else {
                wait(NULL);
        }
}

// stage three
void getPath(void){
        char *path = getenv("PATH");
        printf("%s\n", path);
}

void setPath(char* path){
        if (path == NULL) {
                printf("ERROR - setpath requires a parameter\n");
        }
        else{
                setenv("PATH", path, 1);
        }      
}

void changeDir(char* path, char* path2){
        if(path != NULL && path2 != NULL) { //Check for too many arguments
                printf("ERROR - cd only takes one argument\n"); // error message for invalid usage
        }
        else if(path == NULL) { // if user typed only "cd" then go to HOME
                char* home = getenv("HOME"); // get HOME directory from environment
                // try to change directory to HOME, -1 means it failed
                if (chdir(home) == -1) {
                        perror("cd"); // print system error message for why cd failed
                }
        }
        else { //if user typed "cd <path>" then go to that path - try to change directory to the user provided directory
                if(chdir(path) == -1) {
                        perror("cd");  //print system error message (e.g. no such directory)
                }
        } 
}

/*
        _       _ _
     __| |_   _| | |_   _
    / _` | | | | | | | | |
   | (_| | |_| | | | |_| |
    \__,_|\__,_|_|_|\__, |
                    |___/

  _ _     _          
 (_) |__ | |__  _   _
 | | '_ \| '_ \| | | |
 | | |_) | |_) | |_| |
 |_|_.__/|_.__/ \__, |
                |___/

  __  __ _
 |  \/  / |
 | |\/| | |
 | |  | | |
 |_|  |_|_|
           
    _               __ 
   / \   _ __ __ _ / _|
  / _ \ | '__/ _` | |_ 
 / ___ \| | | (_| |  _|
/_/   \_\_|  \__,_|_|  
 */
