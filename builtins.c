#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include"headers/config.h"
#include"headers/utilities.h"
#include"headers/prompt.h"
#include<limits.h>


void pwd(data d){
    char* cur_dir = (char*)malloc(PATH_SIZE);
    getcwd(cur_dir, PATH_SIZE);

    printf("/home/%s%s\n",d.user,&cur_dir[strlen(d.HOME)]);
}

void cd(char* input_dir, data d){
    char* old_dir = (char*)malloc(PATH_SIZE);
    getcwd(old_dir, PATH_SIZE);

    if(input_dir == NULL) input_dir="~";
    if(input_dir[0]=='~') {
        char* cur = (char*)(malloc(BUF_SIZE));
        strcpy(cur, d.HOME);
        strcat(cur, input_dir+1);
        input_dir = cur;
    }

    int cd_err = chdir(input_dir);
    char* new_dir = (char*)malloc(PATH_SIZE);
    getcwd(new_dir, PATH_SIZE);

    for(int i=0;i<strlen(d.HOME);i++){
        if(d.HOME[i]!=new_dir[i]){
            cd_err=-1;
            break;
        }
    }
    if(cd_err<0) {
        printf("the file or folder %s does not exist\n", input_dir);
        chdir(d.HOME);
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

