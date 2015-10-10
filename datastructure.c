#include "datastructure.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct connectionInfo *serverliststartPtr=NULL;

struct connectionInfo *serverlistendPtr=NULL;

struct connectionInfo *peerliststartPtr=NULL;

struct connectionInfo *peerlistendPtr=NULL;



void insertClientToServerList(struct connectionInfo *newClient)
{
  if(serverliststartPtr==NULL)
  {
    serverliststartPtr = newClient;
    serverlistendPtr = newClient;
    serverlistendPtr->next=NULL;
  }
  else
  {
    serverlistendPtr->next = newClient;
    serverlistendPtr = newClient;
    serverlistendPtr->next = NULL;
  }
}


void removeClientFromServerList(int sockfd)
{
  struct connectionInfo *itr;
  itr = serverliststartPtr;
  struct connectionInfo *itrPrev;
  itrPrev = itr;

  while(itr!=NULL)
  {
    if(itr->sockfd == sockfd)
    {
      if(serverliststartPtr == itr)
      {
	serverliststartPtr = itr->next;
	itrPrev = serverliststartPtr;
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


struct connectionInfo* getClientFromServerList(int sockfd)
{
  struct connectionInfo *itr;
  itr = serverliststartPtr;
  while(itr!=NULL)
  {
    if(itr->sockfd==sockfd)
    {
      return itr;
    }
    else
      itr = itr->next;
  }
  return NULL;
}

void removeClientFromServerListWithIpPort(char *ip, char *portNo)
{
  struct connectionInfo *itr;
  struct connectionInfo *itrPrev;

  itr = serverliststartPtr;
  itrPrev = itr;

  while(itr!=NULL)
  {
    if(strcmp(ip, itr->clientAddress)==0 && strcmp(portNo,itr->portNo)==0)
    {
      itrPrev->next = itr->next;
      free(itr);
      itr = itrPrev->next;
    }
    else
    {
      itrPrev = itr;
      itr = itr->next;
    }
  }
}


void insertClientToPeerList(struct connectionInfo *newClient)
{
  if(peerliststartPtr==NULL)
  {
    peerliststartPtr = newClient;
    peerlistendPtr = newClient;
    peerlistendPtr->next=NULL;
  }
  else
  {
    peerlistendPtr->next = newClient;
    peerlistendPtr = newClient;
    peerlistendPtr->next = NULL;
  }
}


void removeClientFromPeerList(int sockfd)
{
  struct connectionInfo *itr;
  itr = peerliststartPtr;
  struct connectionInfo *itrPrev;
  itrPrev = itr;

  while(itr!=NULL)
  {
    if(itr->sockfd == sockfd)
    {
      if(peerliststartPtr == itr)
      {
	peerliststartPtr = itr->next;
	itrPrev = peerliststartPtr;
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


struct connectionInfo* getClientFromPeerList(int sockfd)
{
  struct connectionInfo *itr;
  itr = peerliststartPtr;
  while(itr!=NULL)
  {
    if(itr->sockfd==sockfd)
    {
      return itr;
    }
    else
      itr = itr->next;
  }
  return NULL;
}

void removeClientFromPeerListWithIpPort(char *ip, char *portNo)
{
  struct connectionInfo *itr;
  struct connectionInfo *itrPrev;

  itr = peerliststartPtr;
  itrPrev = itr;

  while(itr!=NULL)
  {
    if(strcmp(ip, itr->clientAddress)==0 && strcmp(portNo,itr->portNo)==0)
    {
      itrPrev->next = itr->next;
      free(itr);
      itr = itrPrev->next;
    }
    else
    {
      itrPrev = itr;
      itr = itr->next;
    }
  }
}
