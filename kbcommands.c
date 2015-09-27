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
			return 1;
		}
		i++;
	}
	return 0; 
}

