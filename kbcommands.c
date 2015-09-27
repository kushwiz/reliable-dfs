#include "kbcommands.h"

enum Commands Server_Commands[] = {HELP,CREATOR,DISPLAY,LIST,QUIT};
enum Commands Client_Commands[] = {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

int canExecute(int isClient, enum Commands cmd)
{
  enum Commands *arr;
	int i=0, size=0;;
	if(isClient)
	{
		arr = &Client_Commands[0];
		size = ARLEN(Client_Commands);
	}
	else
	{
		arr = &Server_Commands[0];
		size = ARLEN(Server_Commands);
	}
	while(i<size)
	{
		if(*(arr+i)==cmd)
		{
			return EXIT_SUCCESS;
		}
		i++;
	}
	return EXIT_FAILURE;
}

int prepareRegex()
{
	regex_t re;
  regmatch_t rm[3];
  if (regcomp(&re, REGISTER_CMD_REGEX, REG_EXTENDED) != 0) {
    fprintf(stderr, "Failed to compile regex '%s'\n", REGISTER_CMD_REGEX);
    return EXIT_FAILURE;
  }
	return EXIT_SUCCESS;
}

int findCommand(char *userInput)
{

  if (regexec(&re, userInput, 3, rm, 0) == 0) {
    printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
    printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
  } else {
    printf("Invalid Command, See HELP");
  }

}
