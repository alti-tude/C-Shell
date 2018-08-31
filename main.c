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
        ls_main(i, parsed);
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

void main(){
    data d = init();
    char* s = (char*)malloc(BUF_SIZE);
    char* sc = (char*)malloc(BUF_SIZE);

    while(1){
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
            int f=0; //flag is not zero for bg_proc
            for(int j=strlen(sc)-1;j>=0;j--) {   
                char a = sc[j];
                if(a == ' ' || a == '\t' || a== '\n') continue;
                if(a == '&') {f=j; break;} 
                else break;
            }
            int pid; //b(pid) is not 0 in parent
            pid = fork();
            
            if(f!=0){
                if(pid==0) {
                    execute_this(pid,d,sc);
                    return;
                }
            }
            else{
                if(pid==0) {
                    execute_this(pid,d,sc);
                    return;
                }
                else wait(&pid);
            }
        }
    }
}
