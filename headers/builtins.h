#ifndef BUILTINS_H
#define BUILTINS_H

void echo(char* buf);
void cd(char* input_dir, data d);
void pwd(data d);
void pinfo(char* pid, data d);
void remindme(data d, char* s);
void realtime(char *sc);
void setEnv(char *sc);
void unsetEnv(char *sc);
void kjob(char *sc, int* child_pid);
void fg(char *sc, int* child_pid);
void bg(char* sc, int* child_pid);
void overkill(int* child_pid);
void jobs(int *child_pid, char** names);
void handleZ();
#endif