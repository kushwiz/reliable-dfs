#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<regex.h>
#include<errno.h>

#define ARLEN(x) (sizeof(x)/sizeof(x[0]))

#define REGISTER_CMD_REGEX "^REGISTER (.+) ([0-9]+)$"
#define CREATOR_CMD_REGEX "^CREATOR$"
#define DISPLAY_CMD_REGEX "^DISPLAY$"
#define CONNECT_CMD_REGEX "^CONNECT (.+) ([0-9]+)$"
#define LIST_CMD_REGEX "^LIST$"
#define TERMINATE_CMD_REGEX "^TERMINATE ([0-9]+)$"
#define QUIT_CMD_REGEX "^QUIT$"
#define GET_CMD_REGEX "^GET ([0-9]+) (.+)$"
#define PUT_CMD_REGEX "^PUT ([0-9]+) (.+)$"
#define SYNC_CMD_REGEX "^SYNC$"

enum Commands {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

int canExecute(int, enum Commands);

int prepareRegex();

int whichCommands(char*);
