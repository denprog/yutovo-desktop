#include "mpconfig.h"
#include "misc.h"
#include "nlr.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "objtuple.h"
#include <fxcg/keyboard.h>

int ck_getkey(int * keyptr); // khicas main.cc

static mp_obj_t ion_readRTC()
{
  return mp_obj_new_int((RTC_GetTicks()*1000)/128);
}
static MP_DEFINE_CONST_FUN_OBJ_0(ion_readRTC_obj, ion_readRTC);


static mp_obj_t ion_getkey(size_t n_args, const mp_obj_t *args){
  if (n_args==0){
    unsigned int key; GetKey(&key);
    if (key==KEY_CTRL_LEFT) key=0;
    else if (key==KEY_CTRL_RIGHT) key=3;
    else if (key==KEY_CTRL_UP) key=1;
    else if (key==KEY_CTRL_DOWN) key=2;
    else if (key==KEY_CTRL_EXE) key=4;
    else if (key==KEY_CTRL_EXIT) key=5;    
    return mp_obj_new_int(key);
  }
  int expectkey=-1;
  if (MP_OBJ_IS_INT(args[0]))
    expectkey=mp_obj_get_int(args[0]);
  int c,r;
  short unsigned int key;
  int res = GetKeyWait_OS(&c,&r,1 /* KEYWAIT_HALTOFF_TIMEROFF*/,0,1,&key);
  //char buf[256]; sprint_int(buf,res); do_confirm(buf); sprint_int(buf,c); do_confirm(buf); sprint_int(buf,r); do_confirm(buf); sprint_int(buf,key); do_confirm(buf);
  if (res==1 /* KEYREP_KEYEVENT*/){
    key=kbd_convert(r,c);
    if (expectkey>=0 && key!=expectkey)
      return mp_obj_new_int(-2);      
    return mp_obj_new_int(key);
  }
  return mp_obj_new_int(-1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ion_getkey_obj, 0, 1, ion_getkey);

bool mp_int_float(mp_obj_t e,double * d); // in graphic.c
static mp_obj_t ion_sleep(size_t n_args, const mp_obj_t *args){
  double t=0;
  if (!mp_int_float(args[0],&t) || t<=0 || t>60)
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Expecting wait time in seconds (0 to 60)"));
  int i=t*1000; // milliseconds
  OS_InnerWait_ms(i);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ion_sleep_obj, 1, 1, ion_sleep);

STATIC const mp_map_elem_t mp_module_ion_globals_table[] = {
        { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ion) },
	//{ MP_OBJ_NEW_QSTR(MP_QSTR_Texture), (mp_obj_t) &ion_texture_type },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_getkey), (mp_obj_t) &ion_getkey_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_keydown), (mp_obj_t) &ion_getkey_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_sleep), (mp_obj_t) &ion_sleep_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_readRTC), (mp_obj_t) &ion_readRTC_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_monotonic), (mp_obj_t) &ion_readRTC_obj },
	{ MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t) &ion_readRTC_obj },
	{ MP_ROM_QSTR(MP_QSTR_KEY_LEFT), MP_OBJ_NEW_SMALL_INT(0) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_UP), MP_OBJ_NEW_SMALL_INT(1) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_DOWN), MP_OBJ_NEW_SMALL_INT(2) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_RIGHT), MP_OBJ_NEW_SMALL_INT(3) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_OK), MP_OBJ_NEW_SMALL_INT(4) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_BACK), MP_OBJ_NEW_SMALL_INT(5) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_HOME), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_F6) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_ONOFF), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_AC) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SHIFT), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_SHIFT) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_ALPHA), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_ALPHA) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_XNT), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_XTT) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_VAR), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_VARS) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_TOOLBOX), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_F4) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_BACKSPACE), MP_OBJ_NEW_SMALL_INT(KEY_CTRL_DEL) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_EXP), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_EXPN) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_LN), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_LN) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_LOG), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_LOG) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_IMAGINARY), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_0) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_COMMA), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_COMMA) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_POWER), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_POW) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SINE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_SIN) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_COSINE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_COS) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_TANGENT), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_TAN) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_PI), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_PI) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SQRT), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_SQUARE) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SQUARE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_SQUARE) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SEVEN), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_7) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_EIGHT), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_8) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_NINE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_9) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_LEFTPARENTHESIS), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_LPAR) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_RIGHTPARENTHESIS), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_RPAR) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_FOUR), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_4) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_FIVE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_5) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_SIX), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_6) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_MULTIPLICATION), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_MULT) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_DIVISION), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_DIV) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_ONE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_1) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_TWO), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_2) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_THREE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_3) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_PLUS), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_PLUS) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_MINUS), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_MINUS) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_ZERO), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_0) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_DOT), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_DP) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_EE), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_EXP) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_ANS), MP_OBJ_NEW_SMALL_INT(KEY_CHAR_PMINUS) },
	{ MP_ROM_QSTR(MP_QSTR_KEY_EXE), MP_OBJ_NEW_SMALL_INT(5) },
};

STATIC const mp_obj_dict_t mp_module_ion_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_ion_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_ion_globals_table),
        .table = (mp_map_elem_t*) mp_module_ion_globals_table,
    },
};

const mp_obj_module_t mp_module_ion = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*) &mp_module_ion_globals,
};
