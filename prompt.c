#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include"headers/config.h"
#include"headers/utilities.h"
#include<limits.h>

void display_prompt(data d){
    char* cur_dir = (char*)malloc(PATH_SIZE);
    getcwd(cur_dir, PATH_SIZE);

    if(strlen(d.HOME)>strlen(cur_dir)) 
        printf("%s@%s:%s >  ", d.user, d.host, cur_dir);
    else{
        int flag=0;
        for(int i =0;i<strlen(d.HOME);i++)
            if(d.HOME[i]!=cur_dir[i]){
                flag = 1;
                break;
            }
        
        if(flag == 1)
            printf("%s@%s:%s >  ", d.user, d.host, &cur_dir[0]);
        else
            printf("%s@%s:~%s >  ", d.user, d.host, &cur_dir[strlen(d.HOME)]);

            
    }
    
}
