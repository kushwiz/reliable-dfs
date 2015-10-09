#include "kbcommands.h"
#include "communicator.h"
#include "datastructure.h"

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
  strcpy(commandPatterns[HELP], "^HELP\n");
  strcpy(commandPatterns[CREATOR], "^CREATOR\n");
  strcpy(commandPatterns[DISPLAY], "^DISPLAY\n");
  strcpy(commandPatterns[REGISTER], "^REGISTER (.+) ([0-9]+)\n");
  strcpy(commandPatterns[CONNECT], "^CONNECT (.+) ([0-9]+)\n");
  strcpy(commandPatterns[LIST], "^LIST\n");
  strcpy(commandPatterns[TERMINATE], "^TERMINATE ([0-9]+)\n");
  strcpy(commandPatterns[QUIT], "^QUIT\n");
  strcpy(commandPatterns[GET], "^GET ([0-9]+) (.+)\n");
  strcpy(commandPatterns[PUT], "^PUT ([0-9]+) (.+)\n");
  strcpy(commandPatterns[SYNC], "^SYNC\n");
}

void doCreator()
{
  printf("Name: Kushal Bhandari\nUBIT name: kbhandar\nUB email: kbhandar@buffalo.edu\n");
}

void doList()
{
  struct connectionInfo *itr;
  itr = startPtr;
  printf("id:\tHostname\t\t\tIP address\tPort No.\n");
  int i = 1;
  while(itr!=NULL)
  {
    printf("%d\t%s\t%s\t%s\n",i,itr->fqdn,itr->clientAddress,itr->portNo);
    itr = itr->next;
    i+=1;
  }

}
