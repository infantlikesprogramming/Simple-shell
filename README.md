# Simple-shell

This program acts as a simple shell:  
1. It can receive and execute Linux commands (with arguments)  
2. Pipe and IO redirection features: It provides pipe (`!`) or IO redirection (`<` or `>`) features  
3. However, only one of these features can be used per command, and at one time only per command (i.e. there may not be two pipes, or a pipe and an IO redirection in the same command).  
4. History feature: To execute the most recent command, simply enter `!!`  
5. Background feature: To execute the command in the background and move on with the shell, * add `&` after a command  
6. To exit the program, simply run the command `exit`

*Note: Not all commands will run. The implementation of the shell is very basic, using execvp() to run commands.

## How to run (for beginner programmers):  
1. Compile and run the program: `gcc -o shell simple-shell.c && ./shell`  
2. Or just run compiled program: `./shell`

## Test commands:  
1. `pwd` (output full path name of the current directory)  
2. `cat defs.h > sample.txt &` (read the file defs.h and redirect the output to sample.txt in the background)  
3. `wc -l < defs.h` (list the number of lines in defs.h)  
4. `ls | sort` (output the (sorted) items in a directory)  
5. `!! &` (run previous command in the background)  

