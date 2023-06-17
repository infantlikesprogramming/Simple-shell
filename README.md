# Simple-shell
This program acts as a simple shell:

It can receive and execute Linux commands (with arguments)\n
Pipe and IO redirection features: It provides pipe ("|") or IO redirection ("<" or ">") features 
However, only one of these features can be used per command, and at one time only per command (i.e. there may not be two pipes, or a pipe and an IO redirection in the same command). 
History feature: To execute the most recent command, simply enter "!!" 
Background feature: To execute the command in the background and move on with the shell, * add " &" after a command
To exit the program, simply run the command “exit”
*Note: Not all commands will run. The implementation of the shell is very basic, using execvp() to run commands.
How to run (for beginner programmers):
Compile and run the program: gcc -o simple-shell simple-shell.c && ./simple-shell
Or just run compiled program: ./simple-shell
Test commands:
pwd (output full path name of the current directory)
cat defs.h > sample.txt & (read the file defs.h and redirect the output to sample.txt in the background)
wc -l < defs.h (list the number of lines in defs.h)
ls | sort (output the (sorted) items in a directory)
!! & (run previous command in the background)
