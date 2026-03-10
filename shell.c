/* group number:
             ___  
  _ __ ___  ( _ ) 
 | '_ ` _ \ / _ \ 
 | | | | | | (_) |
 |_| |_| |_|\___/ 
                  
compile and run code (just copy and paste into terminal then hit enter):

        gcc shell.c -pedantic -Wall -o shell && ./shell

*/

#include <unistd.h> // fork, excel, execvp
#include <sys/types.h> // pid_t datatype
#include <sys/wait.h> // wait
#include <errno.h> // errno
#include <stdio.h> // printf, getf
#include <string.h> // string comparioson, string into token
#include <stdlib.h> // get enviroment, malloc

#define DELIMITERS " \t|><&;\n" // all things that would split a string into multiple tokens
#define MAX_LINE_LEN 512 // the maximum number of charaters in a string
#define MAX_TOCKEN_NUM 50 // the maximum number of tokens (max words)
#define HISTORY_SIZE 20 // the maximum number of tokens to keep in history
#define MAX_ALIASES 10 // the maximum number of aliases
#define MAX_ALIAS_LEN 512 // the maximum alias command length


// global variables for history
char history[HISTORY_SIZE][MAX_LINE_LEN];
int history_count = 0;

// global variables for aliases
char alias_names[MAX_ALIASES][MAX_LINE_LEN]; // store alias names
char alias_commands[MAX_ALIASES][MAX_LINE_LEN]; // store commands 
int alias_count = 0; // how many aliases exist



// declare functions so that main functions know that they exist
void tokenize(char *input, char *tokens[]);
void printToken(char *token);
void execute(char *tokens[]);

// built in functions part of stage 3/4
void getPath(void);
void setPath(char* path);
void changeDir(char* path, char* path2);

// history functions
int stringToInt(char* str);
void addHistory(char* command);
void printHistory(FILE* output);
int isHistoryCommand(char* input);
char* getHistoryCommand(char* input);
void loadHistory(void);
void saveHistory(void);
char* getHistoryFilepath(void);

// alias functions
void addAlias(char* name, char* command);
void removeAlias(char* name);
void printALias(void);
char* checkAlias(char* firstToken, char* remInput);


