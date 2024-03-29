/* Miscellaneous functions involving line parsing,
 * command running, and child processes. This is
 * used in conjunction with mush.c. */

#include "misc.h"

#define LEN_OF_IN_PIPE 18
#define LEN_OF_OUT_PIPE 16


/* Modifed from my Assignment 5: parseline. Goes
 * through each line in a list of lines and creates
 * structures for easier handling when actually
 * running commands. Stores the commands in the passed
 * in "pipeline" variable.
 * Also creates the pipes which will be used in the case
 * of a pipeline being passed. */
int parselines(char* lines[], int count, struct command pipeline[]) {
   int i, j, argc = 0;
   char* argv[COMMAND_ARGS];
   char* line;
   const char s[7] = " \n\t\v\f\r";
   int pipefds[COMMAND_ARGS][2];
   
   int input_flag = 0;
   int output_flag = 0;
   
   /* go through each line */
   for (i=0; i < count; i++) {

      snprintf(pipeline[i].line, sizeof(pipeline[i].line), "%s", lines[i]);
      pipeline[i].input = -1;
      pipeline[i].output = -1;
      
      /* get the first command */
      line = strtok(lines[i], s);
      while (line != NULL) {
         /* if there is an input redirect symbol, set the
          * input flag and go to the next string to find out
          * what the input is */
         if (NULL != strchr(line, '<')) {
            input_flag = 1;
            line = strtok(NULL, s);
            continue;
         }
         
         /* if there is an output redirect symbol, same as
          * input stuff up there ^^^ */
         if (NULL != strchr(line, '>')) {
            output_flag = 1;
            line = strtok(NULL, s);
            continue;
         }
         
         /* if the input flag is set: */
         if (input_flag) {
            /* if we're not in the first thing in our pipeline, that is
             * an ambiguous input. bad stuff */
            if (i != 0) {
               fprintf(stderr, "%s: ambiguous input\n", argv[0]);
               return -1;
            }
            /* if not, set the input in our command to this and
             * reset the input flag */
            pipeline[i].input = open(line, O_RDONLY);
            if (pipeline[i].input == -1) {
               perror(line);
               return -1;
            }
            input_flag = 0;
            line = strtok(NULL, s);
            continue;
         }

         /* if the output flag is set: (see comments above for input */         
         if (output_flag) {
            if (i != (count - 1)) {
               fprintf(stderr, "%s: ambiguous output\n", argv[0]);
               return(-1);
            }
            pipeline[i].output = open(line, O_WRONLY | O_CREAT | O_TRUNC, 
                                            S_IRUSR | S_IWUSR | S_IRGRP | 
                                            S_IWGRP | S_IROTH | S_IWOTH);
            if (pipeline[i].output == -1) {
               perror(line);
               return(-1);
            }
            output_flag = 0;
            line = strtok(NULL, s);
            continue;
         }
         
         /* set this spot in the argv to this line */
         argv[argc] = line;
         
         /* set this spot in the command's argv data */
         pipeline[i].argv[argc] = (char*) malloc(strlen(argv[argc]) + 1);
         snprintf(pipeline[i].argv[argc], strlen(argv[argc]) + 1, "%s", 
                  argv[argc]);
         
         /* increment the number of arguments */
         argc++;
         
         /* if we have too many arguments we can just make that an error
          * (thanks, Professor Nico!) */
         if (argc > COMMAND_ARGS) {
            fprintf(stderr, "Too many arguments.\n");
            return(-1);
         }
         
         /* go to the next argument */
         line = strtok(NULL, s);
         
      }

      /* once we hit here, we're done with this particular command. We
       * just need to put all its stuff into its structure for use later */
      
      /* set the argc */
      /* if our final argc was zero, there was an invalid null command. */
      if (argc == 0) {
         fprintf(stderr, "invalid null command\n");
         return(-1);
      }
      pipeline[i].argc = argc;
      
      /* null terminate our argv */
      pipeline[i].argv[argc] = NULL;
      
      /* if we have pipes and we're not on the last command,
       * create a pipe. */
      if ((count > 1) && (i != (count - 1))) {
         if (-1 == pipe(pipefds[i])) {
            perror("pipe");
            exit(-1);
         }
      }
      
      /* if the input was not already redefined, define it */
      if (-1 == pipeline[i].input) {
         if (i == 0) {
            /* if we're on the first in the pipeline, stdin is the input */
            pipeline[i].input = STDIN_FILENO;
         }
         else {
            /* otherwise, the input is the pipe from the last command. */
            pipeline[i].input = pipefds[i-1][0];
         }
      }
      
      /* if the output was not already redefined, define it */
      if (-1 == pipeline[i].output) {
         if (i == (count - 1)) {
            /* if we're on the last in the pipeline, stdout is the output */
            pipeline[i].output = STDOUT_FILENO;
         }
         else {
            /* otherwise, the output is the pipe to the next command. */
            pipeline[i].output = pipefds[i][1];
         }
      }

      /* clear out our argc and argv for the next line */
      argc = 0;
      for (j = 0; j < COMMAND_ARGS; j++)
         argv[j] = NULL;
   }
   
   return 0;
}

