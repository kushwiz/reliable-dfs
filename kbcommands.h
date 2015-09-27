#define ARLEN(x) (sizeof(x)/sizeof(x[0]))

enum Commands {HELP,CREATOR,DISPLAY,REGISTER,CONNECT,LIST,TERMINATE,QUIT,GET,PUT,SYNC};

int canExecute(int, enum Commands);
