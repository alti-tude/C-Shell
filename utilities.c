#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include<limits.h>
#include "config.h"

data init(){
    data ret;
    //home
    ret.HOME=(char*)(malloc(PATH_SIZE));
    getcwd(ret.HOME,PATH_SIZE);

    //user
    ret.user = (char*)(malloc(LOGIN_NAME_MAX));
    getlogin_r(ret.user, LOGIN_NAME_MAX);

    //host
    ret.host = (char*)(malloc(HOST_NAME_MAX));
    gethostname(ret.host, HOST_NAME_MAX);

    return ret;
}
