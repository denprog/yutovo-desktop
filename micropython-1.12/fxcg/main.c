/* 
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
//#include <sys/stat.h>
//#include <sys/types.h>
#include <errno.h>
#include <fxcg/keyboard.h>

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/stackctrl.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/mperrno.h"
#include "extmod/misc.h"
#include "genhdr/mpversion.h"
#include "input.h"
#include "main.h"
#ifndef PATH_MAX
#define PATH_MAX 128
#endif

void raisememerr(){
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Memory error"));
}

void py_ck_ctrl_c(){
  // if (ctrl_c_py) raisememerr();
}

int gc_ramfree(){
#if MICROPY_ENABLE_GC
  gc_collect();
  gc_info_t i;
  gc_info(&i);
  return i.free;
#else
  return -1;
#endif
}

void console_output(const char *,int);
const char * read_file(const char * filename);
bool file_exists(const char * filename);


#if 1
const char numpy_script[]=R"(
import linalg
import math
class array: 
    def __init__(self, a): 
        self.a = a 

    def __add__(self, other):
        B=linalg.add(self.a , other.a)
        return array(B)

    def __sub__(self, other):
        B=linalg.sub(self.a , other.a)
        return array(B)

    def __mul__(self, other):
        if type(self)==array:
            if type(other)==array:
                B=linalg.mul(self.a , other.a)
                return array(B)
            B=linalg.mul(self.a,other)
            return array(B)
        B=linalg.mul(self,other.a)
        return array(B)

    def __rmul__(self, other): 
        if type(self)==array:
            if type(other)==array:
                return array(linalg.mul(self.a , other.a))
            return array(linalg.mul(self.a,other))
        return array(linalg.mul(self,other.a))

    def __matmul__(self, other):
        return __mul(self,other)

    def __getitem__(self,key):
        r=(self.a)[key]
        if type(r)==list or type(r)==tuple:
            return array(r)
        return r

    def __setitem__(self, key, value):
        if (type(value)==array):
            (self.a)[key]=value.a
        else:
            (self.a)[key]=value
        return None

    def __len__(self):
        return len(self.a)
    
    def __str__(self): 
        return 'array('+str(self.a)+')'
  
    def __repr__(self): 
        return 'array('+str(self.a)+')'
  
    def __neg__(self):
        return array(-self.a)

    def __pos__(self):
        return self
    
    def __abs__(self):
        return array(linalg.abs(self.a))

    def __round__(self):
        return array(linalg.apply(round,self.a,linalg.matrix))

    def __trunc__(self):
        return array(linalg.apply(trunc,self.a,linalg.matrix))

    def __floor__(self):
        return array(linalg.apply(floor,self.a,linalg.matrix))

    def __ceil__(self):
        return array(linalg.apply(ceil,self.a,linalg.matrix))

    def T(self):
        return array(linalg.transpose(self.a))
            
def real(x):
    if type(x)==array:
        return array(linalg.re(x.a))
    return x.real

def imag(x):
    if type(x)==array:
        return array(linalg.im(x.a))
    return x.imag

def conj(x):
    if type(x)==array:
        return array(linalg.conj(x.a))
    return linalg.conj(x)

def sin(x):
    if type(x)==array:
        return array(linalg.apply(math.sin,x.a,linalg.matrix))
    return math.sin(x)

def cos(x):
    if type(x)==array:
        return array(linalg.apply(math.cos,x.a,linalg.matrix))
    return math.cos(x)

def tan(x):
    if type(x)==array:
        return array(linalg.apply(math.tan,x.a,linalg.matrix))
    return math.tan(x)

def asin(x):
    if type(x)==array:
        return array(linalg.apply(math.asin,x.a,linalg.matrix))
    return math.asin(x)

def acos(x):
    if type(x)==array:
        return array(linalg.apply(math.acos,x.a,linalg.matrix))
    return math.acos(x)

def atan(x):
    if type(x)==array:
        return array(linalg.apply(math.atan,x.a,linalg.matrix))
    return math.atan(x)

def sinh(x):
    if type(x)==array:
        return array(linalg.apply(math.sinh,x.a,linalg.matrix))
    return math.sinh(x)

def cosh(x):
    if type(x)==array:
        return array(linalg.apply(math.cosh,x.a,linalg.matrix))
    return math.cosh(x)

def tanh(x):
    if type(x)==array:
        return array(linalg.apply(math.tanh,x.a,linalg.matrix))
    return math.tanh(x)

def exp(x):
    if type(x)==array:
        return array(linalg.apply(math.exp,x.a,linalg.matrix))
    return math.exp(x)

def log(x):
    if type(x)==array:
        return array(linalg.apply(math.log,x.a,linalg.matrix))
    return math.log(x)

def size(x):
    if type(x)==array:
        return linalg.size(x.a)
    return linalg.size(x)

def shape(x):
    if type(x)==array:
        return linalg.shape(x.a)

def dot(a,b):
    return a*b

def transpose(a):
    if type(x)==array:
        return array(linalg.transpose(x.a))

def trn(a):
    if type(x)==array:
        return array(linalg.conj(linalg.transpose(x.a)))
    return linalg.conj(linalg.transpose(x.a))

def zeros(n,m=0):
    return array(linalg.zeros(n,m))

def ones(n,m=0):
    return array(linalg.ones(n,m))

def eye(n):
    return array(linalg.eye(n))

def det(x):
    if type(x)==array:
        return linalg.det(x.a)
    return linalg.det(x)

def inv(x):
    if type(x)==array:
        return array(linalg.inv(x.a))
    return linalg.inv(x)

def solve(a,b):
    if type(a)==array:
        if type(b)==array:
            return array(linalg.solve(a.a,b.a))
        return array(linalg.solve(a.a,b))
    if type(b)==array:
        return array(linalg.solve(a,b.a))
    return linalg.solve(a,b)

def eig(a):
    if type(a)==array:
        r=linalg.eig(a.a)
        return array(r[0]),array(r[1])
    return linalg.eig(a)

def linspace(a,b,c):
    return array(linalg.linspace(a,b,c))

def arange(a,b,c=1):
    return array(linalg.arange(a,b,c))

def reshape(a,n,m=0):
    if type(n)==tuple:
        m=n[1]
        n=n[0]
    if type(a)==array:
        return array(linalg.matrix(n,m,a.a))
    return linalg.matrix(n,m,a)
)";

mp_lexer_t * mp_lexer_new_from_file(const char * filename) {
  const char * script=read_file(filename);
  if (!script && strcmp(filename,"numpy.py")==0){
    // desactivated since it crashes, no idea why
    // printf("using builtin numpy.py\n");
    script=numpy_script;
  }
  if (script) 
    return mp_lexer_new_from_str_len(qstr_from_str(filename), script, strlen(script), 0 /* size_t free_len*/);
  else
    mp_raise_OSError(MP_ENOENT);
}
#endif

