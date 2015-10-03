#include "communicator.h"
#include <pthread.h>

static int sockfd;
static int listener;
static fd_set master;
static int fdmax;

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int setup_socket_()
{
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes=1;
    int rv;
		int listener;

		FD_ZERO(&master);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, SERVPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
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

    if (listen(sockfd, BACKLOG_CONN) == -1) {
        perror("listen");
        exit(1);
    }

		FD_SET(listener, &master);
		fdmax = listener;

	return 0;
}

void* socketRunner(void *arg)
{
	printf("Thread run");
	int new_fd;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  char s[INET6_ADDRSTRLEN];
	fd_set read_fds;

  printf("server: waiting for connections...\n");

  while(1) {  // main accept() loop
		read_fds = master;
		int i=0;

		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}
		printf("ready for loop\n");
		for(i=0;i<=fdmax;i++){
			if(FD_ISSET(i, &read_fds)) {
				printf("fd: %d\n",i);
				if(i == listener) {
					sin_size = sizeof their_addr;
					new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
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
					close(new_fd);
					FD_CLR(new_fd, &master);
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

pthread_t tid[2];

void* doSomeThing(void *arg)
{
    unsigned long i = 0;
    pthread_t id = pthread_self();

    if(pthread_equal(id,tid[0]))
    {
        printf("\n First thread processing\n");
    }
    else
    {
        printf("\n Second thread processing\n");
    }

    for(i=0; i<(0xFFFFFFFF);i++);

    return NULL;
}


int main(void)
{
  int err,i=0;
		if(setup_socket_() == 0) {
    while(i < 1)
    {
        err = pthread_create(&(tid[i]), NULL, &socketRunner, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

        i++;
    }
	while(1);
}

    return 0;
}
