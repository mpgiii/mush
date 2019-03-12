/* parseline
 * A program that parses the input of a command line
 * and stores them as a list of command structures
 * which contain each command's argc, argv, input, and
 * output.
 *
 * Written by Michael Georgariou for 
 * CPE 357 with Professor Nico at
 * Cal Poly SLO.                                   */

#include "misc.h"

int main(int argc, char* argv[]) {
   
   int i, c, length;
   char input[LINE_LENGTH + 1];
   FILE* infile;
   
   if (argc != 1) {
      if (argc > 2) {
         fprintf(stderr, "%s: too many arguments.\n", argv[0]);
         return -1;
      }
      if (NULL == (infile = fopen(argv[1], "r"))) {
         perror("fopen");
         return -1;
      }
   }
   else {
      infile = stdin;
   }

   
   while (1) {
      length = 0;
      if (infile == stdin) {
         printf("8-P ");
         fflush(stdout);
      }

      /* grab the input from stdin */
      while ((c = fgetc(infile)) != '\n' && c != EOF) {
         if (length > LINE_LENGTH) {
            fprintf(stderr, "command too long\n");
            exit(EXIT_FAILURE);
         }
         else {
            input[length++] = c;
         }
      }
      input[length] = '\0';

      /* if the user inputted nothing, go away */
      if (length == 0) {
         fprintf(stderr, "nothing to read.\n");
         continue;
      }
      
      helper(input, length);
      
      for (i = 0; i < LINE_LENGTH; i++) {
         input[i] = '\0';
      }
      
      if (infile != stdin) {
         break;
      }
      
   }
   
   return 0;
}

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
            printf("%c\n", *(end-1));
            fprintf(stderr, "invalid null command\n");
            exit(EXIT_FAILURE);
         }
         /* store the line in our list of lines */
         lines[j] = (char *) calloc(end - start + 1, sizeof(char));
         strncpy(lines[j++], start, end - start);
      }
      /* get ready to find next pipe */
      start = end + 1;
      
   }
   
   if (j > PIPE_COMMANDS) {
      fprintf(stderr, "pipeline too deep\n");
      exit(-1);
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
