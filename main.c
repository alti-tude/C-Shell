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

void update_jobs(int* creationTime){
    int c = 0;
    job_order = (int*)malloc(sizeof(int)*MAX_PROC);
    for(int i=0;i<MAX_PROC;i++) job_order[i]=-1;
    for(int i=0;i<MAX_PROC;i++){
        long long int mi=LLONG_MAX, mid=-1;

        for(int j=0;j<MAX_PROC; j++){
            if(creationTime[j]!=-1){
                if(mi>creationTime[j]){
                    mi = creationTime[j];
                    mid = j;
                }
            }
        }
        if(mid==-1) break;
        job_order[c++] = mid; 
        creationTime[mid]=-1;
    }
}

void main(){
    data d = init();
    char* s = (char*)malloc(BUF_SIZE);
    char* sc = (char*)malloc(BUF_SIZE);
    int* child_pid = (int*)malloc(sizeof(int)*MAX_PROC);
    char** names = (char**)malloc(sizeof(char*)*MAX_PROC);
    int creationTime[MAX_PROC];

    for(int i=0;i<MAX_PROC;i++) child_pid[i]=-1, creationTime[i]=-1;
    while(1){
        //check if child proc quit and update creationTime
        for(int i=0;i<MAX_PROC;i++) creationTime[i]=-1;

        for(int i=0;i<MAX_PROC;i++)
        {
            if(child_pid[i]!=-1){
                int rPID = waitpid(child_pid[i], &status, WNOHANG);
                if(rPID != 0){
                    fprintf(stderr, "%s with pid %d exited normally\n", names[i], child_pid[i]);
                    child_pid[i]=-1;
                }
                else{
                    int status;
                    int pid = child_pid[i];
                    int rPID = waitpid(child_pid[i], &status, WNOHANG);
                    if(rPID == 0){
                        char *proc_file_name = (char*)malloc(BUF_SIZE);
                        sprintf(proc_file_name, "/proc/%d/stat", pid);
                        char buf[1000];

                        int fd = open(proc_file_name, O_RDONLY);
                        if(fd<0) continue;
                        read(fd, buf, 1000);

                        int c = 1;
                        char *tok = strtok(buf, " \n");
                        while(tok!=NULL){
                            if(c==22) break;
                            tok = strtok(NULL, " \n");
                            c++;
                        }
                        char* crap = tok + strlen(tok)-1;
                        creationTime[i] = strtol(tok, &crap, 10);
                    }
                }
            }
        }

        update_jobs(creationTime);
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
            char* command = (char*) malloc(BUF_SIZE);
            strcpy(command, com_ar[i]);
            char** com_ar2 = (char**) malloc(1000*sizeof(char*));
            char* tok = strtok(command, "|\n");
            int c2=0;
            while(tok!=NULL){
                com_ar2[c2] = (char*) malloc(BUF_SIZE);
                strcpy(com_ar2[c2], tok);
                tok = strtok(NULL, "|\n");
                c2++;
            }
            int pipeFD[2];

            int act_std_in = dup(0), act_std_out = dup(1);
            int read = 0, write = 1;

            for(int j=0;j<c2;j++){ 
                if(read!=0) dup2(read, 0), close(read);                
                pipe(pipeFD);
                
                write = pipeFD[1];
                read = pipeFD[0];  
                dup2(write, 1), close(write);

                if(j==c2-1) dup2(act_std_out, 1);
                road_block(status , pid, child_pid, d, sc, com_ar2, j, names);   
              
            }
            dup2(act_std_in, 0);
            dup2(act_std_out,1);
        }


        if(kill_proc == 1) return;
    }
}
