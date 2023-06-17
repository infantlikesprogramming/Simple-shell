// utility functions
void printarray(char *argsarr[]);
int prompt();
void freeSpares(char *argsarr[], int start, int end);
void reallocArray(char *argsarr[]);
void splitArray(char *argsarr[],char *argsarr2[], int taskSignPos);

int getArgsForExec(char args[], char *argsarr[], int *taskSignPos, int *waitFlag, int *taskFlag, char hisargs[]);
int getArgs(char *args1, char *argsarr[], int * taskSignPos, int *waitFlag, int * recent);
void getArgsAndFlags(char args[], char *argsarr[], int *taskFlag, int *taskSignPos, int *waitFlag, int *recent);

// Function to run commands
void comExec(char *argsarr[], int waitFlag, int taskFlag, int taskSignPos);
void comExecNormal(char *argsarr[], int waitFlag);
void comExecRedirect(char *argsarr[], char *argsarr2[] , int waitFlag, int taskFlag,int taskSignPos);
void comExecPipe(char *argsarr[], char *argsarr2[], int waitFlag, int taskSignPos);
int execPipe(char *argsarr[], char *argsarr2[], int waitFlag);
