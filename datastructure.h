struct connectionInfo
{
	char clientAddress[100];
	char portNo[10];
	int sockfd;
	char fqdn[100];
	struct connectionInfo *next;
};

extern struct connectionInfo *serverliststartPtr;
extern struct connectionInfo *serverlistendPtr;

void insertClientToServerList(struct connectionInfo*);
void removeClientFromServerList(int);
struct connectionInfo* getClientFromServerList(int);
void removeClientFromServerListWithIpPort(char*,char*);
