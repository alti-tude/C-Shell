#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/utsname.h>
#include<grp.h>
#include<pwd.h>
#include<sys/types.h>
#include<time.h>
#include"headers/config.h"


void usage(){
    fprintf(stderr,"Usage: ls [flags] [args]\n");
}

void print_perm(char *file){
    struct stat st;
    stat(file, &st);
    long long int temp = st.st_mode;
    char stor[] = "rwxrwxrwx";
    char perm[] = "---------";
    char filetype = '-';

    if(S_ISREG(temp)) filetype='-';
    if(S_ISDIR(temp)) filetype='d';
    if(S_ISLNK(temp)) filetype='l';
    if(S_ISBLK(temp)) filetype='b';
    if(S_ISCHR(temp)) filetype='c';
    if(S_ISFIFO(temp)) filetype='|';

    for(int i=8;i>=0;i--) 
    {
        if(temp%2==1) perm[i]=stor[i];
        temp/=2;
    }
    printf("%c%s\t", filetype, perm);
}
void print_janam_kundli(char* file){
    struct stat st;
    stat(file, &st);
    int uid =  st.st_uid;
    int gid = st.st_gid;
    struct group *grp = getgrgid(gid);
    struct passwd *usr = getpwuid(uid); 
    char* date = malloc(256);
    // printf("%ld\n", st.st_mtime);
    strftime(date, 256, "%b\t%d\t%H:%M", localtime(&st.st_mtime));
    printf("%s\t%s\t%ld\t\t\t\t%s\t", usr->pw_name, grp->gr_name, st.st_size, date);
}

void ls(char* flags, char* folder, data d){
    
    if(folder[0]=='~') {
        char* cur = (char*)(malloc(PATH_SIZE));
        strcpy(cur, d.HOME);
        strcat(cur, folder+1);
        folder = cur;
    }
    if(strcmp(flags, "la")==0 || strcmp(flags, "al")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }
        char* cpy = (char*)malloc(PATH_SIZE);

        while((dir_buf = readdir(dir)) != NULL){
            sprintf(cpy, "%s/%s", folder, dir_buf->d_name);
            print_perm(cpy);
            print_janam_kundli(cpy);
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else if(strcmp(flags, "l")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }

        char* cpy = (char*)malloc(PATH_SIZE);

        while((dir_buf = readdir(dir)) != NULL){
            if(dir_buf->d_name[0]=='.') continue;            
            sprintf(cpy, "%s/%s", folder, dir_buf->d_name);
            print_perm(cpy);
            print_janam_kundli(cpy);
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else if(strcmp(flags, "a")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }
        while((dir_buf = readdir(dir)) != NULL){
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else if(strcmp(flags, "none")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }
        while((dir_buf = readdir(dir)) != NULL){
            if(dir_buf->d_name[0]=='.') continue;            
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else{
        usage();
        return;
    }
}

void ls_main(int argc, char* argv[], data d){
    
    int fc[2]={0,0};
    // argc=0;
    // for(; argv[argc]!=NULL;argc++) printf("%s\n", argv[argc]);
    for(int i=1;i<argc;i++){
        if(argv[i][0]=='-'){
            for(int j=1; argv[i][j]!='\0'; j++)
                if(argv[i][j]=='l') fc[0]=1;
                else if(argv[i][j]=='a') fc[1]=1;
                else {
                    usage();
                    return;
                }
        }
    }

    char flags[7]="";
    if(fc[0]==1) strcat(flags, "l");
    if(fc[1]==1) strcat(flags, "a");
    if(fc[0]==0 && fc[1]==0) {
        strcpy(flags, "none");
    }
    int f = 0;
    for(int i=1;i<argc;i++){
        if(argv[i][0]!='-'){
            printf("folder: %s\n", argv[i]);
            ls(flags, argv[i], d);
            printf("\n");
            f = 1;
        }
    }

    if(f==0) ls(flags, ".", d);
    
}