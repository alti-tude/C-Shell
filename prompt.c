#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include"config.h"
#include"utilities.h"
#include<limits.h>

void display_prompt(data d){
    char* cur_dir = (char*)malloc(PATH_SIZE);
    getcwd(cur_dir, PATH_SIZE);

    printf("%s@%s: %s>\n", d.user, d.host, &cur_dir[strlen(d.HOME)]);
}
