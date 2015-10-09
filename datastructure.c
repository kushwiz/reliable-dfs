#include "datastructure.h"
#include<stdio.h>
#include<stdlib.h>

struct connectionInfo *startPtr=NULL;

struct connectionInfo *endPtr=NULL;

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


