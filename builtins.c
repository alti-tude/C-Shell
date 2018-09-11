#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include"headers/config.h"
#include"headers/utilities.h"
#include"headers/prompt.h"
#include<limits.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<fcntl.h>

void pwd(data d){
    char* cur_dir = (char*)malloc(PATH_SIZE);
    getcwd(cur_dir, PATH_SIZE);
    printf("%s\n", cur_dir);
    // printf("/home/%s%s\n",d.user,&cur_dir[strlen(d.HOME)]);
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
    // char* new_dir = (char*)malloc(PATH_SIZE);
    // getcwd(new_dir, PATH_SIZE);

    // for(int i=0;i<strlen(d.HOME);i++){
    //     if(d.HOME[i]!=new_dir[i]){
    //         cd_err=-1;
    //         break;
    //     }
    // }
    if(cd_err<0) {
        printf("the file or folder %s does not exist\n", input_dir);
        chdir(old_dir);
    }   
}

void echo(char* buf){
    char* tok = strtok(buf, delims);
    if(tok==NULL) return;
    else tok = strtok(NULL, delims);
    while(tok!=NULL){
        printf("%s ", tok);
        tok = strtok(NULL, delims);
    }
    printf("\n");
}

void pinfo(char* pid, data d){
    char *proc_file_name = (char*)malloc(BUF_SIZE);
    char spid[100];
    sprintf(proc_file_name, "/proc/%s/stat", pid);

    int fd = open(proc_file_name, O_RDONLY);
    if(fd<0) printf("process does not exist");
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
    if(i!=3 || (c=strtol(ar[1], crap, 10))==0) printf("Usage: remindme [time!=0] [msg]");
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
        // printf("%d\n", i);
        // sleep(1000);
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

