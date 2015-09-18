#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>

#define REGISTER_CMD_REGEX "^REGISTER (.+) ([0-9]+)$"

int main(int argc, char **argv) {

  char programType[10];
  char userInput[100];
  int portNo;
  regex_t re;
  regmatch_t rm[3];

  if(argc != 3) {
    printf("Usage:");
    printf("./dfs <type> <port>");
    return 0;
  } else {
    portNo = atoi(argv[2]);
    strcpy(programType, argv[1]);
    if((strcmp(programType, "server")!=0) && (strcmp(programType,"client")!=0)) {
      printf("Usage:");
      printf("./dfs <type> <port>");     
      return 0;
    }
  }

  if (regcomp(&re, REGISTER_CMD_REGEX, REG_EXTENDED) != 0) {
    fprintf(stderr, "Failed to compile regex '%s'\n", REGISTER_CMD_REGEX);
    return EXIT_FAILURE;
  }

  strcpy(userInput, "REGISTER hello 12");

  if (regexec(&re, userInput, 3, rm, 0) == 0) {
    printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
    printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
  } else {
    printf("Invalid Command, See HELP");
  }

  printf("\nStarting %s on port %d", programType, portNo);
  
  return 0;

}
