#include "kbcommands.h"
#include "communicator.h"
#include<pthread.h>

int currentCommand;

int main(int argc, char **argv)
{
  pthread_t tid[2];
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

  if(setup_server_socket(argv[2]) == 0)
  {
    // Start listener thread
    if(pthread_create(&tid[0], NULL, &server_socket_runner, NULL)!=0) {
      printf("thread error\n");
      return 1;
    } else {
      printf("thread created successfully!\n");
    }
  }
  do
  {
    scanf("%s", userInput);
    executeCommand(userInput);
  }while(currentCommand!=QUIT);

  return 0;
}

void executeCommand(char *userInput)
{
 unsigned char buf[256];
 int packetsize;
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
	  printf("DO HELP\n");
	  return;
	  break;
	case CREATOR:
	  doCreator();
	  return;
	  break;
	case DISPLAY:
	  packetsize = pack(buf, "hs", DISPLAY, "Hello World!");
	  packi16(buf+1, packetsize);
	  printf("buffer:%s\n",buf);
	  send_data_via_socket("127.0.0.1", "8900", buf, packetsize);
	  return;
	case REGISTER:
	  printf("DO REGISTER\n");
	  return;
	  break;
	case CONNECT:
	  break;
	case LIST:
	  printf("DO LIST\n");
	  doList();
	  return;
	  break;
	case TERMINATE:
	  break;
	case QUIT:
	  close_all_server_connections();
	  return;
	  break;
	case GET:
	  break;
	case PUT:
	  break;
	case SYNC:
	  break;
      }
      //			printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
      //			printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
    }
  }
  printf("Invalid command see HELP\n");
}
