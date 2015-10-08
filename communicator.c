#include "communicator.h"
#include "kbcommands.h"

#define MAX(x,y) (x>y)?x:y

int currentCommand = 0;

int listener;
fd_set master;
int fdmax;
int isClient = 0;

struct connectionInfo *startPtr=NULL;

struct connectionInfo *endPtr=NULL;

fd_set slave;
int slave_fdmax;

char *port;

//State constants
int is_registered = 0;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void insertClientToMasterList(struct connectionInfo *newClient)
{
	if(startPtr==NULL)
	{
		startPtr = newClient;
		endPtr = newClient;
		endPtr->next=NULL;
	}
	else
	{
		endPtr->next = newClient;
		endPtr = newClient;
		endPtr->next = NULL;
	}
}


void removeClientFromMasterList(int sockfd)
{
	struct connectionInfo *itr;
	itr = startPtr;
	struct connectionInfo *itrPrev;
	itrPrev = itr;

	while(itr!=NULL)
	{
		if(itr->sockfd == sockfd)
		{
			if(startPtr == itr)
			{
				startPtr = itr->next;
				itrPrev = startPtr;
				free(itr);
			}
			else
			{
				itrPrev->next = itr->next;
				free(itr);
				itr = itrPrev->next;
			}
			return;
		}
		else
		{	itrPrev = itr;
			itr=itr->next;
		}
	}
}

int setup_server_socket(char *portNo)
{
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	int rv;

	port = portNo;

	FD_ZERO(&master);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, portNo, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((listener = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(listener, p->ai_addr, p->ai_addrlen) == -1) {
			close(listener);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(listener, BACKLOG_CONN) == -1) {
		printf("listen error\n");
		perror("listen");
		exit(1);
	}

	FD_SET(listener, &master);
	fdmax = listener;
	server_socket_runner();
	return 0;
}

void server_socket_runner()
{
	int new_fd;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	fd_set read_fds;
	unsigned char buf[256];    // buffer for client data
	char userInput[500];
	int nbytes;
	fd_set slave_read_fds;
	struct timeval tv = {0, 200000};	// 200ms timeout
	FD_ZERO(&slave_read_fds);
	FD_SET(0,&master);
	while(currentCommand!=QUIT) {  // main accept() loop
		read_fds = master;
		slave_read_fds = slave;
		int i=0;
		if(select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1)
		{
			perror("select1");
			exit(4);
		}
		if(select(slave_fdmax+1, &slave_read_fds, NULL, NULL, &tv) == -1)
		{
			perror("select2");
			exit(4);
		}
		int max = MAX(fdmax, slave_fdmax);
		for(i=0;i<=max;i++){
			if(FD_ISSET(i, &read_fds)) {
				//	printf("fd:%d\n",i);
				if( i == 0 )
				{
					fgets(userInput, sizeof(userInput), stdin);
					executeCommand(userInput);
				}
				else if(i == listener) {
					sin_size = sizeof their_addr;
					new_fd = accept(listener, (struct sockaddr *)&their_addr, &sin_size);
					if (new_fd == -1) {
						perror("accept");
						continue;
					} else {
						FD_SET(new_fd, &master);
						fdmax = new_fd;
					}
					inet_ntop(their_addr.ss_family,
							get_in_addr((struct sockaddr *)&their_addr),
							s, sizeof s);
					printf("server: got connection from %s\n", s);

					// Add client info to list
					//struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
					//strcpy(newClient->clientAddress, s);
					//newClient->sockfd = new_fd;
					//newClient->next = NULL;
					//insertClientToMasterList(newClient);

				} else {
					// handle data from a client
					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("selectserver: socket %d hung up\n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						removeClientFromMasterList(i);
						FD_CLR(i, &master); // remove from master set
					} else {
						int cmdl;
						unpack(buf, "h", &cmdl);
						process_socket_actions(cmdl, buf, i);
					} // send
				} // not a listener

			} // fd_isset
			else if(FD_ISSET(i, &slave_read_fds)) {
				printf("clientrun_fd: %d\n",i);
				// handle data from a client
				if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
					// got error or connection closed by client
					if (nbytes == 0) {
						// connection closed
						printf("recvclient: socket %d hung up\n", i);
					} else {
						perror("recv");
					}
					close(i); // bye!
					//removeClientFromList(i);
					FD_CLR(i, &slave); // remove from master set
				} else {
					printf("recd data:%s\n", buf);
				} // send

			}

		} // fdmax loop
	} // infinite loop
}

