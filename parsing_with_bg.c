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
#include<signal.h>

void road_runner(int status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i, char** names);

void road_block(int status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i, char** names)
{
    char* infil;
    char* outfil;
    int temp_std_in = dup(0), temp_std_out = dup(1);

    strcpy(sc, com_ar[i]);
    char* out_redirect = strchr(sc, '>');
    if(out_redirect!=NULL){
        outfil = strtok(out_redirect+1, " >\t\n|");
        
        int fd;
        if(*(out_redirect+1) == '>') fd = open(outfil, O_APPEND| O_WRONLY | O_CREAT, 0644);
        else fd = open(outfil, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        dup2(fd, 1);
        close(fd);
        com_ar[i][out_redirect-sc] = '\0';
    }

    strcpy(sc, com_ar[i]);
    char* in_redirect = strchr(sc, '<');
    if(in_redirect!=NULL){
        infil = strtok(in_redirect+1, " >\t\n|");
    
        int fd = open(infil, O_RDONLY);
        if(fd<0) {perror("Error"); return;}
        else dup2(fd, 0);
        close(fd);
        com_ar[i][in_redirect-sc] = '\0';
    }

    road_runner(status, pid, child_pid, d, sc, com_ar, i, names);
    dup2(temp_std_in, 0);   
    close(temp_std_in); 
    dup2(temp_std_out, 1);
    close(temp_std_out); 
    if(kill_proc == 1) return;
}

void road_runner(int status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i, char** names){
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
    if(strcmp(tok, "ls")==0 || strcmp(tok, "echo")==0 || strcmp(tok, "overkill")==0 || strcmp(tok, "bg") ==0 || strcmp(tok, "fg")==0 || strcmp(tok, "kjob") == 0 || strcmp(tok, "unsetenv")==0 || strcmp(tok, "cd")==0 || strcmp(tok, "jobs") == 0 || strcmp(tok, "pinfo")==0 || strcmp(tok, "clock")==0 || strcmp(tok, "setenv")==0){
        // fprintf(stderr, "%s]\n", sc);
        execute_this(pid, d, sc, child_pid, names);
        return;
    }
    if(strcmp(tok,"quit")==0 ){
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
    names[i] = (char*)malloc(BUF_SIZE);
    strcpy(names[i], sc);
    //bg proc

    if(f!=0){
        if(pid==0) { 
            signal(SIGTSTP, handleZ);
            signal(SIGINT, handleC);
            quit_proc = 0;
            sent_to_bg = 0;
            execute_this(pid,d,sc, child_pid, names);
            kill_proc = 1;
            return;
        }
        else{
            setpgid(pid, pid);
            signal(SIGTSTP, handleZ);
            signal(SIGINT, handleC);
            quit_proc = 0;
            sent_to_bg = 0;
        }
    }
    else{ //fg proc
        if(pid==0) { //child
            execute_this(pid,d,sc, child_pid, names);
            kill_proc = 1;
            return;
        }
        else { //parent
            signal(SIGTSTP, handleZ);
            signal(SIGINT, handleC);

            while(!sent_to_bg && !quit_proc && waitpid(pid, &status, WNOHANG)!=pid);
	        for(i=0;i<MAX_PROC && child_pid[i]!=pid;i++);
           
            if(quit_proc) kill(child_pid[i], 9);
            else if(sent_to_bg){
                kill(child_pid[i], SIGSTOP);
                setpgid(pid, pid);            
            }
            else{
                child_pid[i]=-1;
            }

            sent_to_bg = 0;
            quit_proc = 0;
        }
    }


}