mp_import_stat_t mp_import_stat(const char *path) {
  if (strcmp(path,"numpy.py")==0 || file_exists(path)) {
    return MP_IMPORT_STAT_FILE;
  }
  return MP_IMPORT_STAT_NO_EXIST;
}



void mp_keyboard_interrupt(void) {
  printf("Keyboard Intr\n");
  MP_STATE_VM(mp_pending_exception) = MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception));
}

int execution_in_progress_py = 0; // set to 1 during program execution

int intr_key=KEY_PRGM_ACON;
int ctrl_c_py=0;
static int PRGM_GetKey_() {
  unsigned char buffer[12];
  PRGM_GetKey_OS( buffer );
  return ( ( buffer[1] & 0x0F) *10 + ( ( buffer[2] & 0xF0) >> 4 ));
}


void mp_hal_set_interrupt_char(int c){ intr_key=c;}

// __attribute__((noinline))
int micropython_port_vm_hook_loop() {
  /* This function is called very frequently by the MicroPython engine. We grab
   * this opportunity to interrupt execution and/or refresh the display on
   * platforms that need it. */

  /* Doing too many things here slows down Python execution quite a lot. So we
   * only do things once in a while and return as soon as possible otherwise. */
  static int count=0;
  ++count;
  if ( (count & 0x7f) != 0x7f )
    return 1;
  short unsigned int key = PRGM_GetKey_();
  if (key == intr_key){
    if (!ctrl_c_py)
      printf("ACON pressed\n");
    ctrl_c_py += 1;
  }
  if (ctrl_c_py % 2)
    mp_keyboard_interrupt();
  else
    ctrl_c_py +=2;
  return 1;
}

