#include "communicator.h"
#include "kbcommands.h"
#include "datastructure.h"

#define MAX(x,y) (x>y)?x:y

int currentCommand = 0;

int listener;
fd_set master;
int fdmax;
int isClient = 0;

fd_set slave;
int slave_fdmax;

char *port;
char shostname[500];
char sipaddr[100];

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

struct tm* getcurrenttime()
{
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return timeinfo;
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

	if(isClient == 0)
	{
		printf("isCLient:%d\n",isClient);
		gethostname(&shostname[0], sizeof(shostname));
		struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
		hostname_to_ip(&shostname[0], &sipaddr[0]);
		strcpy(newClient->clientAddress, sipaddr);
		strcpy(newClient->fqdn, shostname);
		strcpy(newClient->portNo, portNo);
		newClient->sockfd = listener;
		newClient->next = NULL;
		insertClientToServerList(newClient);
		printf("fqdn:%s\n",newClient->fqdn);
		printf("fqdn:%s\n",serverliststartPtr->fqdn);
	}

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
	unsigned char buf[BUFFSIZE];    // buffer for client data
	char userInput[500];
	int nbytes;
	fd_set slave_read_fds;
	struct timeval tv = {0, 200000};	// 200ms timeout
	FD_ZERO(&slave_read_fds);
	FD_SET(0,&master);
	int k=0;
	while(currentCommand!=QUIT) {  // main accept() loop
		read_fds = master;
		slave_read_fds = slave;
		int i=0;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select1");
			exit(4);
		}
		//		if(select(slave_fdmax+1, &slave_read_fds, NULL, NULL, &tv) == -1)
		//		{
		//			perror("select2");
		//			exit(4);
		//		}
		int max = MAX(fdmax, slave_fdmax);
		for(i=0;i<=fdmax;i++){
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
						if(fdmax<new_fd){
							fdmax = new_fd;
						}
					}
					inet_ntop(their_addr.ss_family,
							get_in_addr((struct sockaddr *)&their_addr),
							s, sizeof s);
					printf("server: got connection from %s  sfd:%d\n", s, new_fd);

					// Add client info to list
					//struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
					//strcpy(newClient->clientAddress, s);
					//newClient->sockfd = new_fd;
					//newClient->next = NULL;
					//insertClientToServerList(newClient);

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
						printf("closing\n");
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
						if(isClient == 0)
						{
							struct connectionInfo *leavingClient = getClientFromServerList(i);
							unsigned char innerbuf[BUFFSIZE];
							int innerpacketsize = 0;
							innerpacketsize += pack(innerbuf+innerpacketsize, "h", REMOVE_FROM_SERVER_IP_LIST);
							innerpacketsize += pack(innerbuf+innerpacketsize, "s", leavingClient->clientAddress);
							innerpacketsize += pack(innerbuf+innerpacketsize, "s", leavingClient->portNo);
							for(k=0;k<=fdmax;k++)
							{
								if(k!=listener && k>listener){
									send(k, &innerbuf, innerpacketsize,0);
								}
							}
							removeClientFromServerList(i);
						}
						else
						{
							removeClientFromPeerList(i);
						}
					} else {
						int cmdl;
						unpack(buf, "h", &cmdl);
						printf("recd some data cmd:%d\n",cmdl);
						process_socket_actions(cmdl, buf, i);
					} // send
				} // not a listener

			} // fd_isset
			//	else if(FD_ISSET(i, &slave_read_fds)) {
			//		printf("clientrun_fd: %d\n",i);
			//		// handle data from a client
			//		if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
			//			// got error or connection closed by client
			//			if (nbytes == 0) {
			//				// connection closed
			//				printf("recvclient: socket %d hung up\n", i);
			//			} else {
			//				perror("recv");
			//			}
			//			close(i); // bye!
			//			//removeClientFromList(i);
			//			FD_CLR(i, &slave); // remove from master set
			//		} else {
			//			int cmdl;
			//			unpack(buf, "h", &cmdl);
			//			process_socket_actions(cmdl, buf, i);
			//		} // send

			//	}

		} // fdmax loop
	} // infinite loop
}

