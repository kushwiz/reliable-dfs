#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>

#define BACKLOG_CONN 10

void *socketRunner(void*);
int setup_server_socket(char*);
void closeAllConnections();

struct connectionInfo
{
	char clientAddress[100];
	int sockfd;
	struct connectionInfo *next;
};

extern struct connectionInfo *startPtr;
extern struct connectionInfo *endPtr;
