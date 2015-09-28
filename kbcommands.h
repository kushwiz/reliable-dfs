#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>

#define ARLEN(x) (sizeof(x)/sizeof(x[0]))

#define CMDCOUNT 11

enum Commands {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

int canExecute(int, enum Commands);

int compileAllRegex();

int whichCommands(char*);

void prepareCommandPatterns();

extern char commandPatterns[CMDCOUNT][50];

void executeCommand();
