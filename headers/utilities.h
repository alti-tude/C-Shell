#ifndef UTILITIES_H
#define UTILITIES_H
data init();
void execute_this(int pid,data d, char * s);
void road_block(int  status , int pid, int* child_pid, data d, char* sc, char** com_ar, int i);
#endif