// Command line options, with their defaults
// STATIC uint emit_opt = MP_EMIT_OPT_NONE;

STATIC void stderr_print_strn(void *env, const char *str, size_t len) {
  console_output(str,len); return;
    (void)env;
    ssize_t dummy = write(STDERR_FILENO, str, len);
    mp_uos_dupterm_tx_strn(str, len);
    (void)dummy;
}

const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};

#define FORCED_EXIT (0x100)
// If exc is SystemExit, return value where FORCED_EXIT bit set,
// and lower 8 bits are SystemExit value. For all other exceptions,
// return 1.
STATIC int handle_uncaught_exception(mp_obj_base_t *exc) {
    // check for SystemExit
    if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(exc->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
        // None is an exit value of 0; an int is its value; anything else is 1
        mp_obj_t exit_val = mp_obj_exception_get_value(MP_OBJ_FROM_PTR(exc));
        mp_int_t val = 0;
        if (exit_val != mp_const_none && !mp_obj_get_int_maybe(exit_val, &val)) {
            val = 1;
        }
        return FORCED_EXIT | (val & 255);
    }

    // Report all other exceptions
    mp_obj_print_exception(&mp_stderr_print, MP_OBJ_FROM_PTR(exc));
    return 1;
}

#define LEX_SRC_STR (1)
#define LEX_SRC_VSTR (2)
#define LEX_SRC_FILENAME (3)
#define LEX_SRC_STDIN (4)

// Returns standard error codes: 0 for success, 1 for all other errors,
// except if FORCED_EXIT bit is set then script raised SystemExit and the
// value of the exit is in the lower 8 bits of the return value
STATIC int execute_from_lexer(int source_kind, const void *source, mp_parse_input_kind_t input_kind, bool is_repl) {

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        // create lexer based on source kind
        mp_lexer_t *lex;
        if (source_kind == LEX_SRC_STR) {
            const char *line = source;
            lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, line, strlen(line), false);
        } else if (source_kind == LEX_SRC_VSTR) {
            const vstr_t *vstr = source;
            lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, vstr->buf, vstr->len, false);
        } else if (source_kind == LEX_SRC_FILENAME) {
            lex = mp_lexer_new_from_file((const char*)source);
        } else { // LEX_SRC_STDIN
            lex = mp_lexer_new_from_fd(MP_QSTR__lt_stdin_gt_, 0, false);
        }

        qstr source_name = lex->source_name;

        #if MICROPY_PY___FILE__
        if (input_kind == MP_PARSE_FILE_INPUT) {
            mp_store_global(MP_QSTR___file__, MP_OBJ_NEW_QSTR(source_name));
        }
        #endif
	parser_errorline=0; parser_errorcol=0;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);

        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, is_repl);

        // execute it
        mp_call_function_0(module_fun);
        // check for pending exception
        if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
            mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            nlr_raise(obj);
        }

        nlr_pop();
        return 0;
    } else {
        // uncaught exception
        return handle_uncaught_exception(nlr.ret_val);
    }
}

STATIC char *strjoin(const char *s1, int sep_char, const char *s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char *s = malloc(l1 + l2 + 2);
    memcpy(s, s1, l1);
    if (sep_char != 0) {
        s[l1] = sep_char;
        l1 += 1;
    }
    memcpy(s + l1, s2, l2);
    s[l1 + l2] = 0;
    return s;
}