/* note: the function below was used in the parseline
 * project. It does not work in this implementation of 
 * line parsing, as the struct used for a command has
 * been slightly modified. However, I have left it here,
 * commented out, in case I need it for troubleshooting
 * in the future.
 */

/*
int printerheader(int stage, struct command com) {
   int i;

   printf("--------\n");
   printf("Stage %d: \"%s\"\n", stage, com.line);
   printf("--------\n");
   printf("%10s: %s\n", "input", com.input);
   printf("%10s: %s\n", "output", com.output);
   printf("%10s: %d\n", "argc", com.argc);
   printf("%10s: ", "argv");
   for (i=0; i < com.argc; i++) {
      if (i != 0)
         printf(",");
      printf("\"%s\"", com.argv[i]);
   }
   printf("\n");
   return 0;
}
*/



/* Goes through each of the commands in the pipeline,
 * creates child processes for them, and runs them
 * all. Also sets the SIGINT signal to be ignored
 * in order to kill child processes without blowing up
 * our shell as well. */
int run_commands(int count, struct command pipeline[]) {
   int i, j;
   int status;
   int newcount;
   
   /* ignore sigint while we run our children */
   signal(SIGINT, SIG_IGN);

   newcount = count;

   for (i = 0; i < count; i++) {


      /* also checks for "cd" and "exit" to handle with my built-ins */
      if (0 == strcmp(pipeline[i].argv[0], "cd")) {
         newcount--;
         my_cd(pipeline[i].argc, pipeline[i].argv);
         signal(SIGINT, SIG_DFL);
         return 1;
      }
      else if ((0 == strcmp(pipeline[i].argv[0], "exit")) ||
               (0 == strcmp(pipeline[i].argv[0], "quit")) ||
               (0 == strcmp(pipeline[i].argv[0], "logout"))) {
         exit(-1);
      }

      /* if it's neither of these, fork out */
      else if (fork() == 0) {
         /* child */

         /* make the stdin and stdout match what inputs and
          * outputs were previously specified by user */
         if (-1 == dup2(pipeline[i].input, STDIN_FILENO)) {
            perror("dup2");
            signal(SIGINT, SIG_DFL);
            exit(-1);
         }
         if (-1 == dup2(pipeline[i].output, STDOUT_FILENO)) {
            perror("dup2");
            signal(SIGINT, SIG_DFL);
            exit(-1);
         }

         /* close all open file descriptors */
         for (j = 0; j < count; j++) {
            if (pipeline[j].input != STDIN_FILENO)
               close(pipeline[j].input);
            if (pipeline[j].output != STDOUT_FILENO)
               close(pipeline[j].output);
         }

         /* reenable our signal for the child to handle */
         signal(SIGINT, SIG_DFL);
         
         /* and finally execute the command given */
         if (-1 == execvp(pipeline[i].argv[0], pipeline[i].argv)) {
            perror(pipeline[i].argv[0]);
            signal(SIGINT, SIG_DFL);
            exit(-1);
         }
         
         return 0;
      }

   }

   /* parent */

   /* close all open file descriptors */
   for (j = 0; j < count; j++) {
      if (pipeline[j].input != STDIN_FILENO)
         close(pipeline[j].input);
      if (pipeline[j].output != STDOUT_FILENO)
         close(pipeline[j].output);
   }

   for (j = 0; j < newcount; j++) {
      /* wait for all our beautiful children */
      if (-1 == wait(&status)) {
         perror("wait");
         signal(SIGINT, SIG_DFL);
         exit(-1);
      }

   }
   
   /* re-enable the signal after the children have run */
   signal(SIGINT, SIG_DFL);
   
   return 0;
}
