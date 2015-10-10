struct connectionInfo
{
	char clientAddress[100];
	char portNo[10];
	int sockfd;
	char fqdn[100];
	struct connectionInfo *next;
};

extern struct connectionInfo *startPtr;
extern struct connectionInfo *endPtr;

void insertClientToMasterList(struct connectionInfo*);
void removeClientFromMasterList(int);
struct connectionInfo* getClientFromMasterList(int);
void removeClientFromMasterListWithIpPort(char*,char*);
