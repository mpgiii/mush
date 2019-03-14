/* mush
 * A minimally useful shell. Can run all commands
 * available to it in its given path.
 * The shell also supports pipelines, and features
 * both an interactive and a batch processing mode.
 *
 * This shell is limited to a maximum pipeline size
 * of 10, a maximum amount of arguments to a given
 * function of 10, and a maximum line length of 512.
 * These values can be adjusted in the misc header.
 *
 * Mush also has support for redirection of the
 * input and output of a file, using ">" and "<"
 * like one would in BASH.
 *
 * The shell also handles an interrupt signal.
 * If the signal is generated during the running of a
 * child process, it will terminate the child but
 * continue running the shell. However, if no children
 * are being run and the shell receives an interrupt
 * signal, it will close the shell.
 *
 * Written by Michael Georgariou for 
 * CPE 357 with Professor Nico at
 * Cal Poly SLO.                                   */

#include "misc.h"


int helper(char input[], int length) {
   char* lines[PIPE_COMMANDS];
   struct command pipeline[PIPE_COMMANDS];
   int j = 0;

   char *start, *end;

   /* set our start pointer to the beginning of the input */
   start = input;

   while (1) {
      /* breaks once we hit the end of the input */
      /* populates lines to be parsed later */

      /* set the end pointer to the first occurance of a pipe */
      end = strchr(start, '|');

      /* if there are no more pipes */
      if (end == NULL) {
         /* set the end pointer to the end of our input */
         end = &input[length];
         /* store the line in our list of lines */
         lines[j] = (char *) calloc(end - start + 1, sizeof(char));
         strncpy(lines[j++], start, end - start);
         /* and we are done */
         break;
      }
      else {
         /* if the pipe is not surrounded by whitespace, exit */
         if (!isspace(*(end-1)) || !isspace(*(end+1))) {
            fprintf(stderr, "invalid null command.\n");
            return -1;
         }
         /* store the line in our list of lines */
         lines[j] = (char *) calloc(end - start + 1, sizeof(char));
         strncpy(lines[j++], start, end - start);
      }
      /* get ready to find next pipe */
      start = end + 1;
      
   }
   
   if (j > PIPE_COMMANDS) {
      fprintf(stderr, "Pipeline too deep.\n");
      return(-1);
   }
   
   /* once we've gotten here, lines is populated and ready to be parsed */

   if (-1 == parselines(lines, j, pipeline)) {
      return -1;
   }
   
   if (-1 == run_commands(j, pipeline)) {
      return -1;
   }
   
   return 0;
}


int main(int argc, char* argv[]) {
   
   int i, c = 0, length;
   char input[LINE_LENGTH + 1];
   FILE* infile;
   
   /* parse input file, if given */
   if (argc != 1) {
      /* only expect two arguments, max */
      if (argc > 2) {
         fprintf(stderr, "%s: too many arguments.\n", argv[0]);
         return -1;
      }
      if (NULL == (infile = fopen(argv[1], "r"))) {
         perror("fopen");
         return -1;
      }
   }
   /* if there was no input, set stdin as our input */
   else {
      infile = stdin;
   }

   
   /* loop in here until the exit command is typed */
   while (1) {
      length = 0;
      
      /* if we were are the end of the file: */
      if (c == EOF) {
         /* and stdin is our input, reset. */
         if (infile == stdin) {
            printf("\n");
         }
         /* and something else is our input, stop doing stuff */
         else {
            break;
         }
      }
      
      /* print the prompt (but only if using stdin) */
      if (infile == stdin) {
         printf("8-P ");
         fflush(stdout);
      }
      

      /* grab the input from stdin */
      while (((c = fgetc(infile)) != '\n') && (c != EOF)) {
         if (length > LINE_LENGTH) {
            fprintf(stderr, "command too long\n");
            exit(EXIT_FAILURE);
         }
         else {
            input[length++] = c;
         }
      }
      /* null terminate (so we can treat it like a string */
      input[length] = '\0';

      /* if the user inputted nothing, go away */
      if (length == 0) {
         continue;
      }
      
      /* handle the input. see above function */
      helper(input, length);
      
      /* clear the buffer for the next command */
      for (i = 0; i < LINE_LENGTH; i++) {
         input[i] = '\0';
      }      
   }
   
   return 0;
}