STATIC int do_repl(void) {
    mp_hal_stdout_tx_str("MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "\n");

    // use simple readline

    for (;;) {
        char *line = prompt(">>> ");
        if (line == NULL) {
            // EOF
            return 0;
        }
        while (mp_repl_continue_with_input(line)) {
            char *line2 = prompt("... ");
            if (line2 == NULL) {
                break;
            }
            char *line3 = strjoin(line, '\n', line2);
            free(line);
            free(line2);
            line = line3;
        }

        int ret = execute_from_lexer(LEX_SRC_STR, line, MP_PARSE_SINGLE_INPUT, true);
        if (ret & FORCED_EXIT) {
            return ret;
        }

        free(line);
    }
}

#include "main.h"
int do_file(const char *file) {
    return execute_from_lexer(LEX_SRC_FILENAME, file, MP_PARSE_FILE_INPUT, false);
}

/*
STATIC int do_str(const char *str) {
    return execute_from_lexer(LEX_SRC_STR, str, MP_PARSE_FILE_INPUT, false);
}
*/

STATIC void set_sys_argv(char *argv[], int argc, int start_arg) {
    for (int i = start_arg; i < argc; i++) {
        mp_obj_list_append(mp_sys_argv, MP_OBJ_NEW_QSTR(qstr_from_str(argv[i])));
    }
}

MP_NOINLINE int main_(int argc, char **argv);


#if 0
int micropy_eval(const char * str){
  parser_errorline=0;
  mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
  mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
  return 0;
  mp_obj_t module_fun = mp_compile(&pt, lex->source_name,true);
  // mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::Back);
  mp_call_function_0(module_fun);
}
#else
int micropy_eval(const char * str){
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    //printf("%s ->mp \n", str);
    HourGlass();
    parser_errorline=0;
    //printf("ev1 %s\n",str);
    mp_lexer_t *lex = mp_lexer_new_from_str_len(0, str, strlen(str), false);
    //printf("ev2\n");
    mp_parse_tree_t pt = mp_parse(lex, MP_PARSE_FILE_INPUT);
    //printf("ev2\n");
    mp_obj_t module_fun = mp_compile(&pt, lex->source_name,true);
    //printf("ev3\n");
    // mp_hal_set_interrupt_char((int)Ion::Keyboard::Key::Back);
    mp_call_function_0(module_fun);
    //printf("ev4\n");
    // mp_hal_set_interrupt_char(-1); // Disable interrupt
    nlr_pop();
  } else { // Uncaught exception
#if 1
    handle_uncaught_exception(nlr.ret_val);
#else
    /* mp_obj_print_exception is supposed to handle error printing. However,
     * because we want to print custom information, we copied and modified the
     * content of mp_obj_print_exception instead of calling it. */
    if (mp_obj_is_exception_instance((mp_obj_t)nlr.ret_val)) {
        size_t n, *values;
        mp_obj_exception_get_traceback((mp_obj_t)nlr.ret_val, &n, &values);
        if (n > 0) {
            assert(n % 3 == 0);
            for (int i = n - 3; i >= 0; i -= 3) {
              if (values[i] != 0 || i == 0) {
                if (values[i] == 0) {
                  mp_printf(&mp_plat_print, "  Last command\n");
                } else {
#if MICROPY_ENABLE_SOURCE_LINE
                  mp_printf(&mp_plat_print, "  File \"%q\", line %d", values[i], (int)values[i + 1]);
#else
                  mp_printf(&mp_plat_print, "  File \"%q\"", values[i]);
#endif
                  // the block name can be NULL if it's unknown
                  qstr block = values[i + 2];
                  if (block == MP_QSTR_NULL) {
                    mp_print_str(&mp_plat_print, "\n");
                  } else {
                    mp_printf(&mp_plat_print, ", in %q\n", block);
                  }
                }
              }
            }
        }
    }
    mp_obj_print_helper(&mp_plat_print, (mp_obj_t)nlr.ret_val, PRINT_EXC);
    mp_print_str(&mp_plat_print, "\n");
    /* End of mp_obj_print_exception. */
#endif
  }
  //printf("ev5\n");
  return 0;
}
#endif

