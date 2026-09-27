/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/time.h>

#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/misc.h"

#include <fx/libfx.h>

int ck_getkey(int * keyptr); // khicas main.cc

int mp_hal_stdin_rx_chr(void) {
    int c;
    ck_getkey(&c);
    if (c == KEY_CTRL_EXE) 
      c = '\r';
    if (c == KEY_CTRL_EXIT) 
      c = 4; // EOF, ctrl-D
    return c;
}

#ifdef MICROPY_LIB
void console_output(const char *,int len);
#endif

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
#ifdef MICROPY_LIB
  int l=strlen(str);
  if (l==1 && str[l-1]==10) return;
  // printf("%s %i %i\n",str,l,l==0?-1:str[l-1]);
  console_output(str,len);
  return;
#else
    int ret = write(1, str, len);
    mp_uos_dupterm_tx_strn(str, len);
    (void)ret; // to suppress compiler warning
#endif
}

// cooked is same as uncooked because the terminal does some postprocessing
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    mp_hal_stdout_tx_strn(str, len);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

mp_uint_t mp_hal_ticks_ms(void) {
  return (RTC_GetTicks()*1000)/128;
}

mp_uint_t mp_hal_ticks_us(void) {
  return (RTC_GetTicks()*1000000)/128;
}
