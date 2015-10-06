#include "kbcommands.h"
#include "communicator.h"

int main(int argc, char **argv)
{
  char programType[10];
  int portNo;
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

	printf("ok");
  prepareCommandPatterns();
	printf("ok1");
  printf("\nStarted %s on port %d\n", programType, portNo);

	setup_server_socket(argv[2]);
  return 0;
}
