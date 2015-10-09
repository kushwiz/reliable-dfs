struct connectionInfo
{
	char clientAddress[100];
	char portNo[50];
	int sockfd;
	char fqdn[500];
	struct connectionInfo *next;
};

extern struct connectionInfo *startPtr;
extern struct connectionInfo *endPtr;

void insertClientToMasterList(struct connectionInfo*);
void removeClientFromMasterList(int);
