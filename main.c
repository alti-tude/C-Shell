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
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

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
        {
            if(child_pid[i]!=-1){
                int rPID = waitpid(child_pid[i], &status, WNOHANG);
                if(rPID != 0){
                    fprintf(stderr, "proc with pid %d exited normally\n", child_pid[i]);
                    child_pid[i]=-1;
                }
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
            char* infil;
            char* outfil;
            int temp_std_in = dup(0), temp_std_out = dup(1);

            strcpy(sc, com_ar[i]);
            char* out_redirect = strchr(sc, '>');
            if(out_redirect!=NULL){
                outfil = strtok(out_redirect+1, " >\t\n|");\
                
                int fd;
                if(*(out_redirect+1) == '>') fd = open(outfil, O_APPEND| O_WRONLY | O_CREAT);
                else fd = open(outfil, O_WRONLY | O_CREAT);

                dup2(fd, 1);

                com_ar[i][out_redirect-sc] = '\0';
            }

            strcpy(sc, com_ar[i]);
            char* in_redirect = strchr(sc, '<');
            if(in_redirect!=NULL){
                infil = strtok(in_redirect+1, " >\t\n|");
            
                int fd = open(infil, O_RDONLY);
                if(fd<0) perror("Error");
                dup2(fd, 0);

                com_ar[i][in_redirect-sc] = '\0';
            }

            road_runner(status, pid, child_pid, d, sc, com_ar, i);
            dup2(temp_std_in, 0);    
            dup2(temp_std_out, 1);        
        }
    }
}
