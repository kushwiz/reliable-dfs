#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>

#define ARLEN(x) (sizeof(x)/sizeof(x[0]))

#define CMDCOUNT 11

enum Commands {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

enum ConnectionSignals {ADD_TO_SERVER_IP_LIST=11, REMOVE_FROM_SERVER_IP_LIST, GET_REPLY, PUT_SATNEY, SYNC_PEERS};

int canExecute(int, enum Commands);

int compileAllRegex();

int whichCommands(char*);

void prepareCommandPatterns();

extern char commandPatterns[CMDCOUNT][500];

void executeCommand();

void doCreator();

void doList();
