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
        execvp(parsed[0], parsed);
    }     
}

int pid = 1;
int status;
void main(){
    data d = init();
    char* s = (char*)malloc(BUF_SIZE);
    char* sc = (char*)malloc(BUF_SIZE);
    int* child_pid = (int*)malloc(sizeof(int)*MAX_PROC);
    for(int i=0;i<MAX_PROC;i++) child_pid[i]=-1;

    while(1){
        //check if child proc quit
        for(int i=0;i<MAX_PROC;i++)
            if(child_pid[i]!=-1){
                int rPID = waitpid(child_pid[i], &status, WNOHANG);
                if(rPID != 0){
                    fprintf(stderr, "proc with pid %d exited normally\n", child_pid[i]);
                    child_pid[i]=-1;
                }
            }
        display_prompt(d);
        fgets(s, BUF_SIZE, stdin);
        
        //break input on ;
        char** com_ar = (char**) malloc(sizeof(char*)*NUM_CMDS);
        int c = 0;
        char* cmd_str = strtok(s, ";\n");
        while(cmd_str!=NULL){
            com_ar[c] = (char*)malloc(strlen(cmd_str));
            strcpy(com_ar[c], cmd_str);
            cmd_str = strtok(NULL, ";\n");
            c++;
        }
        
        // run cmds
        for(int i=0;i<c;i++){
            strcpy(sc, com_ar[i]);
            char* temp = (char*)malloc(BUF_SIZE);
            strcpy(temp, sc);    
            char* tok = strtok(temp, delims);

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
                continue;
            }
            if(strcmp(tok,"exit")==0){
                _exit(0);
            } 
            
            //fork
            pid = fork();
            if(f!=0) sc[f]=' ';
            int i=0;
            for(;i<MAX_PROC && child_pid[i]!=-1;i++);
            if(i==MAX_PROC) {
                printf("proc limit exceeded\n");
                continue;
            }
            child_pid[i]=pid;

            //bg proc
            if(f!=0){
                if(pid==0) { 
                    // printf("exec from child");
                    execute_this(pid,d,sc);
                    return;
                }
            }
            else{ //fg proc
                if(pid==0) { //child
                    execute_this(pid,d,sc);
                    return;
                }
                else { //parent
                    while(wait(&status)!=pid);
                    for(i=0;i<MAX_PROC && child_pid[i]!=pid;i++);
                    child_pid[i]=-1;
                }
            }
        }
    }
}
