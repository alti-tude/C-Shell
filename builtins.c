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

void pwd(data d){
    char* cur_dir = (char*)malloc(PATH_SIZE);
    getcwd(cur_dir, PATH_SIZE);
    printf("%s\n", cur_dir);
}

void cd(char* input_dir, data d){
    char* old_dir = (char*)malloc(PATH_SIZE);
    getcwd(old_dir, PATH_SIZE);

    if(input_dir == NULL) input_dir= "~";
    if(input_dir[0]=='~') {
        char* cur = (char*)(malloc(BUF_SIZE));
        strcpy(cur, d.HOME);
        strcat(cur, input_dir+1);
        input_dir = cur;
    }

    int cd_err = chdir(input_dir);
    if(cd_err<0) {
        printf("the file or folder %s does not exist\n", input_dir);
        chdir(old_dir);
    }   
}

void echo(char* buf){
    char* tok = strtok(buf, " \n\t|");
    if(tok==NULL) return;
    else tok = strtok(NULL, " \n\t|");
    while(tok!=NULL){
        if(tok[0]!='$') printf("%s ", tok);
        else printf("%s ", getenv(tok+1));
        
        tok = strtok(NULL, " \n\t|");
    }
    printf("\n");
}

void pinfo(char* pid, data d){
    char *proc_file_name = (char*)malloc(BUF_SIZE);
    char spid[100];
    sprintf(proc_file_name, "/proc/%s/stat", pid);

    int fd = open(proc_file_name, O_RDONLY);
    if(fd<0) {printf("process does not exist\n"); return;}
    struct stat f_stat;
    
    fstat(fd, &f_stat);
    read(fd, proc_file_name, 1000);
    printf("PID -- %s\n", pid);
    char *tok = strtok(proc_file_name, delims);
    for(int i=1;i<=22;i++){
        tok = strtok(NULL, delims);
        if(i==2) printf("Process Status -- %s\n", tok);
        if(i==22) printf("%s {Virtual Memory}\n", tok);
    }
    
    //exec path
    sprintf(proc_file_name, "/proc/%s/exe", pid);
    char *exec_path = (char*)malloc(PATH_SIZE);
    readlink(proc_file_name, exec_path, PATH_SIZE);
    int i;
    for(i = 0; i<strlen(d.HOME) && d.HOME[i]==exec_path[i] && strlen(exec_path)>=strlen(d.HOME); i++);
    if(i==strlen(d.HOME)) 
        printf("Executable path -- ~%s\n", exec_path+strlen(d.HOME));
    else 
        printf("Executable path -- %s\n", exec_path);
}  

void remindme(data d, char *s){
    char *tok;
    tok = strtok(s, delims);
    char **ar=(char**)malloc(100);
    int i=0;
    while(tok!=NULL){
        ar[i++]=tok;    
        tok = strtok(NULL, delims);
    }
    long long int c = 0;
    char **crap  = &(ar[2])-1;
    if(i!=3 || (c=strtol(ar[1], crap, 10))==0) 
    {
        printf("Usage: remindme [time!=0] [msg]");
        return;
    }
    sleep(c);
    printf("\nREMINDER \\{oo}/ %s\n", ar[2]);
    display_prompt(d);
}

volatile sig_atomic_t st;
void handle_sigint(){
    st=1;
}


void realtime(char* sc){
    signal(SIGINT, handle_sigint);

    char* t = strtok(sc, " \n");
    char* ar[3];
    int i=0;
    while(i<3 && t!=NULL){
        ar[i++] = t;
        t = strtok(NULL, " \n"); 
    }
    
    long long c = 0;
    char * temp;
    char ** end;
    if(i!=1){
        temp = ar[2]+strlen(ar[2])-1;
        end = &temp;
        c=strtol(ar[2],end,10);
    }
    
    if( ( i==3 && strcmp(ar[1], "-t")==0 && c!=0 )|| i==1)  {
        if(i==1) c=1;
        
        while(st==0){

            // sleep(1000);
            int fd = open("/proc/driver/rtc", O_RDONLY);
            char* buf=(char*)malloc(100);
            read(fd, buf, 100);

            char* lilTim = strtok(buf, "\n");
            char* bigTim = strtok(NULL, "\n");
            char* tim = strtok(bigTim, ":\n");
            tim = strtok(NULL, " \n");
            printf("%s\t    ", tim);  
            
            tim = strtok(lilTim, ":\n");
            tim = strtok(NULL, " \n");
            printf("%s\n", tim);
            close(fd);
            sleep(c);

        }
    }
    else printf("Usage: clock [-t] [gap !=0]\n");
    st = 0;
}


void setEnv(char* sc){
    char *tok = strtok(sc, " \n");
    char** com = (char**)(malloc(sizeof(char*)*10));

    int i = 0;
    while(tok!=NULL) 
    {   
        com[i] = (char*)malloc(BUF_SIZE);
        strcpy(com[i], tok);
        i++;
        tok = strtok(NULL, " \n");
    }

    if(i==2) setenv(com[1], "", 1);
    else if(i==3) setenv(com[1], com[2], 1);
    else printf("Usage: setenv variable [value]\n");
}

