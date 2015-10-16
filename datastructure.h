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

extern struct connectionInfo *peerliststartPtr;
extern struct connectionInfo *peerlistendPtr;

void insertClientToServerList(struct connectionInfo*);
void removeClientFromServerList(int);
struct connectionInfo* getClientFromServerList(int);
void removeClientFromServerListWithIpPort(char*,char*);
struct connectionInfo* getClientFromServerListWithIp(char*);

void insertClientToPeerList(struct connectionInfo*);
void removeClientFromPeerList(int);
struct connectionInfo* getClientFromPeerList(int);
void removeClientFromPeerListWithIpPort(char*,char*);
struct connectionInfo* getClientFromPeerListWithIp(char*);
struct connectionInfo* getClientFromPeerListWithId(int);
int getPeerListSize();
