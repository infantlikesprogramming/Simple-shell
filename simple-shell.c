#include "execFuncs.h"

/** 
 * Function: main
 * Use: This function will behave like a simple shell that can:
 *      1. Receive and execute commands
 *      2. Pipe and IO redirectionL: A command can have pipe ("|") or input/output 
 * redirection ("<" or ">") features
 *      3. History feature: However, only one of these feature can be used per command, and at one
 * time only per command (i.e. there may not be two pipes, or a pipe and a file 
 * redirection in the same command). 
 *      4. Background feature: To execute the most recent command, simple enter "!!"
 *      5. To execute the command in the background and move on with the shell,
 * add " &" after a command
 *      6. To exit the shell use the command "exit"
 * 
 * Return: 0
*/

int main(void){
    // variables for command and arguments array
    char args[MAX_LINE] = "";   // command string
    char hisargs[MAX_LINE] = "";    // recent command string
    int argnum = MAX_LINE/2 + 1;
    char *argsarr[argnum];  // argument array

    int should_run = 1;     // variable for the while loop's condition
    
    // initialize arguments array
    for (int i = 0; i<argnum; i++){ // Allocate space for all arguments array's elements
        argsarr[i]= (char *) malloc((MAX_LINE/2)*sizeof(char));
    }
    
    // while loop to perform execution of commands
    while (should_run){
        // variables for tasks
        int waitFlag = 1;   // flag to signal whether the process for the command should wait
        int taskFlag = 0;   // flag to signal if a redirection of piping is involed in the command
        int taskSignPos = 0; // position of the redirection of pipe token if taskFlag != 0

        // prompt
        printf("osh>");

        // get the arguments array and flags
        if ((should_run = getArgsForExec(args, argsarr, &taskSignPos, &waitFlag, &taskFlag, hisargs))){
            if (should_run == EXITSHELL){ // if getArgsForExec return EXITSHELL, end program
                printf("Have a good day!\n");
                return 0;
            }
            comExec(argsarr,waitFlag,taskFlag, taskSignPos);    // Run the command
            strcpy(hisargs,args);   // Save current command for history feature
        }
        // Ask the user if they want to repeat
        fflush(stdout);
        should_run = 1; // in case should_run == 0, we know the command is invalid, but we don't want the shell (loop) to stop.
    }
    return 0; 
}