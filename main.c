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

                if(j==c2-1) dup2(act_std_out, 1), fprintf(stderr, "adsdasdda%d%d\n", j, c2);
                road_runner(status , pid, child_pid, d, sc, com_ar2, j);   
              
            }

            // int act_std_in = dup(0), act_std_out = dup(1);
            // int read = dup(0);
            // int write = dup(1);

            // for(int j=0; j<c2; ++j) {
            //     dup2(read, 0);
            //     close(read);

            //     if(j==c2-1) write = dup(act_std_out);
            //     else{
            //         int pipefd[2];
            //         pipe(pipefd);
            //         write = pipefd[1];
            //         read = pipefd[0];
            //     }

            //     dup2(write,1);
            //     close(write);

            //     road_runner(status , pid, child_pid, d, sc, com_ar2, j);
            // }

            fprintf(stderr, "done\n");
            dup2(act_std_in, 0);
            dup2(act_std_out,1);
        }
    }
}