void process_socket_actions(int cmdl, unsigned char *buf, int sfd)
{
	int commandTemp, i=0, packetsize = 0, numbytes, numpackets, filelen, filebytesread, packetseq;
	char *saddress, sport[50], *fqdn = NULL, *dynamicunpacker=NULL;
	unsigned char buff[BUFFSIZE], buff1[BUFFSIZE];
	char filebuffer[FILEBUFFSIZE];
	char caddress[100], pno[10], cfqdn[100], strgetfilename[100];
	fd_set read_fds;
	FILE *fp;
	switch(cmdl)
	{
		case REGISTER:
			read_fds = master;
			saddress = getipbyfd(sfd);
			printf("new client registering:%s\n", saddress);
			struct connectionInfo *itr = serverliststartPtr;
			while(itr!=NULL) {
				memset(buff,0,BUFFSIZE);
				packetsize=0;
				packetsize += pack(buff+packetsize, "h", ADD_TO_SERVER_IP_LIST);
				packetsize += pack(buff+packetsize, "s", itr->clientAddress);
				packetsize += pack(buff+packetsize, "s", itr->portNo);
				packetsize += pack(buff+packetsize, "s", itr->fqdn);
				if((numbytes=send(sfd, &buff, packetsize, 0)) == -1)
				{
					perror("send");
					exit(1);
				}
				else
				{
					printf("bytes sent:%d packetsize:%d to:%d\n",numbytes,packetsize,sfd);
				}
				itr = itr->next;
				usleep(1000);
			}
			unpack(buf, "h100s", &commandTemp, sport);
			fqdn = malloc(100*sizeof(char));
			strcpy(fqdn, getfqdnbyip(saddress, &sport[0]));
			struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
			strcpy(newClient->clientAddress, saddress);
			newClient->sockfd = sfd;
			strcpy(newClient->portNo, sport);
			strcpy(newClient->fqdn, fqdn);
			newClient->next = NULL;
			printf("fqdn:%s\n",newClient->fqdn);
			insertClientToServerList(newClient);
			free(fqdn);
			packetsize = 0;
			packetsize += pack(buff1+packetsize, "h", ADD_TO_SERVER_IP_LIST);
			packetsize += pack(buff1+packetsize, "s", newClient->clientAddress);
			packetsize += pack(buff1+packetsize, "s", newClient->portNo);
			packetsize += pack(buff1+packetsize, "s", newClient->fqdn);
			for(i=0;i<=fdmax;i++){
				if(i!=listener && i>listener) {
					printf("sending to %d\n",i);
					if ((numbytes = send(i, &buff1, packetsize, 0)) == -1) {
						perror("send");
						exit(1);
					}
					else {printf("bytes sent: %d  packetsize:%d to:%d\n",numbytes,packetsize,i);}
				}
			}
			break;

		case CONNECT:
			printf("peer is connecting\n");
			read_fds = master;
			saddress = getipbyfd(sfd);

			unpack(buf, "h100s", &commandTemp, sport);
			fqdn = malloc(100*sizeof(char));
			strcpy(fqdn, getfqdnbyip(saddress, &sport[0]));

			struct connectionInfo *newPeer = malloc(sizeof(struct connectionInfo));
			strcpy(newPeer->clientAddress,saddress);
			strcpy(newPeer->portNo, sport);
			strcpy(newPeer->fqdn, fqdn);
			newPeer->sockfd = sfd;
			newPeer->next = NULL;
			insertClientToPeerList(newPeer);
			free(fqdn);
			break;

		case GET:
			printf("GET requested\n");
			unpack(buf, "h100s", &commandTemp, strgetfilename);
			printf("filename to send for GET:%s\n", strgetfilename);
			sendfileget(strgetfilename, sfd);
			break;

		case GET_REPLY:
			printf("GET REPLY\n");
			memset(filebuffer, 0, FILEBUFFSIZE);
			dynamicunpacker = malloc(50*sizeof(char));
			sprintf(dynamicunpacker, "h100s%dsh", FILEBUFFSIZE);
			unpack(buf, dynamicunpacker, &commandTemp, strgetfilename, filebuffer, &filebytesread, &packetseq);
			free(dynamicunpacker);
			if(packetseq==1 && (access(strgetfilename,F_OK)!=-1))
			{
				fp = fopen(strgetfilename, "rb");
			}
			else
			{ 
				fp = fopen(strgetfilename, "ab");
			}
			printf("size of filebuffer:%lu\n",sizeof(filebuffer));
			fwrite(filebuffer, 1, filebytesread, fp);
			fclose(fp);
			break;

		case PUT_SATNEY:
			printf("PUT REQUEST\n");
			memset(filebuffer, 0, FILEBUFFSIZE);
			dynamicunpacker = malloc(50*sizeof(char));
			sprintf(dynamicunpacker, "h100s%dsh", FILEBUFFSIZE);
			unpack(buf, dynamicunpacker, &commandTemp, strgetfilename, filebuffer, &filebytesread, &packetseq);
			if(packetseq==1 && (access(strgetfilename,F_OK)!=-1))
			{
				fp = fopen(strgetfilename, "rb");
			}
			else
			{ 
				fp = fopen(strgetfilename, "ab");
			}
			fwrite(filebuffer, 1, filebytesread, fp);
			fclose(fp);	
			break;

		case ADD_TO_SERVER_IP_LIST:
			printf("add to server ip list\n");
			unpack(buf, "h100s10s100s", &commandTemp, caddress,pno,cfqdn);
			printf("caddress:%s, pno:%s, cfqdn:%s\n",caddress,pno,cfqdn);
			struct connectionInfo *newClient1 = malloc(sizeof(struct connectionInfo));
			strcpy(newClient1->clientAddress, caddress);
			strcpy(newClient1->portNo, pno);
			strcpy(newClient1->fqdn, cfqdn);
			newClient1->next = NULL;
			insertClientToServerList(newClient1);
			break;

		case REMOVE_FROM_SERVER_IP_LIST:
			printf("remove from server ip list\n");
			unpack(buf, "h100s10s", &commandTemp, caddress,pno);
			printf("caddress:%s, pno:%s\n",caddress,pno);
			removeClientFromServerListWithIpPort(caddress, pno);
			break;
	}
}


