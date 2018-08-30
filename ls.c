#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>

void usage(){
    printf("error in ls\n");
}

void print_perm(char *file){
    struct stat st;
    stat(file, &st);
    long long int temp = st.st_mode;
    char stor[] = "rwxrwxrwx";
    char perm[] = "---------";
    for(int i=8;i>=0;i--) 
    {
        if(temp%2==1) perm[i]=stor[i];
        temp/=2;
    }  
    printf("%s\t", perm);
}


void ls(char* flags, char* folder){
    
    if(strcmp(flags, "-la")==0 || strcmp(flags, "-al")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }
        while((dir_buf = readdir(dir)) != NULL){
            print_perm(dir_buf->d_name);
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else if(strcmp(flags, "-l")==0){
        DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(folder);
        if(dir==NULL) {
            usage();
            return;
        }
        while((dir_buf = readdir(dir)) != NULL){
            if(dir_buf->d_name[0]=='.') continue;            
            print_perm(dir_buf->d_name);
            printf("%s\n", dir_buf->d_name);
        }
        closedir(dir);
    }
    else if(strcmp(flags, "-a")==0){
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

void ls_main(int argc, char* argv[]){
    
    // ls("-la", ".");
    if(argc==1){
        ls("none", ".");
    } 
    else if(argc==2){
        if(argv[1][0]=='-') ls(argv[1], ".");
        else ls("none", argv[1]);
    }
    else if(argc==3){
        ls(argv[1], argv[2]);
    }
}