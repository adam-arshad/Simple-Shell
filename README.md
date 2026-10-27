## Simple Shell

 A Unix-style command line shell written in C, built as a group project for CS210 at the University of Strathclyde.

## Features
- Runs programs by forking a child process and calling execvp
- Built-in commands: cd, getpath, setpath and exit
- Starts in your home directory and restores your original PATH on exit
- Command history of the last 20 commands, with history, !!, !n and !-n
- Saved history in ~/.hist_list, so it carries over between sessions
- Aliases with alias and unalias (up to 10)
- Saved aliases in ~/.aliases
- Alias and history chaining, with cycle detection to stop infinite loops

## Run it

    gcc -Wall -o shell shell.c
    ./shell 
    
## Team

Built by Ibrahim Bashir, Mahad Salad Hassan Nur, Arafat Omar, Muhammed Abdullah and Adam Arshad.
