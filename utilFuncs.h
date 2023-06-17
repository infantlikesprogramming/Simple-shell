#include "decl.h"
#include "defs.h"

/** 
 * Function: freeSpares
 * Use: free all dynamically allocated space to specified subarray of an array
 * Args:
 *      1. argsarr: the array
 *      2. start: the starting index of the subarray
 *      3. end: the ending index of the subarray 
*/
void freeSpares(char *argsarr[], int start, int end){
    for (int i=start; i<end; i++){ // loop through the subarray
        free(argsarr[i]);   // free the memory at index i
        argsarr[i]= NULL;   // assign NULL to element at index i
    }
}

/** 
 * Function: reallocArray
 * Use: Finds the NULL elements in an array and allocate memory to it (used for
 * the initial array)
 * Args: 
 *      1. argsarr: array to be operated on
*/
void reallocArray(char *argsarr[]){  
    for (int i =0; i< MAX_LINE/2; i++){
        if (argsarr[i] == NULL){
            argsarr[i]= (char *) malloc((MAX_LINE/2)*sizeof(char));
        }
    }
}

/** 
 * Function: splitArray
 * Use: Split an array into two arrays if a redirection or a pipe is to be involved
 * Args: 
 *      1. argsarr[]: The initial array to be splited at the token for a redirection
 * or a pipe
 *      2. argsarr2[]: The second array which will hold the file for redirection or
 * command for a pipe
 *      3. taskSignPos: The index of where the redirection/pipe token is in argsarr[] 
*/
void splitArray(char *argsarr[],char *argsarr2[], int taskSignPos){
    // All elements from index in taskSignPos will hold NULL pointers. 
    // Meanwhile tokens after argsarr[taskSignPos] will be shifted to argsarr2
    free(argsarr[taskSignPos]);     // free memory at pipe/redirection token
    argsarr[taskSignPos] = NULL;    // make the array element above hold null pointer
    int i = taskSignPos+1;          // i holds index for argsarr
    int j = 0;                      // j holds index for argsarr2

    // transfer the of tokens for redirection in argsarr to argsarr2
    while(argsarr[i]!=NULL){    
        argsarr2[j] = argsarr[i];
        argsarr[i] = NULL; // avoid dangling pointers
        i++;
        j++;
    }

    freeSpares(argsarr, i, MAX_LINE/2); // free any unused dynamically allocated memory left in argsarr
}

/** 
 * Function: getArgsForExec
 * Use: receive the command and convert it into an array of tokens
 * Args: 
 *      1. args: command string
 *      2. argsarr: array of tokens
 *      3. taskSignPos: pointer to the variable that is the index of the pipe/redirection token in argsarr
 *      4. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 *      5. taskFlag: pointer to the variable (created in main()) that signal whether redirection/piping
 * is involved
 *      6. hisargs: recent command string 
 * Return:
 *      1. returns 0 if the command for history feature (recent command) is entered
 * but no valid command has been enter previously
 *      2. return 1 otherwise
*/
int getArgsForExec(char args[], char *argsarr[], int *taskSignPos, int *waitFlag, int *taskFlag, char hisargs[]){

        fgets(args, MAX_LINE, stdin); // receive user's input
        fflush(stdin);
        if (strcmp(args, "\n") == 0){ // No previous command
            printf("-osh: No command has been entered.\n");
            return 0;
        } else if (strcmp(args, "exit\n") == 0){ // if the command is "exit", return the corresponding value
            return EXITSHELL;
        }

        int recent = 0; // signals if the last recent command is to be executed
        // obtain the arguments (tokens) array from the command and set flags  
        *taskFlag = getArgs(args,argsarr,taskSignPos, waitFlag, &recent);

        if (recent == 1){ // if the last recent command is to be executed
            if (strlen(hisargs) == 0){ // if no previous command is available, return 0
                printf("-osh: No previous command has been entered.\n");
                return 0;
            }
            strcpy(args, hisargs); // copy the last recent command to the current command string
            // obtain the arguments (tokens) array from the command and set flags
            printf("-osh: running: %s",hisargs);
            *taskFlag = getArgs(args,argsarr,taskSignPos, waitFlag, &recent);
        }
        return 1;
}