void unsetEnv(char* sc){
    char *tok = strtok(sc, " \n");
    char** com = (char**)(malloc(sizeof(char*)*10));

    int i = 0;
    while(tok!=NULL) 
    {   
        com[i] = (char*)malloc(BUF_SIZE);
        strcpy(com[i], tok);
        i++;
        tok = strtok(NULL, " \n");
    }

    if(i==2) unsetenv(com[1]);
    else printf("Usage: unsetenv variable\n");
}

void kjob(char* sc, int* child_pid){
    char *tok = strtok(sc, " \n");
    char** com = (char**)(malloc(sizeof(char*)*10));

    int i = 0;
    while(tok!=NULL) 
    {   
        com[i] = (char*)malloc(BUF_SIZE);
        strcpy(com[i], tok);
        i++;
        tok = strtok(NULL, " \n");
    }
    
    if(i==3){
        char* crap = com[2] + strlen(com[2])-1;
        int sig = strtol(com[2], &crap, 10);

        crap = com[1] + strlen(com[1])-1;
        int jobID = strtol(com[1], &crap, 10);
        jobID--;
        if(jobID<MAX_PROC && job_order[jobID] >= 0) 
            kill(child_pid[job_order[jobID]], sig);
        else printf("Usage: kjob job_id[must exist] signal\n");
    }
    else printf("Usage: kjob job_id signal\n");
}

void handleZ(){
    sent_to_bg = 1;
}

void handleC(){
    quit_proc = 1;
}

void fg(char* sc, int* child_pid){
    char *tok = strtok(sc, " \n");
    char** com = (char**)(malloc(sizeof(char*)*10));

    int i = 0;
    while(tok!=NULL) 
    {   
        com[i] = (char*)malloc(BUF_SIZE);
        strcpy(com[i], tok);
        i++;
        tok = strtok(NULL, " \n");
    }

    if(i==2){
        char* crap = com[1] + strlen(com[1])-1;
        int jobID = strtol(com[1], &crap, 10);
        jobID--;

        int status;
        if(jobID<MAX_PROC && job_order[jobID] >= 0) {
            signal(SIGTSTP, handleZ);
            signal(SIGINT, handleC);
            int pid = child_pid[job_order[jobID]];
            // kill(pid, SIGCONT);
            while(!sent_to_bg && !quit_proc && waitpid(child_pid[job_order[jobID]], &status, WNOHANG)!=child_pid[job_order[jobID]]);
            if(quit_proc) kill(child_pid[job_order[jobID]], 9);

            if(sent_to_bg){
                kill(child_pid[job_order[jobID]], SIGSTOP);
                setpgid(pid, pid);            
            }

            sent_to_bg = 0;
            quit_proc = 0;
        }
        else printf("Usage: fg job_id[must exist]\n");
    }
    else printf("Usage: fg job_id\n");
}

void bg(char* sc, int* child_pid){
    char *tok = strtok(sc, " \n");
    char** com = (char**)(malloc(sizeof(char*)*10));

    int i = 0;
    while(tok!=NULL) 
    {   
        com[i] = (char*)malloc(BUF_SIZE);
        strcpy(com[i], tok);
        i++;
        tok = strtok(NULL, " \n");
    }

    if(i==2){
        char* crap = com[1] + strlen(com[1])-1;
        int jobID = strtol(com[1], &crap, 10);
        jobID--;

        int status;
        if(jobID<MAX_PROC && job_order[jobID] >= 0) 
            kill(child_pid[job_order[jobID]], SIGCONT);
        else printf("Usage: bg job_id[must exist]\n");
    }
    else printf("Usage: bg job_id\n");
}

void overkill(int* child_pid){
    for(int i=0;i<MAX_PROC;i++)
        if(child_pid[i]!=-1)
            kill(child_pid[i], 9);
}

void jobs(int* child_pid, char** names){
    int creationTime[MAX_PROC];
    for(int i=0;i<MAX_PROC;i++){
        creationTime[i] = -1;
        if(child_pid[i]!=-1){
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
    int sorted[MAX_PROC];
    int c = 0;

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
        sorted[c++] = mid; 
        creationTime[mid]=-1;
    }

    for(int i=0;i<c; i++){
        int pid = child_pid[sorted[i]];
        char *proc_file_name = (char*)malloc(BUF_SIZE);
        sprintf(proc_file_name, "/proc/%d/stat", pid);
        char buf[1000];

        int fd = open(proc_file_name, O_RDONLY);
        if(fd<0) continue;
        read(fd, buf, 1000);

        printf("[%d]\t", i+1);
        
        int c = 1;
        char *tok = strtok(buf, " \n");
        while(tok!=NULL){
            if(c==3) break;
            tok = strtok(NULL, " \n");
            c++;
        }

        if(strcmp(tok,"S")==0 || strcmp(tok,"R")==0) printf("Running\t");
        else printf("Stopped\t");
        printf("%s[%d]\n", names[sorted[i]], pid);
    }
}
