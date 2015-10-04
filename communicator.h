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

void *server_socket_runner(void*);
int setup_server_socket(char*);
void close_all_server_connections();

struct connectionInfo
{
	char clientAddress[100];
	int sockfd;
	struct connectionInfo *next;
};

extern struct connectionInfo *startPtr;
extern struct connectionInfo *endPtr;
extern int isClient;

void send_data_via_socket(char*, char*, char*, int);