void send_data_via_socket(char *serverAddress, char *portNo, unsigned char *data, int data_size, int *sfd)
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

	FD_SET(senderfd, &master);
	fdmax = senderfd;
	*sfd = senderfd;
	printf("slave_fdmax:%d\n",fdmax);
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
	unsigned char buf[BUFFSIZE];
	int commandTemp, i=0, packetsize = 0, numbytes, numpackets, filelen, filebytesread;
	regmatch_t rm[3];
	struct connectionInfo *returnedPeer;
	char *strclientId = malloc(10*sizeof(char));
	char *strgetfilename = malloc(100*sizeof(char));
	char filebuffer[FILEBUFFSIZE];
	struct connectionInfo *foundClient = NULL; 
	FILE *fp;

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
					printf("IP:%s Port:%s\n", sipaddr, port);
					return;
				case REGISTER:
					if(is_registered == 0) {
						packetsize = 0;
						packetsize += pack(buf+packetsize, "h", currentCommand);
						packetsize += pack(buf+packetsize, "s", port);
						char *serverAddress = malloc(100*sizeof(char));
						//strncpy(serverAddress, userInput + rm[1].rm_so, (int)(rm[1].rm_eo - rm[1].rm_so));
						printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
						sprintf(serverAddress,"%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
						printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
						char *serverPort = malloc(50*sizeof(char));
						sprintf(serverPort, "%.*s", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
						//strncpy(serverPort, userInput + rm[2].rm_so, (int)(rm[2].rm_eo - rm[2].rm_so));
						printf("Server address:%s\n", serverAddress);
						printf("Server port: %s\n", serverPort);
						struct connectionInfo *conObj = malloc(sizeof(struct connectionInfo));
						send_data_via_socket(serverAddress, serverPort, buf, packetsize, &conObj->sockfd);
						strcpy(conObj->clientAddress, serverAddress);
						strcpy(conObj->portNo, serverPort);
						strcpy(conObj->fqdn, getfqdnbyip(serverAddress, serverPort));
						insertClientToPeerList(conObj);
						is_registered = 1;}
					else {
						printf("Already Registered\n");
					}
					return;
					break;
				case CONNECT:
					memset(buf,0,BUFFSIZE);
					packetsize = 0;
					packetsize += pack(buf+packetsize, "h", currentCommand);
					packetsize += pack(buf+packetsize, "s", port);
					char *serverAddress = malloc(100*sizeof(char));
					//strncpy(serverAddress, userInput + rm[1].rm_so, (int)(rm[1].rm_eo - rm[1].rm_so));
					char *serverPort = malloc(50*sizeof(char));
					//strncpy(serverPort, userInput + rm[2].rm_so, (int)(rm[2].rm_eo - rm[2].rm_so));
					printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					sprintf(serverAddress,"%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					sprintf(serverPort, "%.*s", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);

					printf("Server address:%s\n", serverAddress);
					printf("Server port: %s\n", serverPort);
					foundClient = getClientFromPeerListWithIpPort(serverAddress, serverPort);
					if(foundClient == NULL)
					{
						printf("Hold on, this client is not registered with the server\n");
					}
					else
					{
						struct connectionInfo *newPeer = malloc(sizeof(struct connectionInfo));
						strcpy(newPeer->clientAddress, foundClient->clientAddress);
						strcpy(newPeer->fqdn, foundClient->fqdn);
						strcpy(newPeer->portNo, foundClient->portNo);
						send_data_via_socket(serverAddress, serverPort, buf, packetsize, &newPeer->sockfd);
						insertClientToPeerList(newPeer);
					}
					return;
					break;
				case LIST:
					doList();
					return;
					break;
				case TERMINATE:
					returnedPeer = getClientFromPeerListWithId(2);
					if(returnedPeer == NULL)
					{
						printf("Invalid connection id\n");
					}
					else
					{
						close(returnedPeer->sockfd);
						FD_CLR(returnedPeer->sockfd, &master);
						removeClientFromPeerList(returnedPeer->sockfd);
					}
					return;
					break;
				case QUIT:
					close_all_server_connections();
					return;
					break;
				case GET:
					printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					sprintf(strclientId,"%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					sprintf(strgetfilename, "%.*s", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					memset(buf,0,BUFFSIZE);
					packetsize = 0;
					packetsize += pack(buf+packetsize, "h", currentCommand);
					packetsize += pack(buf+packetsize, "s", strgetfilename);
					int idofclient = atoi(strclientId);
					foundClient = getClientFromPeerListWithId(idofclient);
					if(foundClient == NULL)
					{
						printf("Hold on, this client is not registered with the server\n");
					}
					else
					{
						if((numbytes=send(foundClient->sockfd, &buf, packetsize, 0)) == -1)
						{
							perror("send");
							exit(1);
						}
						else
						{
							printf("GET: bytes sent:%d packetsize:%d to:%d\n",numbytes,packetsize,foundClient->sockfd);
						}
					}	
					printf("client id: %d\n", idofclient);
					return;
					break;
				case PUT:
					printf("Text: <<%.*s>>\n", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					sprintf(strclientId,"%.*s", (int)(rm[1].rm_eo - rm[1].rm_so), userInput + rm[1].rm_so);
					printf("Text: <<%.*s>>\n", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					sprintf(strgetfilename, "%.*s", (int)(rm[2].rm_eo - rm[2].rm_so), userInput + rm[2].rm_so);
					memset(buf,0,BUFFSIZE);
					packetsize = 0;
					packetsize += pack(buf+packetsize, "h", currentCommand);
					packetsize += pack(buf+packetsize, "s", strgetfilename);
					foundClient = getClientFromPeerListWithId(atoi(strclientId));
					if(foundClient == NULL)
					{
						printf("Hold on, this client is not registered with the server\n");
					}
					else
					{
						printf("filename to send for PUT%s\n", strgetfilename);
						fp = fopen(strgetfilename,"rb");
						fseek(fp, 0, SEEK_END);
						filelen = ftell(fp);
						rewind(fp);
						printf("filelen:%d\n",filelen);
						numpackets = (filelen + FILEBUFFSIZE - 1) / FILEBUFFSIZE;
						printf("num packets:%d\n",numpackets);
						for(i=0;i<numpackets;i++)
						{
							memset(filebuffer,0,FILEBUFFSIZE);
							memset(buf, 0, BUFFSIZE);
							filebytesread = fread(&filebuffer, 1, FILEBUFFSIZE, fp);
							packetsize=0;
							packetsize+= pack(buf+packetsize, "h", PUT_SATNEY);
							packetsize+= pack(buf+packetsize, "s",strgetfilename);
							packetsize+= pack(buf+packetsize, "s",filebuffer);
							packetsize+= pack(buf+packetsize, "h",filebytesread);
							packetsize+= pack(buf+packetsize, "h",i+1);
							if((numbytes=send(foundClient->sockfd, &buf, packetsize, 0)) == -1)
							{
								perror("send");
								exit(1);
							}
							else
							{
								printf("PUT: bytes sent:%d packetsize:%d to:%d\n",numbytes,packetsize,foundClient->sockfd);
							}

						}
						fclose(fp);
					}	
					printf("client id: %d\n", idofclient);

					return;
					break;
				case SYNC:
					printf("sync\n");
					return;
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

	err = getpeername(fd, (struct sockaddr*) &name, &namelen);
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

int hostname_to_ip(char *hostname , char *ip)
{
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy(ip , inet_ntoa( h->sin_addr ) );
	}

	freeaddrinfo(servinfo); // all done with this structure
	return 0;
}

void sendfileget(char *strgetfilename, int sfd)
{
	FILE *fp;
	int numbytes, packetsize, filelen, filebytesread, numpackets, i;
	char filebuffer[FILEBUFFSIZE]; 
	unsigned char buff[BUFFSIZE];

	fp = fopen(strgetfilename,"rb");
	fseek(fp, 0, SEEK_END);
	filelen = ftell(fp);
	rewind(fp);
	printf("filelen:%d\n",filelen);
	numpackets = (filelen + FILEBUFFSIZE - 1) / FILEBUFFSIZE;
	printf("num packets:%d\n",numpackets);
	for(i=0;i<numpackets;i++)
	{
		memset(filebuffer,0,FILEBUFFSIZE);
		memset(buff, 0, BUFFSIZE);
		filebytesread = fread(&filebuffer, 1, FILEBUFFSIZE, fp);
		packetsize=0;
		packetsize+= pack(buff+packetsize, "h", GET_REPLY);
		packetsize+= pack(buff+packetsize, "s",strgetfilename);
		packetsize+= pack(buff+packetsize, "s",filebuffer);
		packetsize+= pack(buff+packetsize, "h",filebytesread);
		packetsize+= pack(buff+packetsize, "h",i+1);
		if((numbytes=send(sfd, &buff, packetsize, 0)) == -1)
		{
			perror("send");
			exit(1);
		}
		else
		{
			printf("GET: bytes sent:%d packetsize:%d to:%d\n",numbytes,packetsize,sfd);
		}

	}
	fclose(fp);
}
