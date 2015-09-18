#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>

#define REGISTER "^REGISTER ^(.+) ([0-9]+)$"

int main(int argc, char **argv) {

  char programType[10];
  char userInput[100];
  int portNo;
  regex_t re;
  regmatch_t rm[2];

  if(argc != 3) {
    printf("Usage:");
    printf("./dfs <program> <port>");
    return 0;
  } else {
    portNo = atoi(argv[2]);
    strcpy(programType, argv[1]);
    if((strcmp(programType, "server")!=0) && (strcmp(programType,"client")!=0)) {
      printf("Usage:");
      printf("./dfs <program> <port>");     
      return 0;
    }
  }

  if (regcomp(&re, REGISTER, REG_EXTENDED) != 0) {
    fprintf(stderr, "Failed to compile regex '%s'\n", REGISTER);
    return EXIT_FAILURE;
  }

  strcpy(userInput, "REGISTER hello 12");

  regexec(&re, userInput, 2, rm, 0);
  printf("<<%d>>", rm[0].rm_eo);

  printf("\nStarting %s on port %d", programType, portNo);
  
  return 0;

}