int main(void) {
        // save the current path
        char *og_path = getenv("PATH");
        // get the home directory
        char *home = getenv("HOME");
        // set the current working directory to the home directory
        chdir(home);

        loadHistory();

        // declare variables
        char og_input[MAX_LINE_LEN];
        char input[MAX_LINE_LEN];
        char *tokens[MAX_TOCKEN_NUM];

        // infinite loop
        while(1) {
                // print shell prompt
                printf("shellOS> ");

                // read user input and ctrl d = null
                if (fgets(og_input, MAX_LINE_LEN, stdin) == NULL) {
                        break;
                }

                // compares user input to 'exit'
                if (strcmp(og_input, "exit\n") == 0) {
                        break;
                }

                // copy original input into working input so that orignal input is never modified
                strcpy(input, og_input);

                // NEW: Check if this is a history command (!!, !5, !-2, etc)
                if (isHistoryCommand(og_input)) {
                        char* history_cmd = getHistoryCommand(og_input);
                        if (history_cmd != NULL) {
                                // Replace working input with the history command
                                strcpy(input, history_cmd);
                                printf("%s", history_cmd);  // show what we're executing
                        } else {
                                continue;  // error was printed, get next command
                        }
                }
                // Checking for alias
                char* firstToken = strtok(input, " \t\n"); // get first word
                char* remInput = NULL;
                
                if(firstToken != NULL) { // go to the rest of the line in og_line
                 remInput = og_input + (firstToken - input) + strlen(firstToken);
                 
                  // skip leading spaces/ tabs between alias and rest of line
                     while(*remInput == ' ' || *remInput == '\t') {
                       remInput++;
                   }
                   
                   // calls function that checks if firstToken is an alias. 
                   // If yes, replacement will contain full command with alias replaced
                   char* replace = checkAlias(firstToken, remInput);
                    if(replace != NULL) {
                     strcpy(input, replace); // replace input with expanded alias so shell can execute it
                    }
                  }
                    
                   

                // call functions to split string into tokens
                tokenize(input, tokens);

                // call function to run command (doesn't allow an empty input)
                if (tokens[0] != NULL) {

                        if (!isHistoryCommand(og_input)) {
                                addHistory(og_input);
                        }
                        
                        // Stage 7: Alias commands
                        if(strcmp(tokens[0], "alias") == 0) { // checks if user typed "alias"
                         
                        if(tokens[1] == NULL) {
                          printALias(); // check in no argument. User just typed "alias"
                        } else {
                          char command[MAX_LINE_LEN] = ""; // add new alias
                          for(int i = 2; tokens[i] != NULL; i++) { // loop through all tokens after alias name
                            strcat(command, tokens[i]);
                            if(tokens[i+1] != NULL) strcat(command, " "); // concatenates each token to command. adds space between tokens
                            }
                            addAlias(tokens[1], command);
                           }
                        }
                        // handles unalias
                        else if (strcmp(tokens[0], "unalias") == 0) { 
                         if (tokens[1] == NULL) {
                             printf("ERROR, unalias + name required\n");
                           } else {
                           removeAlias(tokens[1]);
                           }
                        }            

                        // check if built in command or shell command
                        if (strcmp(tokens[0], "getpath") == 0) {
                                getPath();
                        }
                        else if (strcmp(tokens[0], "setpath") == 0) {
                                setPath(tokens[1]);
                        }
                        else if(strcmp(tokens[0], "cd") == 0) {
                                changeDir(tokens[1], tokens[2]);
                        }
                        else if(strcmp(tokens[0], "history") == 0) {
                                // print history to terminal
                                printHistory(stdout);
                        }
                        else {
                                execute(tokens);
                        }
                }
        }

        // print history to file
        saveHistory();

        // set working directory to original
        setenv("PATH", og_path, 1);
        printf("Orignal path restored: %s\n", getenv("PATH"));

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

// convert a sting into an integer
int stringToInt(char* str){
        int result = 0;
        int i = 0;
        int digit;

        while (str[i] >= '0' && str[i] <= '9'){
                digit = str[i] - '0';
                result = result * 10 + digit;
                i++;
        }

        return result;
}

// add a command to the history
void addHistory(char* command){
        // find where to store command in circular array
        int index = history_count % HISTORY_SIZE;
        // copy command into the history location
        strcpy(history[index], command);
        history_count++;
}

void printHistory(FILE* output){
        // find out how much of the history is being used
        int total = history_count;
        if (total > HISTORY_SIZE){
                total = HISTORY_SIZE;
        }

        int start_num = history_count - total + 1;
        int index;

        for (int i = 0; i < total; i++){
                index = (start_num + i - 1) % HISTORY_SIZE;
                fprintf(output, "%d %s", start_num + i, history[index]);
        }

}

int isHistoryCommand(char* input){
        if (input[0] == '!'){
                return 1;
        }
        return 0;
}

char* getHistoryCommand(char* input){
        int target_num;

        // handle !!
        if (strcmp(input, "!!\n") == 0) {
                if (history_count == 0) {
                        printf("Error, no commands in history \n");
                        return NULL;
                }
                target_num = history_count;
        } else if (input[1] == '-') {
                // skip the - and get the offset num
                int offest = stringToInt(input + 2);
                target_num = history_count - offest;

        } else {
                target_num = stringToInt(input + 1);

        }

        if (target_num < 1 || target_num > history_count){
                printf("Error, invalid history number \n");
                return NULL;
        }

        int oldest = history_count - HISTORY_SIZE + 1;
        if (oldest < 1) {
                oldest = 1;
        }

        if (target_num < 1){
                printf("Error, invalid history number \n");
                return NULL;
        }
        
        int index = (target_num - 1) % HISTORY_SIZE;
        return history[index];
}

void loadHistory(void) {
        FILE *file = fopen(getHistoryFilepath(), "r");
        if (file == NULL) {
                return; // file doesn't exist yet
        }

        char line[MAX_LINE_LEN];
        char* command;
        while (fgets(line, sizeof(line), file) != NULL) {
                command = strchr(line, ' '); // skip the number

                if (command != NULL) {
                        command++; // move past space
                        addHistory(command);
                }
        }

        fclose(file);
}

void saveHistory(void) {
        FILE *file = fopen(getHistoryFilepath(), "w");
        if (file == NULL) {
                return;
        }

        printHistory(file);
        fclose(file);
}

char* getHistoryFilepath(void) {
    static char filepath[MAX_LINE_LEN]; // static so pointer stays valid
    char *home = getenv("HOME");
    snprintf(filepath, sizeof(filepath), "%s/.hist_list", home);
    return filepath;
}

void addAlias(char* name, char* command) {
        int i;
        
        for (i = 0; i< alias_count; i++) {
                if (strcmp(alias_names[i], name) == 0) {
                        strcpy(alias_commands[i], command);
                        printf("Alias updated\n", name);
                        return;
                }
        }
        
        if (alias_count >= MAX_ALIASES) {
                printf("ERROR - maximum number of aliases reached\n");
                return;
        }
        strcpy(alias_names[alias_count], name);
        strcpy(alias_commands[alias_count], command);
        alias_count++;
}

void removeAlias(char* name) {
        int found;
        int i;
        found = -1;

        for (i = 0; i < alias_count; i++) {
                if (strcmp(alias_names[i], name) == 0) {
                        found = i;
                        break;
                }
        }
        if (found == -1) {
                printf("ERROR - no such alias\n");
                return;
        }
        for (i = found; i < alias_count - 1; i++) {
                strcpy(alias_names[i], alias_names[i + 1]);
                strcpy(alias_commands[i], alias_commands[i + 1]);
        }
        alias_count--;
}

void printALias(void) {
        int i;

        if (alias_count == 0) {
                printf("No aliases set\n");
                return;
        }
        for (i = 0; i < alias_count; i++) {
                printf("alias %s='%s'\n", alias_names[i], alias_commands[i]);
        }


}

char* checkAlias(char* firstToken, char* remInput) {
         static char expanded[MAX_LINE_LEN];
        int i;

        for (i = 0; i < alias_count; i++) {
                if (strcmp(firstToken, alias_names[i]) == 0) {
                        strcpy(expanded, alias_commands[i]);

                      if (remInput != NULL) {
                                strcat(expanded, " ");
                                strcat(expanded, remInput);
                        } else {
                                strcat(expanded, "\n");
                        }
                        
                        return expanded;
                }
        }
        
        return NULL;
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
