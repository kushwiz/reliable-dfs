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
#define SERVPORT "27000"

void *socketRunner(void*);
int setup_socket_();
void closeAllConnections();

