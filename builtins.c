#include "builtins.h"

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

int my_exit() {
   exit(-1);
}
