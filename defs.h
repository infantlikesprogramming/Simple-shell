#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define MAX_LINE 80 // max length command
#define NORMAL 10
#define INREDIRECT 11
#define OUTREDIRECT 12
#define PIPE 13
#define READ_END 0
#define WRITE_END 1
#define EXITSHELL 5