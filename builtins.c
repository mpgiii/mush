#include "builtins.h"

/* changes the working directory */
/* requires two arguments. */
int my_cd(int argc, char* argv[]) {
   if (argc < 2) {
      fprintf(stderr, "cd: missing argument.\n");
      return -1;
   }
   
   if (argc > 2) {
      fprintf(stderr, "cd: too many arguments.\n");
      return -1;
   }
   
   if (chdir(argv[1]) == -1) {
      perror(argv[1]);
      return -1;
   }

   return 0;
}

/* exits the program */
int my_exit() {
   exit(-1);
}
