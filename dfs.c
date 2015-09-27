#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>
#include "kbcommands.h"

#define REGISTER_CMD_REGEX "^REGISTER (.+) ([0-9]+)$"
#define CREATOR_CMD_REGEX "^CREATOR$"
#define DISPLAY_CMD_REGEX "^DISPLAY$"
#define CONNECT_CMD_REGEX "^CONNECT (.+) ([0-9]+)$"
#define LIST_CMD_REGEX "^LIST$"
#define TERMINATE_CMD_REGEX "^TERMINATE ([0-9]+)$"
#define QUIT_CMD_REGEX "^QUIT$"
#define GET_CMD_REGEX "^GET ([0-9]+) (.+)$"
#define PUT_CMD_REGEX "^PUT ([0-9]+) (.+)$"
#define SYNC_CMD_REGEX "^SYNC$"

int main(int argc, char **argv) {

  char programType[10];
  char userInput[100];
  int portNo;
  regex_t re;
  regmatch_t rm[3];
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

	printf("Can I execute REGISTER(%d) on %s: %d", REGISTER, programType, canExecute(isClient,REGISTER));

  printf("\nStarting %s on port %d", programType, portNo);

  return 0;

}
