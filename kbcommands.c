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
  itr = peerliststartPtr;
  printf("id:\tHostname\t\t\tIP address\tPort No.\tSfd\n");
  int i = 1;
  while(itr!=NULL)
  {
    printf("%d\t%s\t%s\t%s\t%d\n",i,itr->fqdn,itr->clientAddress,itr->portNo,itr->sockfd);
    itr = itr->next;
    i+=1;
  }

}

void doHelp()
{
	printf("HELP: Displays list of available commands\n");
	printf("CREATOR: Displays author information\n");
	printf("DISPLAY: Displays the IP and port of this process\n");
	printf("REGISTER <server IP> <port>: Registers this client to the server\n");
	printf("CONNECT <destination> <port>: Connects as a peer to the specified address\n");
	printf("LIST: Displays a numbered list of all connected connections on this process\n");
	printf("TERMINATE <id>: Terminates a connection from the available list of connections\n");
	printf("QUIT: Closes all connections and terminates this process\n");
	printf("GET <connection_id> <file>: Download the specified file from the host\n");
	printf("PUT <connection_id> <file>: Uploads the specified file to the host\n");
	printf("SYNC: Synchronizes host specific text files across its peers\n");
}
