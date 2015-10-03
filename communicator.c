#include "communicator.h"

int listener;
fd_set master;
int fdmax;
int senderfd;

struct connectionInfo *startPtr=NULL;

struct connectionInfo *endPtr=NULL;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void insertClientToList(struct connectionInfo *newClient)
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


void removeClientFromList(int sockfd)
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

	return 0;
}

void* socketRunner(void *arg)
{
	int new_fd;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
	fd_set read_fds;

  while(1) {  // main accept() loop
		read_fds = master;
		int i=0;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}
		for(i=0;i<=fdmax;i++){
			if(FD_ISSET(i, &read_fds)) {
				printf("fd: %d\n",i);
				if(i == listener) {
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
					send(new_fd, "Hello World!!", 13, 0);
					FD_CLR(new_fd, &master);

					// Add client info to list
					struct connectionInfo *newClient = malloc(sizeof(struct connectionInfo));
					strcpy(newClient->clientAddress,s);
					newClient->sockfd = new_fd;
					newClient->next = NULL;
					insertClientToList(newClient);


				} else {
					printf("sending data\n");
					if(send(i, "Hello World!", 13, 0) == -1){
						perror("send");
					} // send
				} // not a listener
			} // fd_isset

		} // fdmax loop
	} // infinite loop
}

void closeAllConnections()
{
	for(int i=0;i<fdmax;i++)
	{
		if(i!=listener)
		{
			FD_CLR(i, &master);
		}
	}
  FD_CLR(listener, &master);
}
