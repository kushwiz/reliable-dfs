#include "kbcommands.h"
#include "communicator.h"

enum Commands Server_Commands[] = {HELP,CREATOR,DISPLAY,LIST,QUIT};
enum Commands Client_Commands[] = {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

char commandPatterns[CMDCOUNT][500];

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

void prepareCommandPatterns()
{
  strcpy(commandPatterns[HELP], "^HELP$");
  strcpy(commandPatterns[CREATOR], "^CREATOR$");
  strcpy(commandPatterns[DISPLAY], "^DISPLAY$");
  strcpy(commandPatterns[REGISTER], "^REGISTER (.+) ([0-9]+)$");
  strcpy(commandPatterns[CONNECT], "^CONNECT (.+) ([0-9]+)$");
  strcpy(commandPatterns[LIST], "^LIST$");
  strcpy(commandPatterns[TERMINATE], "^TERMINATE ([0-9]+)$");
  strcpy(commandPatterns[QUIT], "^QUIT$");
  strcpy(commandPatterns[GET], "^GET ([0-9]+) (.+)$");
  strcpy(commandPatterns[PUT], "^PUT ([0-9]+) (.+)$");
  strcpy(commandPatterns[SYNC], "^SYNC$");
}

void doCreator()
{
  printf("Name: Kushal Bhandari\nUBIT name: kbhandar\nUB email: kbhandar@buffalo.edu\n");
}

void doList()
{
  struct connectionInfo *itr;
  itr = startPtr;

  while(itr!=NULL)
  {
    printf("%s|||%d\n",itr->clientAddress,itr->sockfd);
    itr = itr->next;
  }

}
