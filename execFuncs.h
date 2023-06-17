#include "utilFuncs.h"

/** 
 * Function: comExec
 * Use: pick out the which task type (redirection/pipe/normal) the command is 
 * and run the corresponding function to run that command
 * Args: 
 *      1. argsarr: array of tokens
 *      2. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 *      3. taskFlag: pointer to the variable (created in main()) that signal whether redirection/piping
 * is involved
 *      4. taskSignPos: pointer to the variable that holds the index of the 
 * pipe/redirection token in argsarr
*/
void comExec(char *argsarr[], int waitFlag, int taskFlag, int taskSignPos){
    // fork a child process using fork()
    char *argsarr2[MAX_LINE/4] = {NULL};
    // Select function correspond to task type to run commands
    if (taskFlag == NORMAL){ // NORMAL
        comExecNormal(argsarr,waitFlag);
    } else if (taskFlag == INREDIRECT || taskFlag == OUTREDIRECT) { // IN(OUT)REDIRECT
        comExecRedirect(argsarr, argsarr2 ,waitFlag,taskFlag,taskSignPos);
    } else if (taskFlag == PIPE){ // PIPE
        comExecPipe(argsarr, argsarr2, waitFlag, taskSignPos);
    }
    freeSpares(argsarr, 0, MAX_LINE/2); // free memory in argsarr
    freeSpares(argsarr2, 0, MAX_LINE/4); // free memory is argsarr2
}

/** 
 * Function: comExecNormal
 * Use: run the command of normal (no redirection/pipe involved) task type
 * Args: 
 *      1. argsarr: array of tokens
 *      2. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 * Return: 0
*/
void comExecNormal(char *argsarr[], int waitFlag){
    // fork a child process using fork()
    pid_t pid;
    int status;

    /* fork a child process*/
    pid = fork();

    if(pid < 0){ /* error occured */
        fprintf(stderr, "Fork failed\n");
    } else if (pid == 0){ // Child
        if(execvp(argsarr[0], argsarr)<0){
            printf("-osh: The command is invalid\n");
            exit(1);
        } // run the command, if the command is invalid, print message
        
    } else { // parent
        // wait if waitFlag is 1
        if (waitFlag == 1){
            waitpid(pid, &status, 0);
        }
    }
}

/** 
 * Function: comExecRedirect
 * Use: run the command of redirection task type
 * Args: 
 *      1. argsarr: array of tokens
 *      2. argsarr2: array of a token for a sole file (for redirection)
 *      3. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 *      4. taskFlag: pointer to the variable (created in main()) that signal whether the task type is in/out redirection
 * is involved
 *      5. taskSignPos: pointer to the variable that holds the index of the 
 * pipe/redirection token in argsarr
*/
void comExecRedirect(char *argsarr[], char *argsarr2[] , int waitFlag, int taskFlag, int taskSignPos){
    // convert argsarr to contain only command arguments and transfer file for 
    // redirection to argsarr2
    splitArray(argsarr, argsarr2, taskSignPos); 

    //fork a child process using fork()
    pid_t pid;
    int status;
    int fd =-1; // file descriptor for redirection file

    // fd2 is a backup file descriptor that points to stdout
    int fd2=30;
    dup2(STDOUT_FILENO, fd2);

    /* fork a child process*/
    pid = fork();
    

    if(pid < 0){ /* error occured */
        fprintf(stderr, "Fork failed\n");
    }
    else if (pid == 0){
        if (taskFlag == INREDIRECT) { // INREDIRECT task type executed here
            fd = open(argsarr2[0], O_RDONLY | O_CREAT, 0777); // open file for input
            dup2(fd, STDIN_FILENO);  // file descriptor for stdin will now hold above file
            if(execvp(argsarr[0], argsarr)<0){
                dup2(fd2, STDOUT_FILENO);
                printf("-osh: The command is invalid\n");
                close(fd);
                exit(1);
            } // run the command, if the command is invalid, print message
        } else { // OUTREDIRECT task type executed here
            fd = open(argsarr2[0], O_WRONLY | O_CREAT, 0777); // open file to output to
            dup2(fd, STDOUT_FILENO); // file descriptor for stdout will now hold above file
            if(execvp(argsarr[0], argsarr)<0){
                dup2(fd2, STDOUT_FILENO);
                printf("-osh: The command is invalid\n");
                close(fd);
                exit(1);
            } // run the command, if the command is invalid, print message
        }
        // Close file descriptor (this line probably does not get executed)
        close(fd);
    } else {
        // wait for the child process if the flag for waiting is 1
        if (waitFlag == 1){
            waitpid(pid, &status, 0);
        }
    }
}

