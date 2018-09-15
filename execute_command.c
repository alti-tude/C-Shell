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

void execute_this(int pid,data d, char * s){
    char* sc = (char*)malloc(BUF_SIZE);
    strcpy(sc, s);
    
    char* tok = strtok(s, delims);
    if(tok == NULL) return;
    // printf("%s\n", tok);
    if(strcmp(tok, "echo")==0) echo(sc);
    else if(strcmp(tok, "pwd")==0) pwd(d);
    else if(strcmp(tok, "remindme")==0) remindme(d, sc);
    else if(strcmp(tok, "clock")==0) realtime(sc);
    else if(strcmp(tok, "cd")==0) {
        tok = strtok(NULL, delims);
        cd(tok, d);
    }
    else if(strcmp(tok, "ls")==0) {
        char ** parsed = (char**)malloc(sizeof(char*)*10);
        int i = 0;
        while(tok!=NULL){
            parsed[i] = (char*)malloc(strlen(tok));
            strcpy(parsed[i], tok);
            tok = strtok(NULL, delims);
            i++;
        }
        ls_main(i, parsed, d);
    }
    else if(strcmp(tok, "pinfo")==0){
        int i;
        for(i=0;i<strlen(sc) && (sc[i]==' ' || sc[i]=='\t' || sc[i]=='\n');i++);
        if(strlen(tok)==strlen(sc)-i){ 
            int temp = getpid();
            sprintf(tok, "%d", temp);
        }
        else tok = strtok(NULL, delims);
        pinfo(tok, d);
    }
    else{
        char ** parsed = (char**)malloc(sizeof(char*)*10);
        int i = 0;
        while(tok!=NULL){
            parsed[i] = (char*)malloc(strlen(tok));
            strcpy(parsed[i], tok);
            tok = strtok(NULL, delims);
            i++;
        }
    fprintf(stderr, "DEBUG] child exec waiting\n");

        execvp(parsed[0], parsed);
    }     
}