/** 
 * Function: getArgs
 * Use: sets up for a conversion a string into an array of tokens and obtains flags
 * Args: 
 *      1. args: command string
 *      2. argsarr: array for tokens
 *      3. taskSignPos: pointer to the variable that is the index of the pipe/redirection token in argsarr
 *      4. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 *      5. recent: pointer to the variable that signals whether history feature is 
 * used ("!!")
 * Return:
 *      1. returns an integer signaling if a redirection/pipe is in the command
*/
int getArgs(char args[], char *argsarr[], int * taskSignPos, int *waitFlag, int * recent){ 
        reallocArray(argsarr); // Allocate memory to arguments (token) array
        int taskFlag = NORMAL; // variable to return value for taskFlag
        char args1[MAX_LINE] = ""; 
        strcpy(args1,args);  // copy of args

        // // trim the \n from the command line
        int lenarg = strlen(args1);
        args1[lenarg-1]='\0';
        
        // produce argument arrays and get flags (task, wait, recent)
        getArgsAndFlags(args1, argsarr, &taskFlag, taskSignPos, waitFlag, recent);

        // return value for the task type
        return taskFlag;
}

/** 
 * Function: getArgsAndFlags
 * Use: convert a string (args) into an array of tokens while setting taskFlag, waitFlag, recent and finding the position for the pipe/redirection token
 * Args: 
 *      1. args: command string to be converted
 *      2. argsarr: array of tokens
 *      3. taskFlag: pointer to the variable (created in getArgs()) that signal whether redirection/piping
 * is involved
 *      4. taskSignPos: pointer to the variable that is the index of the pipe/redirection token in argsarr
 *      5. waitFlag: pointer to the variable that signals whether the wait option
 * is selected
 *      6. recent: pointer to the variable that signals whether history feature is 
 * used ("!!")
*/
void getArgsAndFlags(char args[], char *argsarr[], int *taskFlag, int *taskSignPos, int *waitFlag, int *recent){
        // get the token
        char *token = "";   // variable to get the tokens from args
        token = strtok(args," "); // get the tokens from args delimited by " "
        
        //Variable to keep count of while loop iterations
        int i=0;

        // copy the tokens to arguments (tokens) array
        while(token !=NULL){
            // copy current token into current array index
            strcpy(argsarr[i], (char *) token);
            token = strtok(NULL," ");
            
            // check to see if the token indicates task.
            if (strcmp(argsarr[i],"<") == 0){
                *taskFlag = INREDIRECT; // set flag for task type
                *taskSignPos = i; // set position for redirection/pipe token in argsarr
            } else if (strcmp(argsarr[i],">") == 0){
                *taskFlag = OUTREDIRECT;
                *taskSignPos = i;
            } else if (strcmp(argsarr[i],"|") == 0){
                *taskFlag = PIPE;
                *taskSignPos = i;
            }
            i++;
        }
        
        // Check to see if there is a non-wait option
        if (strcmp(argsarr[i-1],"&")==0){
            *waitFlag = 0; // set flag if non-wait option is involved
            free(argsarr[i-1]); // free token "&"
            argsarr[i-1]=NULL;  // avoid dangling pointer
        }

        // Free unneeded memory
        freeSpares(argsarr,i, MAX_LINE/2);

        // Make sure the last element is NULL no matter what happen
        free(argsarr[MAX_LINE/2]);
        argsarr[MAX_LINE/2]= NULL;

        // Check to see if the command wants to execute a recent command
        if (!strcmp(argsarr[0], "!!") && argsarr[1]==NULL){
            *recent = 1; // set the flag for the recent option to be 1
        }
}

