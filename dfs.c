#include "kbcommands.h"

int isClient = 0;
int currentCommand;

int main(int argc, char **argv)
{
  char programType[10];
  char userInput[100];
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

	prepareCommandPatterns();
  printf("\nStarting %s on port %d", programType, portNo);

	do
	{
	printf("\n> ");
	scanf("%s", userInput);
	executeCommand(userInput);
	}while(currentCommand!=QUIT);

  return 0;
}

void executeCommand(char *userInput)
{
  regmatch_t rm[3];
	for(int i=0; i<CMDCOUNT; i++)
	{
		regex_t re;
  	if (regcomp(&re, commandPatterns[i], REG_EXTENDED) != 0)
		{
  	  fprintf(stderr, "Failed to compile regex '%s'\n", commandPatterns[i]);
  	}

		if (regexec(&re, userInput, 3, rm, 0) == 0)
		{ currentCommand = i;
			switch(i)
			{
				case HELP:
					printf("DO HELP");
					return;
				break;
				case CREATOR:
				break;
				case DISPLAY:
				break;
				case REGISTER:
					printf("DO REGISTER");
					return;
				break;
				case CONNECT:
				break;
				case LIST:
					printf("DO LIST");
					return;
				break;
				case TERMINATE:
				break;
				case QUIT:
				break;
				case GET:
				break;
				case PUT:
				break;
				case SYNC:
				break;
			}
//	    printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
//	    printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
	  }
	}
	printf("Invalid command see HELP\n");
}
