#ifndef MICROPY_MAIN_H
#define MICROPY_MAIN_H
int do_file(const char *file);
char * micropy_init();
int micropy_eval(const char * line);
void  mp_deinit();
void  mp_stack_ctrl_init();
extern int execution_in_progress_py;
extern int ctrl_c_py;
void raisememerr();
void py_ck_ctrl_c();
extern char * pythonjs_static_heap;
void HourGlass(void) ;
int gc_ramfree();
#endif
