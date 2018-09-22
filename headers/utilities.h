#ifndef UTILITIES_H
#define UTILITIES_H
data init();
void execute_this(int pid,data d, char * s, int* child_pid, char** names);
void road_block(int  status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i, char** names);
void road_runner(int  status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i, char** names);
#endif
