#include "kbcommands.h"

int main(int argc, char **argv) {

  char programType[10];
  char userInput[100];
  int portNo;
  int isClient = 0;

  if(argc != 3) {
    printf("Usage:");
    printf("./dfs <type> <port>");
    return 0;
  } else {
    portNo = atoi(argv[2]);
    strcpy(programType, argv[1]);
    if((strcmp(programType, "s")!=0) && (strcmp(programType,"c")!=0)) {
      printf("Usage:");
      printf("./dfs <type> <port>");
      return 0;
    } else {
      if(strcmp(programType,"c")==0) {
        isClient = 1;
      }
    }
  }



  strcpy(userInput, "REGISTER hello 12");

	printf("Can I execute REGISTER(%d) on %s: %d", REGISTER, programType, canExecute(isClient,REGISTER));

  printf("\nStarting %s on port %d", programType, portNo);

  return 0;

}