#ifndef MICROPY_LIB
int main(int argc, char **argv) {
    #if MICROPY_PY_THREAD
    mp_thread_init();
    #endif
    // We should capture stack top ASAP after start, and it should be
    // captured guaranteedly before any other stack variables are allocated.
    // For this, actual main (renamed main_) should not be inlined into
    // this function. main_() itself may have other functions inlined (with
    // their own stack variables), that's why we need this main/main_ split.
    mp_stack_ctrl_init();
    return main_(argc, argv);
}
#endif

char * micropy_init(int stack_size,int heap_size){
  // mp_stack_set_limit(40000 * (BYTES_PER_WORD / 4));

  //Disable output buffering, otherwise interactive mode becomes useless
  //setbuf(stdout, NULL);
  
  mp_stack_set_limit(stack_size);// (32768);

#if MICROPY_ENABLE_GC
#if 0 
  gc_test();
  return 0;
#endif
#if 0
  size_t addr=0x8c200000;
  addr+=2*1024*1024+256*1024;
  printf("mp_init %d : %p\n", 256*1024, addr);
  gc_init(addr,addr+256*1024-1);
#else
  char *heap = pythonjs_static_heap;
  if (!heap)
    heap=malloc(heap_size);
  if (!heap){
    return 0;
  }
  printf("mp_init %d : %p\n", heap_size, heap);
  gc_init(heap, heap + heap_size - 1);
#endif
  mp_init_py();
  mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
  mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

  return heap;
#endif
  mp_init_py();
  mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), 0);
  mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);
  printf("mp_init stack=%d nogc\n", stack_size);
  return (char *) 1;

}

MP_NOINLINE int main_(int argc, char **argv) {
  
  char * heap=micropy_init(16*1024,32*1024);
#if MICROPY_ENABLE_GC
  if (!heap)
    return 1;
#endif
    size_t path_num = 2;
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_path), path_num);
    mp_obj_t *path_items;
    mp_obj_list_get(mp_sys_path, &path_num, &path_items);
    mp_obj_list_init(MP_OBJ_TO_PTR(mp_sys_argv), 0);

    path_items[0] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    path_items[1] = MP_OBJ_NEW_QSTR(qstr_from_str("/documents/ndless"));

    mp_obj_list_init(mp_sys_argv, 0);
  
    const int NOTHING_EXECUTED = -2;
    int ret = NOTHING_EXECUTED;

    for (int a = 1; a < argc; a++) {
        char *pathbuf = malloc(PATH_MAX);
        char *basedir = realpath(argv[a], pathbuf);
        if (basedir == NULL) {
            mp_printf(&mp_stderr_print, "%s: can't open file '%s': [Errno %d] %s\n", argv[0], argv[a], errno, strerror(errno));
            // CPython exits with 2 in such case
            ret = 2;
            break;
        }

        // Set base dir of the script as first entry in sys.path
        char *p = strrchr(basedir, '/');
        path_items[0] = MP_OBJ_NEW_QSTR(qstr_from_strn(basedir, p - basedir));
        free(pathbuf);

        set_sys_argv(argv, argc, a);
        ret = do_file(argv[a]);
        break;
    }

    if (ret == NOTHING_EXECUTED) {
        ret = do_repl();
    }
    else
    {
        puts("Press any key to exit.");
        //wait_key_pressed();
    }

    mp_deinit();

    if (heap) free(heap);

    return ret & 0xff;
}


void nlr_jump_fail(void *val) {
    printf("FATAL: uncaught NLR %p\n", val);
    exit(1);
}

// Factory function for I/O stream classes
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
#if 1
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Not implemented (micropython-1.12/fxcg/main.c)"));
#else
    // TODO: analyze buffering args and instantiate appropriate type
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kwargs, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
    return fdfile_open(&mp_type_textio, arg_vals);
#endif
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
