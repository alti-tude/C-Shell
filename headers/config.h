#ifndef CONFIG_H
#define CONFIG_H

long long int PATH_SIZE; 
size_t BUF_SIZE;
// extern long long int USER_SIZE = 1000; 
struct data{
    char* HOME;
    char* user;
    char* host;
};
typedef struct data data;
char* delims;
long long int NUM_CMDS;
long long int MAX_PROC;

int kill_proc;
int sent_to_bg;
int quit_proc;
int *job_order;
#endif


