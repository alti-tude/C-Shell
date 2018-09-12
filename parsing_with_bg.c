#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include"headers/config.h"
#include"headers/utilities.h"
#include"headers/prompt.h"
#include"headers/builtins.h"
#include"headers/ls.h"
#include<limits.h>
#include<sys/wait.h>


void road_runner(int status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i){
    strcpy(sc, com_ar[i]);
    char* temp = (char*)malloc(BUF_SIZE);
    strcpy(temp, sc);    
    char* tok = strtok(temp, delims);
    

    //loop to check if bg//////////////////////////////

    int f=0; //flag is not zero for bg_proc
    for(int j=strlen(sc)-1;j>=0;j--) {   
        char a = sc[j];
        if(a == ' ' || a == '\t' || a== '\n') continue;
        if(a == '&') {f=j; break;} 
        else break;
    }

    if(strcmp(tok, "remindme") == 0) f=strlen(sc);
    if(strcmp(tok, "cd")==0 || strcmp(tok, "pinfo")==0 || strcmp(tok, "clock")==0){
        execute_this(pid, d, sc);
        return;
    }
    if(strcmp(tok,"exit")==0){
        // return;
        _exit(0);
    } 
    
    //fork////////////////////////////////////////////
    pid = fork();
    if(f!=0) sc[f]=' ';
    i=0;
    for(;i<MAX_PROC && child_pid[i]!=-1;i++);
    if(i==MAX_PROC) {
        printf("proc limit exceeded\n");
        _exit(0);
    }
    child_pid[i]=pid;

    //bg proc
    if(f!=0){
        if(pid==0) { 
            // printf("exec from child");
            execute_this(pid,d,sc);
            _exit(0);
        }
    }
    else{ //fg proc
        if(pid==0) { //child
            execute_this(pid,d,sc);
            _exit(0);
        }
        else { //parent
            while(wait(&status)!=pid);
            for(i=0;i<MAX_PROC && child_pid[i]!=pid;i++);
            child_pid[i]=-1;
        }
    }


}