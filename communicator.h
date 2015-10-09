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
#include "datapacker.h"
#define BACKLOG_CONN 10

void server_socket_runner();
int setup_server_socket(char*);
void close_all_server_connections();

extern int isClient;

void send_data_via_socket(char*, char*, unsigned char*, int);
void process_socket_actions(int, unsigned char*, int);

char* getipbyfd(int);
char* getfqdnbyip(char*,char*);
int hostname_to_ip(char*, char*);