/** 
 * Function: comExecPipe
 * Use: set up for execution of the command of task type that involves the use of a pipe
 * Args: 
 *      1. argsarr: array of tokens
 *      2. argsarr2: array of tokens
 *      3. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 * is involved
 *      4. taskSignPos: pointer to the variable that holds the index of the 
 * pipe/redirection token in argsarr
*/
void comExecPipe(char *argsarr[], char *argsarr2[], int waitFlag, int taskSignPos){
    // convert argsarr to contain only command arguments and transfer arguments after 
    // the pipe token to argsarr2
    splitArray(argsarr, argsarr2, taskSignPos);

    pid_t pid; // id for child process
    int status;
    /* fork a child process*/
    pid = fork();

    if(pid < 0){ /* error occured */
        fprintf(stderr, "Fork failed\n");
    }
    else if (pid == 0){ // children process
        execPipe(argsarr,argsarr2, waitFlag); // this function executes the command
    } else { // parent process
        if (waitFlag == 1){ // wait for the child process if the flag for waiting is set to 1
            waitpid(pid, &status, 0);
        }
        //printf("child complete\n");
    }
}

/** 
 * Function: execPipe
 * Use: run the command of task type that involves the use of a pipe
 * Args: 
 *      1. argsarr: array of tokens
 *      2. argsarr2: array of tokens
 *      3. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 * is involved
 * Return:
 *      1. 1 if pipe of fork failed
 *      2. 0
*/
int execPipe(char *argsarr[], char *argsarr2[], int waitFlag){
    int fd[2];
    pid_t pid; // id for child process
    int status;

    // fd2 is a backup file descriptor that points to stdout
    int fd2=30;
    dup2(STDOUT_FILENO, fd2);

    /* create the pipe */
    if (pipe(fd) == -1){
        fprintf(stderr, "Pipe failed\n");
        return 1;
    }
    
    /* fork a child process */
    pid = fork();

    if (pid < 0){ // error occured
        fprintf(stderr, "Forked failed\n");
        return 1;
    } else if (pid == 0){ // child process
        close(fd[READ_END]); //close the unused end of the pipe
        // file descriptor for stdout will now point to the file at the write end of the pipe.
        dup2(fd[WRITE_END], STDOUT_FILENO);
        if(execvp(argsarr[0], argsarr)<0){
            dup2(fd2, STDOUT_FILENO);
            printf("-osh: The command before the pipe is invalid and is ignored\n");
            close(fd[WRITE_END]);
            exit(1);
        } // run the command, if the command is invalid, print message
    } else if (pid > 0){ // parent process
        close(fd[WRITE_END]); //close the unused end of the pipe
        waitpid(pid, &status, 0); // wait for the child process to finish
        // file descriptor for stdin will now point to the file at the read end of the pipe.
        dup2(fd[READ_END], STDIN_FILENO);
        if(execvp(argsarr2[0], argsarr2)<0){
            printf("-osh: The command after the pipe is invalid and is ignored\n");
            close(fd[READ_END]);
            exit(1);
        } // run the command, if the command is invalid, print message
    }
    
    return 0;
}