void process_socket_actions(int cmdl, unsigned char *buf, int sfd)
{

			int commandTemp;
			char *saddress, sport[50];
	switch(cmdl)
	{
		case REGISTER:
			saddress = getipbyfd(sfd);
			printf("new client registering:%s\n", saddress);
			unpack(buf, "h100s", &commandTemp, sport);
			char *fqdn = getfqdnbyip(saddress, &sport[0]);
			struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
			strcpy(newClient->clientAddress, saddress);
			newClient->sockfd = sfd;
			strcpy(newClient->portNo, sport);
			strcpy(newClient->fqdn, fqdn);
			newClient->next = NULL;
			printf("fqdn:%s\n",newClient->fqdn);
			insertClientToMasterList(newClient);
		break;
	}
}


void send_data_via_socket(char *serverAddress, char *portNo, unsigned char *data, int data_size)
{
	int  senderfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(serverAddress, portNo, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((senderfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(senderfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(senderfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo);  //all done with this structure

	FD_SET(senderfd, &slave);
	slave_fdmax = senderfd;
	printf("slave_fdmax:%d\n",slave_fdmax);
	if ((numbytes = send(senderfd, data, data_size, 0)) == -1) {
		perror("send");
		exit(1);
	}

}

void close_all_server_connections()
{
	int i;
	for(i=0;i<fdmax;i++)
	{
		FD_CLR(i, &master);
	}
}


void executeCommand(char *userInput)
{
	unsigned char buf[256];
	int packetsize;
	int i;
	regmatch_t rm[3];
	for(i=0; i<CMDCOUNT; i++)
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
					printf("IP:%s Port:%s\n", getipbyfd(fdmax), port);
					return;
				case REGISTER:
					if(is_registered == 0) {
					packetsize = pack(buf, "hs", currentCommand, port);
					packi16(buf+2, packetsize);
					char *serverAddress = malloc(100*sizeof(char));
					strncpy(serverAddress, userInput + rm[1].rm_so, (int)(rm[1].rm_eo - rm[1].rm_so));
					char *serverPort = malloc(50*sizeof(char));
					strncpy(serverPort, userInput + rm[2].rm_so, (int)(rm[2].rm_eo - rm[2].rm_so));
//								printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
	//							printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					printf("Server address:%s\n", serverAddress);
					printf("Server port: %s\n", serverPort);
					send_data_via_socket(serverAddress, serverPort, buf, packetsize);
					is_registered = 1;}
					else {
					  printf("Already Registered\n");
					}
					return;
					break;
				case CONNECT:
					break;
				case LIST:
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

char* getipbyfd(int fd)
{
	int err;
	char *buffer;
	buffer = malloc(100*sizeof(char));

	struct sockaddr_in name;
	socklen_t namelen = sizeof(name);

	err = getsockname(fd, (struct sockaddr*) &name, &namelen);
	const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);

	if(p != NULL)
	{
		return buffer;
	}
	else
	{
		return NULL;
	}
}

char* getfqdnbyip(char *ipaddr, char *servport)
{
struct sockaddr_in sa;
sa.sin_family = AF_INET;
inet_pton(AF_INET, ipaddr, &sa.sin_addr);
  char node[NI_MAXHOST];
  int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), node, sizeof(node), NULL, 0, 0);
  if (res)
  {
    printf("fqdn:%s\n", gai_strerror(res));
    exit(1);
  }
  return &node[0];
}
