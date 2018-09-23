#include<unistd.h>

long long int PATH_SIZE = 1000; 
size_t BUF_SIZE = 131072;
char* delims=" ;\t\n";
long long int NUM_CMDS = 10;
long long int MAX_PROC = 10;

int kill_proc = 0;
int sent_to_bg = 0;