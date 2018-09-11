#ifndef BUILTINS_H
#define BUILTINS_H

void echo(char* buf);
void cd(char* input_dir, data d);
void pwd(data d);
void pinfo(char* pid, data d);
void remindme(data d, char* s);
void realtime(char *sc);
#endif