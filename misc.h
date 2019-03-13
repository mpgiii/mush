#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "builtins.h"

#define LINE_LENGTH 512
#define PIPE_COMMANDS 10
#define COMMAND_ARGS 10

struct command {
   char* argv[COMMAND_ARGS];
   char line[LINE_LENGTH];
   int argc;
   int input; /* file descriptor */
   int output; /* file descriptor */
};

extern int parselines(char* lines[], int count, struct command pipeline[]);

extern int printerheader(int stage, struct command com);

extern int run_commands(int count, struct command pipeline[]);
