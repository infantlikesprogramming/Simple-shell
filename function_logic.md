## `main()` logic:
In the main function, variables for holding the command, previous command, and tokens array will be declared and initialized.

There will then be a loop where a command is fetched by `getArgsForExec()` and converted into an array of tokens delimited by “ “. If the command is valid its token array will be passed into comExec() for the command to be executed. 
Flags for how the fetched command string should be executed will also be declared and initialized in the loop in main() and will be passed into getArgsForExec() to be modified and then passed into `comExec()` to help execute the commands accordingly. The flags variables will give information on whether a feature is required (e.g. piping, IO redirection, background, history)

## Trio of `getArgsForExec()`, `getArgs()`, and `getArgsAndFlags()`:
In `getArgsForExec()`, a command is fetched and processed to run in the way specified by the background or history feature provided by the shell, and passed into `getArgs()` to extract the command to be executed into an array of arguments. `getArgs()` set up the token array passed into it for holding tokens of the command. It also clones the command string for the tokenization of the arguments in the command while leaving the original command intact. After the setup mentioned above, it calls `getArgsAndFlags()` for the conversion and also the obtainment of the value of the flags (which decides how the command should be executed). `getArgsAndFlags()` uses `strtok()` to extract tokens from the command string. These tokens will be added to the token array. At the same time, tokens will be evaluated to decide what features are involved in the execution of the command. 

## Execution logic:
When flags (features indicators) and the token array are passed into `comExec()`, the function (`comExec()`) will create a new array needed for IO redirection or pipe features. It then calls comExecNormal() or comExecRedirect, or comExecPipe() to execute the command according to the features involved. At the point of comExec, the token array can only include command tokens, and one of the three feature tokens (IO redirection tokens and the pipe token).

### Normal execution logic
Execution of commands without IO redirection or pipe features will be done in `comExecNormal()`. When comExecNormal() is called, the initial token array is the argument array since it does not have any IO redirection or pipe tokens. A child process will be forked where `execvp()` and the arguments array will be used to run a task (the command).

### IO redirection logic:
Execution of commands with the IO redirection feature will be done in `comRedirectExec()`. At this point, the token array contains tokens for the command, IO redirection, and the file for IO redirection. First, `splitArray()` is called to remove non-argument tokens from the token array, turning it into an argument array, while transferring the token for the file for IO redirection into the second array (created in `comExec()`). A children's process is then forked. In the children process, `dup2()` is used to make the file descriptor of the stdin/stdout stream (according to the IO redirection being taking the input from a file or outputting into a file) point to the file for IO redirection. `execvp()` and the arguments array will be used to run the specified command. At the moment, the name for the file for IO redirection is held in the first element of the feature/second array instead of a variable. The reason for this is that this can be an advantage when a person reimplements the shell with a multiple-file IO redirection feature in mind.

### Pipe logic:
Execution of commands with the pipe feature will be set up in `comPipeExec()`. At this point, the token array contains tokens for the 2 commands (piped together) and a pipe token. First, `splitArray()` is called to remove argument tokens after the pipe and the pipe token from the token array, turning this array into an argument array for the first command (before the pipe token) while transferring the token for the second command (after the pipe token) into the second array*. A children process will be forked. In the children process, `execPipe()` is called (the two argument arrays are passed in as well) to execute the command containing the pipe feature.

In `execPipe()`, the first command will be executed and the output will be redirected into the stdin stream in the execution of the second command. A pipe is created using `pipe()`. A children process is then forked. The plan is to execute the first command in the children process and communicate the output to the parent process, where it can be used as the input for the second command (the command after the pipe token). In the children process, `dup2()` is used to help send the output of the first command into the pipe, while `execvp()` and the first arguments array will be used to run the first command. The parent process will wait for the children process to complete. `dup2()` will then be used to redirect the message from the pipe to the stdin stream of the parent process. `execvp()` and the first arguments array will be used to run the second command.

## Background and history features:

The background feature is enabled by the use of `waitpid()` the parent (main) process.

The history feature is enabled by checking for the command `!!` and execute the command in the history buffer (the previous command) if `!!` is entered.
