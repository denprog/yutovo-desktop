/* -*- mode:C -*- */
#if defined EMCC && !defined NO_QSTR
#include <emscripten.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "gc.h"
#include "qstr.h"
#include "obj.h"
#include "objfun.h"
#include "objstr.h"
#include "objint.h"
#include "runtime.h"
#include "mpz.h"
#include "lexer.h"
#include "parse.h"
#include "compile.h"
#include <fx/libfx.h>
#define RAND_MAX 2147483647
void py_ck_ctrl_c(); // in main.c
extern const struct _mp_obj_module_t mp_module_graphic;
// basic
#ifdef NUMWORKS
void c_sprint_double(char * s,double d);
void strcat_double(char * s,double d){
  for (;*s;)
    ++s;
  c_sprint_double(s,d);
}
int numworks_get_pixel(int x,int y);
int os_get_pixel(int x,int y);
void sync_screen();
#else
int os_get_pixel(int x,int y);
void sync_screen();
#endif // NUMWORKS
void c_set_pixel(int x,int y,int c); // c_ prefixed functions run freeze=true
void c_fill_rect(int x,int y,int w,int h,int c);
int os_draw_string_small(int x,int y,int c,int bg,const char * s,bool fake);
int os_draw_string(int x,int y,int c,int bg,const char * s,bool fake);
int c_draw_string(int x,int y,int c,int bg,const char * s,bool fake);
int c_draw_string_small(int x,int y,int c,int bg,const char * s,bool fake);
int c_draw_string_medium(int x,int y,int c,int bg,const char * s,bool fake);
// more complex paths
void c_draw_rectangle(int x,int y,int w,int h,int c);
void c_draw_line(int x0,int y0,int x1,int y1,int c);
void c_draw_circle(int xc,int yc,int r,int color,bool q1,bool q2,bool q3,bool q4);
void c_draw_filled_circle(int xc,int yc,int r,int color,bool left,bool right);
void c_draw_polygon(int * x,int *y ,int n,int color);
void c_draw_filled_polygon(int * x,int *y, int n,int xmin,int xmax,int ymin,int ymax,int color);
void c_draw_arc(int xc,int yc,int rx,int ry,int color,double theta1, double theta2);
void c_draw_filled_arc(int x,int y,int rx,int ry,int theta1_deg,int theta2_deg,int color,int xmin,int xmax,int ymin,int ymax,bool segment);
void c_turtle_forward(double d);
void c_turtle_left(double d);
void c_turtle_up(int i);
void c_turtle_goto(double x,double y);
void c_turtle_cap(double x);
int c_turtle_crayon(int i);
void c_turtle_rond(int x,int y,int z);
void c_turtle_disque(int x,int y,int z,int centre);
void c_turtle_fill(int i);
void c_turtle_fillcolor(double r,double g,double b,int entier);
void c_turtle_getposition(double * x,double * y);
void c_turtle_clear(int clrpos);
void c_turtle_show(int visible);
int c_turtle_getcap();
void c_turtle_towards(double x,double y);
int c_turtle_getcolor();
void c_turtle_color(int);
void c_turtle_fillcolor1(int c);
int getkeywait(int * c,int * r,int,int,int,short unsigned int * key);
int kbd_convert(int r,int c);
int kbd_filter(int key); // replace up/down/right/left/exe/exit with Numworks keycodes
int do_confirm(const char *);
//const double M_PI=3.1415926535897932;
#define LCD_WIDTH 320
#define LCD_HEIGHT 222
int select_item(const char ** ptr,const char * title,bool askfor1);

#define mp_rgb(r,g,b) ((((r*32)/256)<<11) | (((g*64)/256)<<5) | (b*32/256))

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

void raisetypeerr(){
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Bad argument type"));
}
bool mp_int_float(mp_obj_t e,double * d){
  *d=0;
  if (mp_obj_is_float(e)){
    *d=mp_obj_get_float(e);
    return true;
  }
  if (MP_OBJ_IS_INT(e)){
    *d=mp_obj_get_int(e);
    return true;
  }
  return false;
}

const char * translate_point_type(const char * s){
  if (strcmp("+",s)==0)
    return "plus_point";
  if (strcmp("*",s)==0)
    return "star_point";
  if (strcmp("x",s)==0)
    return "point_croix";
  if (strcmp("o",s)==0)
    return "square_point";
  return s;
}

int mp_int_float2color(mp_obj_t e){
  if (mp_obj_is_float(e)){
    double d=mp_obj_get_float(e);
    if (d<=0) return 0;
    if (d>=1) return 255;
    int r=d*256;
    return r;
  }
  int r=mp_obj_get_int(e);
  if (r<=0) return 0;
  if (r>255) return 255;
  return r;
}

int mp_get_color(mp_obj_t tuple){
  if (MP_OBJ_IS_SMALL_INT(tuple)) 
    return MP_OBJ_SMALL_INT_VALUE(tuple) & 0xffffffff;
  if (MP_OBJ_IS_STR(tuple)){
    const char * col=mp_obj_str_get_str(tuple);
    if (strcmp(col,"black")==0)
      return 0xffff;
    if (strcmp(col,"white")==0)
      return 0;
    if (strcmp(col,"blue")==0)
      return 2079;
    if (strcmp(col,"red")==0)
      return 63488;
    if (strcmp(col,"green")==0)
      return 2016;
    if (strcmp(col,"magenta")==0)
      return 63519;
    if (strcmp(col,"yellow")==0)
      return 65504;
    if (strcmp(col,"cyan")==0)
      return 2047;
    return -1;
  }
  size_t len;
  mp_obj_t * elem;

  mp_obj_get_array(tuple, &len, &elem);
  if (len != 3) {
    mp_raise_TypeError("color needs 3 components");
  }
  int r=mp_int_float2color(elem[0]);
  int g=mp_int_float2color(elem[1]);
  int b=mp_int_float2color(elem[2]);
  return mp_rgb(r,g,b);
}

static mp_obj_t graphic_clear_screen(size_t n_args, const mp_obj_t *args){
  int c=0xffff;
  if (n_args==1) //  && MP_OBJ_IS_SMALL_INT(args[0]))
    c=mp_get_color(args[0]); //MP_OBJ_SMALL_INT_VALUE(args[0]);
  c_fill_rect(0,0,LCD_WIDTH,LCD_HEIGHT,c);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_clear_screen_obj, 0, 1, graphic_clear_screen);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_clear_obj, 0, 1, graphic_clear_screen);

static mp_obj_t graphic_show_screen(size_t n_args, const mp_obj_t *args){
#ifndef NUMWORKS
  sync_screen();
#endif
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_show_screen_obj, 0,1,graphic_show_screen);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_show_obj, 0,1,graphic_show_screen);

mp_obj_t mp_color_tuple(int c){
  mp_obj_tuple_t * t = (mp_obj_tuple_t *)(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
  c &= 0xffff;
  int r=(c>>11)&0x1f,g=(c>>5)&0x3f,b=c&0x1f;
  r *= 8; g*=4; b*=8;
  t->items[0] = MP_OBJ_NEW_SMALL_INT(r);
  t->items[1] = MP_OBJ_NEW_SMALL_INT(g);
  t->items[2] = MP_OBJ_NEW_SMALL_INT(b);
  return MP_OBJ_FROM_PTR(t);
}  

static mp_obj_t graphic_set_pixel(size_t n_args, const mp_obj_t *args) {
  if (n_args<2){
    sync_screen();
    return mp_const_none;
  }
  uint16_t x = mp_obj_get_int(args[0]), y = mp_obj_get_int(args[1]),color=0;
  if (n_args==3)    
    color = mp_get_color(args[2]);
  c_set_pixel(x,y,color);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_set_pixel_obj, 0, 3, graphic_set_pixel);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_pixel_obj, 2, 3, graphic_set_pixel);


static mp_obj_t graphic_draw_line(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    x2 = mp_obj_get_int(args[2]), y2 = mp_obj_get_int(args[3]),
    color=0;
  if (n_args==5)    
    color = mp_get_color(args[4]);
  c_draw_line(x1,y1,x2,y2,color);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_line_obj, 4, 5, graphic_draw_line);

bool mp_array2inttab(mp_obj_t l,int ** x,size_t * n,size_t * m){
  mp_obj_t * elem;
  mp_obj_get_array(l, n, &elem);
  if (*n==0)
    return false;
  mp_obj_t * line;
  mp_obj_get_array(elem[0],m,&line);
  if (*m==0)
    return false;
  int * ptr=(int *)malloc((*n)*(*m)*sizeof(int));
  *x=ptr;
  for (size_t i=0;i<*n;++i){
    size_t M;
    mp_obj_get_array(elem[i],&M,&line);
    if (M!=*m){
      free(*x);
      return false;
    }
    for (size_t j=0;j<*m;++j){
      *ptr=mp_obj_get_int(line[j]);
      ++ptr;
    }
  }
  return true;
}

static mp_obj_t graphic_draw_polygon_(size_t n_args, const mp_obj_t *args,bool filled) {
  int color=0;
  int * tab=0;
  size_t n=0,m=0;
  if (!mp_array2inttab(args[0],&tab,&n,&m))
      return mp_const_false;
  if (m!=2){
    free(tab);
    return mp_const_false;
  }
  int x[n],y[n];
  for (size_t i=0;i<n;++i){
    x[i]=*tab;
    ++tab;
    y[i]=*tab;
    ++tab;
  }
  if (n_args==2)    
    color = mp_get_color(args[1]);
  if (filled)
    c_draw_filled_polygon(x,y,n,0,LCD_WIDTH,0,LCD_HEIGHT,color);
  else
    c_draw_polygon(x,y,n,color);
  return mp_const_none;
}
static mp_obj_t graphic_draw_polygon(size_t n_args, const mp_obj_t *args) {
  return graphic_draw_polygon_(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_polygon_obj, 1, 2, graphic_draw_polygon);
static mp_obj_t graphic_draw_filled_polygon(size_t n_args, const mp_obj_t *args) {
  return graphic_draw_polygon_(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_filled_polygon_obj, 1, 2, graphic_draw_filled_polygon);

static mp_obj_t graphic_draw_rectangle(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    x2 = mp_obj_get_int(args[2]), y2 = mp_obj_get_int(args[3]),
    color=0;
  if (n_args==5)    
    color = mp_get_color(args[4]);
  c_draw_rectangle(x1,y1,x2,y2,color);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_rectangle_obj, 4, 5, graphic_draw_rectangle);

static mp_obj_t graphic_fill_rect(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    x2 = mp_obj_get_int(args[2]), y2 = mp_obj_get_int(args[3]),
    color=0;
  if (n_args==5)    
    color = mp_get_color(args[4]);
  c_fill_rect(x1,y1,x2,y2,color);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_fill_rect_obj, 4, 5, graphic_fill_rect);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_filled_rectangle_obj, 4, 5, graphic_fill_rect);

static mp_obj_t graphic_draw_circle(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    r = mp_obj_get_int(args[2]),
    color=0;
  if (n_args==4)    
    color = mp_get_color(args[3]);
  c_draw_circle(x1,y1,r,color,true,true,true,true);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_circle_obj, 3, 4, graphic_draw_circle);

static mp_obj_t graphic_draw_filled_circle(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    r = mp_obj_get_int(args[2]),
    color=0;
  if (n_args==4)    
    color = mp_get_color(args[3]);
  c_draw_filled_circle(x1,y1,r,color,true,true);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_filled_circle_obj, 3, 4, graphic_draw_filled_circle);

static mp_obj_t graphic_draw_arc(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    r1 = mp_obj_get_int(args[2]), r2=mp_obj_get_int(args[3]),
    t1=0,t2=360,
    color=0;
  if (n_args>=6){
    t1 = mp_obj_get_int(args[4]), t2=mp_obj_get_int(args[5]);
  }
  if (n_args==7)    
    color = mp_get_color(args[6]);
  c_draw_arc(x1,y1,r1,r2,color,t1*M_PI/180,t2*M_PI/180);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_arc_obj, 4, 7, graphic_draw_arc);

static mp_obj_t graphic_draw_filled_arc(size_t n_args, const mp_obj_t *args) {
  int x1 = mp_obj_get_int(args[0]), y1 = mp_obj_get_int(args[1]),
    r1 = mp_obj_get_int(args[2]), r2=mp_obj_get_int(args[3]),
    t1=0,t2=360,
    color=0;
  if (n_args>=6){
    t1 = mp_obj_get_int(args[4]), t2=mp_obj_get_int(args[5]);
  }
  if (n_args>=7)    
    color = mp_get_color(args[6]);
  c_draw_filled_arc(x1,y1,r1,r2,t1,t2,color,0,LCD_WIDTH,0,LCD_HEIGHT,n_args==8);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_filled_arc_obj, 4, 8, graphic_draw_filled_arc);

static mp_obj_t graphic_get_pixel(size_t n_args, const mp_obj_t *args) {
  uint16_t x = mp_obj_get_int(args[0]), y = mp_obj_get_int(args[1]);
  if (x<0 || x>=LCD_WIDTH || y<0 || y>=LCD_HEIGHT)
    return mp_const_none;
  int c=os_get_pixel(x,y);
  if (n_args==3) 
    return MP_OBJ_NEW_SMALL_INT(c);
  return mp_color_tuple(c);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_get_pixel_obj, 2, 3, graphic_get_pixel);

static mp_obj_t graphic_draw_string(size_t n_args, const mp_obj_t *args) {
  uint16_t x = mp_obj_get_int(args[0]), y = mp_obj_get_int(args[1]),c=0,bg=0xffff;
  const char * text = mp_obj_str_get_str(args[2]);
  if (n_args>=4)
    c = mp_get_color(args[3]);
  const char * font = 0;
  if (n_args>=5){
    int c=mp_get_color(args[4]);
    if (c==-1)
      font=mp_obj_str_get_str(args[4]);
    else
      bg =c;
  }
  if (n_args>=6)
    font=mp_obj_str_get_str(args[5]);
  if (font && strcmp(font,"small")==0)
    c_draw_string_small(x,y,c,bg,text,false);
  else {
    if (font && strcmp(font,"large")==0)
      c_draw_string(x,y,c,bg,text,false);
    else
      c_draw_string_medium(x,y,c,bg,text,false);
  }
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_draw_string_obj, 3, 6, graphic_draw_string);

static mp_obj_t graphic_color(size_t n_args, const mp_obj_t *args) {
  int r=mp_int_float2color(args[0]);
  int g=mp_int_float2color(args[1]);
  int b=mp_int_float2color(args[2]);
  return mp_color_tuple(mp_rgb(r,g,b));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(graphic_color_obj, 3, 3, graphic_color);

//
static const mp_map_elem_t graphic_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_get_pixel), (mp_obj_t) &graphic_get_pixel_obj },
	{ MP_ROM_QSTR(MP_QSTR_cyan), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,255,255)) },
	{ MP_ROM_QSTR(MP_QSTR_magenta), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,0,255)) },
	{ MP_ROM_QSTR(MP_QSTR_yellow), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,255,0)) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,0,255)) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,0,0)) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,255,0)) },
    { MP_ROM_QSTR(MP_QSTR_black), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,0,0)) },
    { MP_ROM_QSTR(MP_QSTR_white), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,255,255)) },
	{ MP_ROM_QSTR(MP_QSTR_clear_screen), (mp_obj_t) &graphic_clear_screen_obj },
	{ MP_ROM_QSTR(MP_QSTR_clear), (mp_obj_t) &graphic_clear_obj },
	{ MP_ROM_QSTR(MP_QSTR_show_screen), (mp_obj_t) &graphic_show_screen_obj },
	{ MP_ROM_QSTR(MP_QSTR_set_pixel), (mp_obj_t) &graphic_set_pixel_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_pixel), (mp_obj_t) &graphic_draw_pixel_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_line), (mp_obj_t) &graphic_draw_line_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_polygon), (mp_obj_t) &graphic_draw_polygon_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_filled_polygon), (mp_obj_t) &graphic_draw_filled_polygon_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_rectangle), (mp_obj_t) &graphic_draw_rectangle_obj },
	{ MP_ROM_QSTR(MP_QSTR_fill_rect), (mp_obj_t) &graphic_fill_rect_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_filled_rectangle), (mp_obj_t) &graphic_draw_filled_rectangle_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_circle), (mp_obj_t) &graphic_draw_circle_obj },
 	{ MP_ROM_QSTR(MP_QSTR_draw_filled_circle), (mp_obj_t) &graphic_draw_filled_circle_obj },
 	{ MP_ROM_QSTR(MP_QSTR_draw_arc), (mp_obj_t) &graphic_draw_arc_obj },
 	{ MP_ROM_QSTR(MP_QSTR_draw_filled_arc), (mp_obj_t) &graphic_draw_filled_arc_obj },
	{ MP_ROM_QSTR(MP_QSTR_draw_string), (mp_obj_t) &graphic_draw_string_obj },
	{ MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t) &graphic_color_obj },
};


static MP_DEFINE_CONST_DICT(graphic_locals_dict, graphic_locals_dict_table);

const mp_obj_type_t graphic_type = {
    { &mp_type_type },
    .name = MP_QSTR_graphic,
    .locals_dict = (mp_obj_t)&graphic_locals_dict
};


STATIC const mp_map_elem_t mp_module_graphic_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__graphic) },
	{ MP_ROM_QSTR(MP_QSTR_cyan), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,255,255)) },
	{ MP_ROM_QSTR(MP_QSTR_magenta), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,0,255)) },
	{ MP_ROM_QSTR(MP_QSTR_yellow), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,255,0)) },
    { MP_ROM_QSTR(MP_QSTR_blue), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,0,255)) },
    { MP_ROM_QSTR(MP_QSTR_red), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,0,0)) },
    { MP_ROM_QSTR(MP_QSTR_green), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,255,0)) },
    { MP_ROM_QSTR(MP_QSTR_black), MP_OBJ_NEW_SMALL_INT(mp_rgb(0,0,0)) },
    { MP_ROM_QSTR(MP_QSTR_white), MP_OBJ_NEW_SMALL_INT(mp_rgb(255,255,255)) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), (mp_obj_t) &graphic_set_pixel_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_pixel), (mp_obj_t) &graphic_draw_pixel_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_line), (mp_obj_t) &graphic_draw_line_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_polygon), (mp_obj_t) &graphic_draw_polygon_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_filled_polygon), (mp_obj_t) &graphic_draw_filled_polygon_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_rectangle), (mp_obj_t) &graphic_draw_rectangle_obj },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), (mp_obj_t) &graphic_fill_rect_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_filled_rectangle), (mp_obj_t) &graphic_draw_filled_rectangle_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_circle), (mp_obj_t) &graphic_draw_circle_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_filled_circle), (mp_obj_t) &graphic_draw_filled_circle_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_arc), (mp_obj_t) &graphic_draw_arc_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_filled_arc), (mp_obj_t) &graphic_draw_filled_arc_obj },
    { MP_ROM_QSTR(MP_QSTR_get_pixel), (mp_obj_t) &graphic_get_pixel_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_string), (mp_obj_t) &graphic_draw_string_obj },
    { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t) &graphic_color_obj },
    { MP_ROM_QSTR(MP_QSTR_show_screen), (mp_obj_t) &graphic_show_screen_obj },
    { MP_ROM_QSTR(MP_QSTR_clear_screen), (mp_obj_t) &graphic_clear_screen_obj },
    { MP_ROM_QSTR(MP_QSTR_clear), (mp_obj_t) &graphic_clear_obj },
    { MP_ROM_QSTR(MP_QSTR_show), (mp_obj_t) &graphic_show_obj },

};

STATIC const mp_obj_dict_t mp_module_graphic_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_graphic_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_graphic_globals_table),
        .table = (mp_map_elem_t*)mp_module_graphic_globals_table,
    },
};

const mp_obj_module_t mp_module_graphic = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_graphic_globals,
};

/* ********************************
 *                  CAS           *
 * ******************************** */
int mp_token(const char * line){ // does not work, always mp_type_fun_bc
  mp_obj_dict_t *dict = NULL;
  dict = mp_locals_get();//mp_globals_get();//
  mp_map_t * m=mp_obj_dict_get_map(dict);
  mp_obj_t index=mp_obj_new_str(line,strlen(line));
  mp_map_elem_t *ptr=mp_map_lookup(m, index, MP_MAP_LOOKUP) ;
  // 0 or 3
  if (ptr)
    return 3;
  else
    return 0;
}

const char * const * mp_vars();
const char * const * mp_vars(){
  mp_obj_dict_t *dict = NULL;
  dict = mp_locals_get();

  mp_obj_t dir = mp_obj_new_list(0, NULL);
  if (dict != NULL) {
    for (size_t i = 0; i < dict->map.alloc; i++) {
      if (MP_MAP_SLOT_IS_FILLED(&dict->map, i)) {
	//const char * ch=mp_obj_str_get_str(dict->map.table[i].key);
	mp_obj_t v=dict->map.table[i].value;
	mp_obj_type_t * vt=mp_obj_get_type(v);
	if (vt!=&mp_type_fun_builtin_var
	    && vt!=&mp_type_fun_builtin_0
	    && vt!=&mp_type_fun_builtin_1
	    && vt!=&mp_type_fun_builtin_2
	    && vt!=&mp_type_fun_builtin_3
	    )
	  mp_obj_list_append(dir, dict->map.table[i].key);
      }
    }
  }
  mp_obj_t * line; size_t m=0;
  mp_obj_get_array(dir,&m,&line);
  const char ** tab=(const char **)malloc((m+1)*sizeof(char *));
  if (!tab) return 0;
  for (size_t i=0;i<m;++i){
    tab[i]=mp_obj_str_get_str(line[i]);
  }
  tab[m]=0;
  for (size_t i=0;i<m;++i){
    if (strcmp(tab[i],"__name__")==0){
      tab[i]=tab[m-1];
      tab[m-1]="del ";
      break;
    }
  }
  return tab;
}

/* ********************************z
 *         LINALG & CAS           *
 * ******************************** */
bool r_inv(double *,int n);
bool r_rref(double *,int n,int m);
double r_det(double *,int);
struct double_pair {
  double r,i;
} ;
typedef struct double_pair c_complex;
bool c_inv(c_complex *,int n);
bool c_rref(c_complex *,int n,int m);
c_complex c_det(c_complex *,int);
bool c_egv(c_complex * x,int n); // eigenvectors
bool c_proot(c_complex * x,int n); 
bool c_pcoeff(c_complex * x,int n); 
bool c_fft(c_complex * x,int n,bool inverse); 
bool c_eig(c_complex * x,c_complex * d,int n); // eigenvectors
bool mp_get_c_complex(mp_obj_t e,c_complex * d){
  d->r=d->i=0;
  if (mp_obj_is_float(e)){
    d->r=mp_obj_get_float(e);
    return true;
  }
  if (MP_OBJ_IS_INT(e)){
    d->r=mp_obj_get_int(e);
    return true;
  }
  if (mp_obj_get_type(e)==&mp_type_complex){
    mp_obj_get_complex(e,&d->r,&d->i);
    return true;
  }
  return false;
}

void c_mul(const c_complex a,const c_complex b,c_complex * c){
  c->r=a.r*b.r-a.i*b.i;
  c->i=a.r*b.i+a.i*b.r;
}

void c_sub(const c_complex a,const c_complex b,c_complex * c){
  c->r=a.r-b.r;
  c->i=a.i-b.i;
}

void c_add(const c_complex a,const c_complex b,c_complex * c){
  c->r=a.r+b.r;
  c->i=a.i+b.i;
}

bool mp_array2doubletab(mp_obj_t l,double ** x,size_t * n,size_t * m){
  if (mp_obj_get_type(l)!=&mp_type_tuple &&
      mp_obj_get_type(l)!=&mp_type_list)
    return false;
  mp_obj_t * elem;
  mp_obj_get_array(l, n, &elem);
  if (*n==0)
    return false;
  mp_obj_t * line;
  *m=0;
  if (mp_obj_get_type(elem[0])==&mp_type_tuple ||
      mp_obj_get_type(elem[0])==&mp_type_list)
    mp_obj_get_array(elem[0],m,&line);
  if (*m==0){
    double * ptr=(double *)malloc((*n)*sizeof(double));
    *x=ptr;
    for (size_t i=0;i<*n;++i){
      if (MP_OBJ_IS_SMALL_INT(elem[i])) 
	*ptr=MP_OBJ_SMALL_INT_VALUE(elem[i]);
      else {
	if (mp_obj_is_float(elem[i]))
	  *ptr=mp_obj_float_get(elem[i]);
	else {
	  free(*x);
	  return false;
	}
      }
      ++ptr;
    }
    return true;
  }
  double * ptr=(double *)malloc((*n)*(*m)*sizeof(double));
  *x=ptr;
  for (size_t i=0;i<*n;++i){
    size_t M;
    mp_obj_get_array(elem[i],&M,&line);
    if (M!=*m){
      free(x);
      return false;
    }
    for (size_t j=0;j<*m;++j){
      if (MP_OBJ_IS_SMALL_INT(line[j])) 
	*ptr=MP_OBJ_SMALL_INT_VALUE(line[j]);
      else {
	if (mp_obj_is_float(line[j]))
	  *ptr=mp_obj_float_get(line[j]);
	else {
	  free(*x);
	  return false;
	}
      }
      ++ptr;
    }
  }
  return true;
}

bool mp_array2c_complextab(mp_obj_t l,c_complex ** x,size_t * n,size_t * m){
  if (mp_obj_get_type(l)!=&mp_type_tuple &&
      mp_obj_get_type(l)!=&mp_type_list)
    return false;
  mp_obj_t * elem;
  mp_obj_get_array(l, n, &elem);
  if (*n==0)
    return false;
  mp_obj_t * line;
  *m=0;
  if (mp_obj_get_type(elem[0])==&mp_type_tuple ||
      mp_obj_get_type(elem[0])==&mp_type_list)
    mp_obj_get_array(elem[0],m,&line);
  if (*m==0){
    c_complex * ptr=(c_complex *)malloc((*n)*sizeof(c_complex));
    *x=ptr;
    for (size_t i=0;i<*n;++i){
      ptr->i=0;
      if (MP_OBJ_IS_SMALL_INT(elem[i])) 
	ptr->r=MP_OBJ_SMALL_INT_VALUE(elem[i]);
      else {
	if (mp_obj_is_float(elem[i]))
	  ptr->r=mp_obj_float_get(elem[i]);
	else {
	  if (mp_obj_get_type(elem[i])==&mp_type_complex){
	    double R,I;
	    mp_obj_get_complex(elem[i],&R,&I);
	    ptr->r=R;
	    ptr->i=I;
	  }
	  else {
	    free(*x);
	    return false;
	  }
	}
      }
      ++ptr;
    }
    return true;
  }
  c_complex * ptr=(c_complex *)malloc((*n)*(*m)*sizeof(c_complex));
  *x=ptr;
  for (size_t i=0;i<*n;++i){
    size_t M;
    mp_obj_get_array(elem[i],&M,&line);
    if (M!=*m){
      free(x);
      return false;
    }
    for (size_t j=0;j<*m;++j){
      ptr->i=0;
      if (MP_OBJ_IS_SMALL_INT(line[j])) 
	ptr->r=MP_OBJ_SMALL_INT_VALUE(line[j]);
      else {
	if (mp_obj_is_float(line[j]))
	  ptr->r=mp_obj_float_get(line[j]);
	else {
	  if (mp_obj_get_type(line[j])==&mp_type_complex){
	    double r,i;
	    mp_obj_get_complex(line[j],&r,&i);
	    ptr->r=r;
	    ptr->i=i;
	  }
	  else {
	    free(*x);
	    return false;
	  }
	}
      }
      ++ptr;
    }
  }
  return true;
}

// print array, 1-d if m==0
// returns malloc-ed string
char * printtab(double * x,size_t n,size_t m){
  char buf[32];
  if (m==0){
    char * ptr=(char *)malloc(32*n);
    strcpy(ptr,"[");
    for (size_t i=0;i<n;++i){
#ifdef NUMWORKS
      buf[0]=0;
      strcat_double(buf,x[i]);
      strcat(buf,",");
#else
      sprintf(buf,"%.14g,",x[i]);
#endif
      strcat(ptr,buf);
    }
    ptr[strlen(ptr)-1]=']';
    return ptr;
  }
  char * ptr=(char *)malloc(32*n*m);
  strcpy(ptr,"[");
  for (size_t i=0;i<n;++i){
    strcat(ptr,"[");
    for (size_t j=0;j<m;++j){
#ifdef NUMWORKS
      buf[0]=0;
      strcat_double(buf,*x);
      strcat(buf,",");
#else
      sprintf(buf,"%.14g,",*x);
#endif
      strcat(ptr,buf);
      ++x;
    }
    ptr[strlen(ptr)-1]=']';
    strcat(ptr,",");
  }
  ptr[strlen(ptr)-1]=']';
  return ptr;
}

char * printc_complextab(c_complex * x,size_t n,size_t m){
  char buf[256]="";
  if (m==0){
    char * ptr=(char *)malloc(64*n);
    strcpy(ptr,"[");
    for (size_t i=0;i<n;++i){
#ifdef NUMWORKS
      c_sprint_double(buf,x[i].r);
      strcat(buf,"+i*");
      strcat_double(buf,x[i].i);
      strcat(buf,",");
#else
      sprintf(buf,"%.14g+i*%.14g,",x[i].r,x[i].i);
#endif
      strcat(ptr,buf);
    }
    ptr[strlen(ptr)-1]=']';
    return ptr;
  }
  char * ptr=(char *)malloc(64*n*m);
  strcpy(ptr,"[");
  for (size_t i=0;i<n;++i){
    strcat(ptr,"[");
    for (size_t j=0;j<m;++j){
#ifdef NUMWORKS
      buf[0]=0;
      c_sprint_double(buf,x->r);
      strcat(buf,"+i*");
      strcat_double(buf,x->i);
      strcat(buf,",");
#else
      sprintf(buf,"%.14g+i*%.14g,",x->r,x->i);
#endif
      strcat(ptr,buf);
      ++x;
    }
    ptr[strlen(ptr)-1]=']';
    strcat(ptr,",");
  }
  ptr[strlen(ptr)-1]=']';
  return ptr;
}

mp_obj_t doubletab2mp_array(double *x,size_t n,size_t m){
  mp_obj_t r=mp_obj_new_list(0,NULL);
  for (size_t i=0;i<n;++i){
    mp_obj_t l=mp_obj_new_list(0,NULL);
    for (size_t j=0;j<m;++j){
      mp_obj_list_append(l,mp_obj_new_float(*x));
      ++x;
    }
    mp_obj_list_append(r,l);
  }
  return r;
}

mp_obj_t c_complextab2mp_array(c_complex *x,size_t n,size_t m){
  mp_obj_t r=mp_obj_new_list(0,NULL);
  if (m==0){
    for (size_t i=0;i<n;++i){
      if (x->i==0)
	mp_obj_list_append(r,mp_obj_new_float(x->r));
      else
	mp_obj_list_append(r,mp_obj_new_complex(x->r,x->i));
      ++x;
    }
    return r;
  }
  for (size_t i=0;i<n;++i){
    mp_obj_t l=mp_obj_new_list(0,NULL);
    for (size_t j=0;j<m;++j){
      if (x->i==0)
	mp_obj_list_append(l,mp_obj_new_float(x->r));
      else
	mp_obj_list_append(l,mp_obj_new_complex(x->r,x->i));
      ++x;
    }
    mp_obj_list_append(r,l);
  }
  return r;
}

const char * caseval(const char *);

// mp_obj_t mp_obj_str_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args);
// type= &mp_type_str, n_args==1, n_kw=0
// mp_obj_is_str
// const char *mp_obj_str_get_str(mp_obj_t self_in); 

static mp_obj_t cas_caseval(size_t n_args, const mp_obj_t *args) {
  const char * text = mp_obj_str_get_str(args[0]);
  if (n_args>1){
    size_t len=strlen(text);
    const char * argtext[8];
    for (int i=1;i<n_args;++i){
      argtext[i]=mp_obj_str_get_str(mp_obj_str_make_new(&mp_type_str,1,0,&args[i]));
      len += strlen(argtext[i]);
    }
    char * buf=malloc(len+64);
    strcpy(buf,text);
    strcat(buf,"(");
    for (int i=1;i<n_args;++i){
      strcat(buf,argtext[i]);
      strcat(buf,",");
    }
    buf[strlen(buf)-1]=')';
    const char * val=caseval(buf);
    py_ck_ctrl_c();
    return mp_obj_new_str(val,strlen(val));
  }
  const char * val=caseval(text);
  py_ck_ctrl_c();
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cas_caseval_obj, 1, 8, cas_caseval);

static mp_obj_t cas_get_key(size_t n_args, const mp_obj_t *args) {
  if (n_args==0){
    unsigned int key; GetKey(&key);
    key=kbd_filter(key);
    return mp_obj_new_int(key);
  }
  int expectkey=-1;
  if (MP_OBJ_IS_INT(args[0]))
    expectkey=mp_obj_get_int(args[0]);
  int c,r;
  short unsigned int key;
  int res = getkeywait(&c,&r,1 /* KEYWAIT_HALTOFF_TIMEROFF*/,0,1,&key);
  if (res==1 /* KEYREP_KEYEVENT*/){
    key=kbd_convert(r,c);
    if (expectkey>=0 && key!=expectkey)
      return mp_obj_new_int(-2);      
    return mp_obj_new_int(key);
  }
  return mp_obj_new_int(-1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cas_get_key_obj, 0, 1, cas_get_key);

//
static const mp_map_elem_t cas_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_caseval), (mp_obj_t) &cas_caseval_obj },
	{ MP_ROM_QSTR(MP_QSTR_get_key), (mp_obj_t) &cas_get_key_obj },
	{ MP_ROM_QSTR(MP_QSTR_getkey), (mp_obj_t) &cas_get_key_obj },
	{ MP_ROM_QSTR(MP_QSTR_xcas), (mp_obj_t) &cas_caseval_obj },
	{ MP_ROM_QSTR(MP_QSTR_eval_expr), (mp_obj_t) &cas_caseval_obj },
};


static MP_DEFINE_CONST_DICT(cas_locals_dict, cas_locals_dict_table);

const mp_obj_type_t cas_type = {
    { &mp_type_type },
    .name = MP_QSTR_cas,
    .locals_dict = (mp_obj_t)&cas_locals_dict
};


STATIC const mp_map_elem_t mp_module_cas_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__cas) },
    { MP_ROM_QSTR(MP_QSTR_caseval), (mp_obj_t) &cas_caseval_obj },
    { MP_ROM_QSTR(MP_QSTR_get_key), (mp_obj_t) &cas_get_key_obj },
    { MP_ROM_QSTR(MP_QSTR_getkey), (mp_obj_t) &cas_get_key_obj },
    { MP_ROM_QSTR(MP_QSTR_xcas), (mp_obj_t) &cas_caseval_obj },
    { MP_ROM_QSTR(MP_QSTR_eval_expr), (mp_obj_t) &cas_caseval_obj },
};

STATIC const mp_obj_dict_t mp_module_cas_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_cas_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_cas_globals_table),
        .table = (mp_map_elem_t*)mp_module_cas_globals_table,
    },
};

const mp_obj_module_t mp_module_cas = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_cas_globals,
};

/* ARIT 
   mp_obj_t mp_obj_new_int_from_str_len(const char **str, size_t len, bool neg, unsigned int base);
 */

mp_obj_t str2int(const char * s){
  bool neg=s[0]=='-';
  if (neg)
    ++s;
  return mp_obj_new_int_from_str_len(&s,strlen(s),neg,10);
}

static mp_obj_t str2list2int(const char * val,int N){
  char buf[strlen(val)+1];
  strcpy(buf,val);
  char * buf1=buf;
  mp_obj_t res = mp_obj_new_list(0, NULL);
  for (;*buf1;++buf1){
    if (*buf1=='[')
      break;
  }
  if (*buf1){
    ++buf1;
    for (int i=0;i<N;++i){
      char * buf2=buf1;
      for (;*buf2;++buf2){
	if (*buf2==',' || *buf2==']')
	  break;
      }
      *buf2=0;
      mp_obj_list_append(res,str2int(buf1));
      buf1=buf2+1;
      while (*buf1==' ')
	++buf1;
    }
  }
  return res;
}

mp_obj_t mul(mp_obj_t a,mp_obj_t b){
  return mp_binary_op(MP_BINARY_OP_MULTIPLY,a,b);
}

mp_obj_t quo(mp_obj_t a,mp_obj_t b){
  return mp_binary_op(MP_BINARY_OP_FLOOR_DIVIDE,a,b);
}

mp_obj_t rem(mp_obj_t a,mp_obj_t b){
  return mp_binary_op(MP_BINARY_OP_MODULO,a,b);
}

mp_obj_t sub(mp_obj_t a,mp_obj_t b){
  return mp_binary_op(MP_BINARY_OP_SUBTRACT,a,b);
}

mp_obj_t add(mp_obj_t a,mp_obj_t b){
  return mp_binary_op(MP_BINARY_OP_ADD,a,b);
}

mp_obj_t powmod(mp_obj_t a,mp_obj_t n,mp_obj_t r){
  return mp_obj_int_pow3(a,n,r);
}

int is_zero(const mp_obj_t a){
  if (mp_obj_is_small_int(a))
    return mp_obj_get_int(a)==0;
  const mpz_t *z= &((mp_obj_int_t*)MP_OBJ_TO_PTR(a))->mpz;
  return z->len==0;
}

int is_one(const mp_obj_t a){
  if (mp_obj_is_small_int(a))
    return mp_obj_get_int(a)==1;
  return is_zero(sub(a,mp_obj_new_int(1)));
}

int is_minus_one(const mp_obj_t a){
  if (mp_obj_is_small_int(a))
    return mp_obj_get_int(a)==-1;
  return is_zero(add(a,mp_obj_new_int(1)));
}

mp_obj_t iegcd(const mp_obj_t a_,const mp_obj_t b_){
  mp_obj_t a0=a_,a1=b_,a2,u0=mp_obj_new_int(1),u1=mp_obj_new_int(0),u2,v0=mp_obj_new_int(0),v1=mp_obj_new_int(1),v2;
  while (!is_zero(a1)){
    mp_obj_t q=quo(a0,a1);
    a2=sub(a0,mul(q,a1));
    u2=sub(u0,mul(q,u1));
    v2=sub(v0,mul(q,v1));
    a0=a1; a1=a2; u0=u1; u1=u2;  v0=v1; v1=v2;
  }
  mp_obj_t res = mp_obj_new_list(0, NULL);
  mp_obj_list_append(res,u0);
  mp_obj_list_append(res,v0);
  mp_obj_list_append(res,a0);
  return res;
}

mp_obj_t gcd(const mp_obj_t a_,const mp_obj_t b_){
  mp_obj_t a=a_,b=b_;
  while (!is_zero(b)){
    mp_obj_t r=rem(a,b);
    a=b;
    b=r;
  }
  return a;
#if 0
  while (!mp_obj_is_small_int(b)){
    const mpz_t *z= &((mp_obj_int_t*)MP_OBJ_TO_PTR(b))->mpz;
    if (z->len<=1) break;
    mp_obj_t r=rem(a,b);
    a=b;
    b=r;
  }
  long long B=mp_obj_get_int(b);
  if (B==0) return a;
  mp_obj_t r=rem(a,b);
  long long A=mp_obj_get_int(r);
  while (B!=0){
    long long R=A%B;
    A=B;
    B=R;
  }
  if (A<0) A=-A;
  return mp_obj_new_int(A);
#endif
}

  const short int primes[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999,3001,3011,3019,3023,3037,3041,3049,3061,3067,3079,3083,3089,3109,3119,3121,3137,3163,3167,3169,3181,3187,3191,3203,3209,3217,3221,3229,3251,3253,3257,3259,3271,3299,3301,3307,3313,3319,3323,3329,3331,3343,3347,3359,3361,3371,3373,3389,3391,3407,3413,3433,3449,3457,3461,3463,3467,3469,3491,3499,3511,3517,3527,3529,3533,3539,3541,3547,3557,3559,3571,3581,3583,3593,3607,3613,3617,3623,3631,3637,3643,3659,3671,3673,3677,3691,3697,3701,3709,3719,3727,3733,3739,3761,3767,3769,3779,3793,3797,3803,3821,3823,3833,3847,3851,3853,3863,3877,3881,3889,3907,3911,3917,3919,3923,3929,3931,3943,3947,3967,3989,4001,4003,4007,4013,4019,4021,4027,4049,4051,4057,4073,4079,4091,4093,4099,4111,4127,4129,4133,4139,4153,4157,4159,4177,4201,4211,4217,4219,4229,4231,4241,4243,4253,4259,4261,4271,4273,4283,4289,4297,4327,4337,4339,4349,4357,4363,4373,4391,4397,4409,4421,4423,4441,4447,4451,4457,4463,4481,4483,4493,4507,4513,4517,4519,4523,4547,4549,4561,4567,4583,4591,4597,4603,4621,4637,4639,4643,4649,4651,4657,4663,4673,4679,4691,4703,4721,4723,4729,4733,4751,4759,4783,4787,4789,4793,4799,4801,4813,4817,4831,4861,4871,4877,4889,4903,4909,4919,4931,4933,4937,4943,4951,4957,4967,4969,4973,4987,4993,4999,5003,5009,5011,5021,5023,5039,5051,5059,5077,5081,5087,5099,5101,5107,5113,5119,5147,5153,5167,5171,5179,5189,5197,5209,5227,5231,5233,5237,5261,5273,5279,5281,5297,5303,5309,5323,5333,5347,5351,5381,5387,5393,5399,5407,5413,5417,5419,5431,5437,5441,5443,5449,5471,5477,5479,5483,5501,5503,5507,5519,5521,5527,5531,5557,5563,5569,5573,5581,5591,5623,5639,5641,5647,5651,5653,5657,5659,5669,5683,5689,5693,5701,5711,5717,5737,5741,5743,5749,5779,5783,5791,5801,5807,5813,5821,5827,5839,5843,5849,5851,5857,5861,5867,5869,5879,5881,5897,5903,5923,5927,5939,5953,5981,5987,6007,6011,6029,6037,6043,6047,6053,6067,6073,6079,6089,6091,6101,6113,6121,6131,6133,6143,6151,6163,6173,6197,6199,6203,6211,6217,6221,6229,6247,6257,6263,6269,6271,6277,6287,6299,6301,6311,6317,6323,6329,6337,6343,6353,6359,6361,6367,6373,6379,6389,6397,6421,6427,6449,6451,6469,6473,6481,6491,6521,6529,6547,6551,6553,6563,6569,6571,6577,6581,6599,6607,6619,6637,6653,6659,6661,6673,6679,6689,6691,6701,6703,6709,6719,6733,6737,6761,6763,6779,6781,6791,6793,6803,6823,6827,6829,6833,6841,6857,6863,6869,6871,6883,6899,6907,6911,6917,6947,6949,6959,6961,6967,6971,6977,6983,6991,6997,7001,7013,7019,7027,7039,7043,7057,7069,7079,7103,7109,7121,7127,7129,7151,7159,7177,7187,7193,7207,7211,7213,7219,7229,7237,7243,7247,7253,7283,7297,7307,7309,7321,7331,7333,7349,7351,7369,7393,7411,7417,7433,7451,7457,7459,7477,7481,7487,7489,7499,7507,7517,7523,7529,7537,7541,7547,7549,7559,7561,7573,7577,7583,7589,7591,7603,7607,7621,7639,7643,7649,7669,7673,7681,7687,7691,7699,7703,7717,7723,7727,7741,7753,7757,7759,7789,7793,7817,7823,7829,7841,7853,7867,7873,7877,7879,7883,7901,7907,7919,7927,7933,7937,7949,7951,7963,7993,8009,8011,8017,8039,8053,8059,8069,8081,8087,8089,8093,8101,8111,8117,8123,8147,8161,8167,8171,8179,8191,8209,8219,8221,8231,8233,8237,8243,8263,8269,8273,8287,8291,8293,8297,8311,8317,8329,8353,8363,8369,8377,8387,8389,8419,8423,8429,8431,8443,8447,8461,8467,8501,8513,8521,8527,8537,8539,8543,8563,8573,8581,8597,8599,8609,8623,8627,8629,8641,8647,8663,8669,8677,8681,8689,8693,8699,8707,8713,8719,8731,8737,8741,8747,8753,8761,8779,8783,8803,8807,8819,8821,8831,8837,8839,8849,8861,8863,8867,8887,8893,8923,8929,8933,8941,8951,8963,8969,8971,8999,9001,9007,9011,9013,9029,9041,9043,9049,9059,9067,9091,9103,9109,9127,9133,9137,9151,9157,9161,9173,9181,9187,9199,9203,9209,9221,9227,9239,9241,9257,9277,9281,9283,9293,9311,9319,9323,9337,9341,9343,9349,9371,9377,9391,9397,9403,9413,9419,9421,9431,9433,9437,9439,9461,9463,9467,9473,9479,9491,9497,9511,9521,9533,9539,9547,9551,9587,9601,9613,9619,9623,9629,9631,9643,9649,9661,9677,9679,9689,9697,9719,9721,9733,9739,9743,9749,9767,9769,9781,9787,9791,9803,9811,9817,9829,9833,9839,9851,9857,9859,9871,9883,9887,9901,9907,9923,9929,9931,9941,9949,9967,9973};

// perform N Miller-Rabin tests
int mrtest(int N,const mp_obj_t p){
  if (is_zero(p) || is_one(p))
    return 0;
  if (N>sizeof(primes)/sizeof(unsigned short))
    return -1;
  // p-1==2^t*s
  mp_obj_t un=mp_obj_new_int(1),deux=mp_obj_new_int(2);
  mp_obj_t pm1=sub(p,un);
  int t=1;
  mp_obj_t s=quo(pm1,deux);
  while (is_zero(rem(s,deux))){
    ++t;
    s=quo(s,deux);
  }
  for (int i=0;i<N;++i){
    mp_obj_t a=mp_obj_new_int(primes[i]);
    if (is_zero(sub(a,p))) // a is prime, hence p is
      return 2;
    a=powmod(a,s,p);
    if (is_one(a))
      continue; // success
    int j=0;
    for (;j<t;++j){
      //int A=mp_obj_get_int(a); // debug
      if (is_zero(sub(add(a,un),p)))
	break;
      a=mul(a,a);
      a=rem(a,p);
    }
    if (j==t)
      return 0; // failure
  }
  return 1; // all N tests passed
}

mp_obj_t pollard_f(const mp_obj_t x,const mp_obj_t n,const mp_obj_t k){
  mp_obj_t res=mul(x,x);
  res=add(res,k);
  return rem(res,n);
}

// inner Pollard-rho for composite n with at most N attempts
// iterate x->x^2+k mod n
mp_obj_t pollard_t(const mp_obj_t n,const mp_obj_t k,int N){
  mp_obj_t x=mp_obj_new_int(2),x2=x;
  for (int i=0;i<N;++i){
    x=pollard_f(x,n,k);
    x2=pollard_f(x2,n,k);
    x2=pollard_f(x2,n,k);
    mp_obj_t y=sub(x,x2);
    y=gcd(y,n);
    if (is_one(y) || is_minus_one(y))
      continue;
    if (is_zero(sub(y,n)) || is_zero(add(y,n)))
      return mp_obj_new_int(0);
    return y;
  }
  return mp_obj_new_int(0);
}

// Pollard-rho on composite n with at most N attempts, returns -1 on failure
mp_obj_t pollard(const mp_obj_t n,int N){
  int k=1;
  for (;k<50;){
    mp_obj_t res=pollard_t(n,mp_obj_new_int(k),N);
    if (!is_zero(res))
      return res;
    // res==-1, error
    if (k==1){ k=-1; continue; }
    if (k*k==1){ k=3; continue; }
    k+=2;
  }
  return mp_obj_new_int(0);
}

long long ifactor(long long a,int * tab){
  if (a<0){
    a=-a;
    *tab=-1;
    ++tab;
  }
  if (mrtest(20,mp_obj_new_int(a))==1)
    return a;
  int p=0;
  for (int i=0;i<sizeof(primes)/sizeof(short);++i){
    p=primes[i];
    if ( ((long long)p)*p > a)
      break;
    while (a%p==0){
      *tab=p;
      ++tab;
      a/=p;
    }
  }
  for (;;){
    if ( ((long long)p)*p>a || mrtest(20,mp_obj_new_int(a))==1)
      return a;
    // a is still composite, find a factor with Pollard rho
    mp_obj_t P=pollard(mp_obj_new_int(a),3000);
    if (is_zero(P)) // failure
      return 0;
    p=mp_obj_get_int(P);
    if (a%p!=0)
      return 0;
    *tab=p; ++tab; a/=p;
    while (a%p==0){
      *tab=p;
      ++tab;
      a/=p;
    }
  }
  return 0;
}


static mp_obj_t arit(const mp_obj_t *args,int nargs,int cmd) {
  if (
      !MP_OBJ_IS_INT(args[0])
      || (nargs==2 && !MP_OBJ_IS_INT(args[1]))
      )
    mp_raise_TypeError("integer expected");
  if (cmd==8 && mp_obj_get_int(args[0])>2e5)
    mp_raise_TypeError("integer too large");
#if 1 // use this if Xcas is not compiled with
  if (cmd==0){ // isprime
    return mp_obj_new_int(mrtest(20,args[0]));
  }
  if (cmd==1){ // nextprime
    mp_obj_t a=args[0],un=mp_obj_new_int(1),deux=mp_obj_new_int(2);
    a=add(a,un);
    if (is_zero(rem(a,deux)))
      a=add(a,un);
    while (mrtest(20,a)==0){
      a=add(a,deux);
    }
    return a;
  }
  if (cmd==2){ // prevprime
    mp_obj_t a=args[0],un=mp_obj_new_int(1),deux=mp_obj_new_int(2);
    a=sub(a,un);
    if (is_zero(rem(a,deux)))
      a=sub(a,un);
    while (!is_zero(a) && mrtest(20,a)==0){
      a=sub(a,deux);
    }
    return a;
  }
  if (cmd==3){ // ifactor
    mp_obj_t A=args[0];
    if (!MP_OBJ_IS_SMALL_INT(A))
      mp_raise_TypeError("integer too large");
    long long a=mp_obj_get_int(A);
    int fact[68]={0}; // factor list size can not exceed 64, including -1
    a=ifactor(a,fact);
    mp_obj_t res = mp_obj_new_list(0, NULL);
    int * ptr=fact;
    for (;*ptr;++ptr){
      mp_obj_list_append(res,mp_obj_new_int(*ptr));
    }
    if (a!=1)
      mp_obj_list_append(res,mp_obj_new_int(a));
    return res;
  }
  if (cmd==4){ // gcd
    mp_obj_t a=args[0],b=args[1];
    return gcd(a,b);
  }
  if (cmd==5){ // lcm
    mp_obj_t a=args[0],b=args[1];
    mp_obj_t ab=mul(a,b);
    return quo(ab,gcd(a,b));
  }
  if (cmd==6){ // iegcd
    mp_obj_t a=args[0],b=args[1];
    return iegcd(a,b);
  }
  if (cmd==7){ // euler
    mp_obj_t A=args[0];
    if (!MP_OBJ_IS_SMALL_INT(A))
      mp_raise_TypeError("integer too large");
    long long a=mp_obj_get_int(A);
    int fact[68]={0}; // factor list size can not exceed 64, including -1
    long long b=ifactor(a,fact);
    int * ptr=fact;
    if (*ptr==-1) // skip sign
      ++ptr;
    if (b!=1){ // take care of remaining large prime factor
      a /= b;
      a *= b-1;
    }
    // take care of small prime factors
    while (*ptr){
      int cur=*ptr;
      while (*ptr==cur)
	++ptr;
      a /= cur;
      a *= (cur-1);
    }
    return mp_obj_new_int(a);
  }
  if (cmd==8)
    mp_raise_TypeError("Not implemented");
#endif
  // The size of this buffer is rather arbitrary. If it's not large
  // enough, a dynamic one will be allocated.
  char stack_buf[512];
  char *buf = stack_buf;
  size_t buf_size = sizeof(stack_buf);
  size_t fmt_size;
  char *str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, args[0], 10, NULL, '\0', '\0');
  char BUF[strlen(str)+32];
  switch (cmd){
  case 0:
    sprintf(BUF,"isprime(%s)",str);
    break;
  case 1:
    sprintf(BUF,"nextprime(%s)",str);
    break;
  case 2:
    sprintf(BUF,"prevprime(%s)",str);
    break;
  case 3:
    sprintf(BUF,"ifactor(%s)",str);
    break;
  case 4:
    sprintf(BUF,"gcd(%s,",str);
    break;
  case 5:
    sprintf(BUF,"lcm(%s,",str);
    break;
  case 6:
    sprintf(BUF,"iegcd(%s,",str);
    break;
  case 7:
    sprintf(BUF,"euler(%s)",str);
    break;
  case 8:
    sprintf(BUF,"nprimes(%s)",str);
    break;
  }
  if (buf != stack_buf) {
    m_del(char, buf, buf_size);
  }
  const char * val=0;
  if (nargs==2){
    buf=stack_buf;
    str = mp_obj_int_formatted(&buf, &buf_size, &fmt_size, args[1], 10, NULL, '\0', '\0');
    char BUF2[strlen(BUF)+strlen(str)+32];
    strcpy(BUF2,BUF);
    strcat(BUF2,str);
    if (buf != stack_buf) {
      m_del(char, buf, buf_size);
    }
    strcat(BUF2,")");
    val=caseval(BUF2);
  }
  else
    val=caseval(BUF);
  if (cmd==0){
    if (strcmp(val,"True")==0 || strcmp(val,"true")==0 || strcmp(val,"1")==0)
      return mp_const_true;
    if (strcmp(val,"False")==0 || strcmp(val,"false")==0 || strcmp(val,"0")==0)
      return mp_const_false;
  }
  if (cmd==1 || cmd==2 || cmd==4 || cmd==5  || cmd==7 || cmd==8){
    return str2int(val);
  }
  if (cmd==6){
    return str2list2int(val,3);
  }
  return mp_obj_new_str(val,strlen(val));
}
static mp_obj_t arit_isprime(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,0);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_isprime_obj, 1, 1, arit_isprime);

static mp_obj_t arit_nextprime(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_nextprime_obj, 1, 1, arit_nextprime);

static mp_obj_t arit_prevprime(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,2);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_prevprime_obj, 1, 1, arit_prevprime);

static mp_obj_t arit_ifactor(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,3);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_ifactor_obj, 1, 1, arit_ifactor);

static mp_obj_t arit_gcd(size_t n_args, const mp_obj_t *args) {
  return arit(args,2,4);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_gcd_obj, 2, 2, arit_gcd);

static mp_obj_t arit_lcm(size_t n_args, const mp_obj_t *args) {
  return arit(args,2,5);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_lcm_obj, 2, 2, arit_lcm);

static mp_obj_t arit_iegcd(size_t n_args, const mp_obj_t *args) {
  return arit(args,2,6);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_iegcd_obj, 2, 2, arit_iegcd);

static mp_obj_t arit_euler(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,7);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_euler_obj, 1, 1, arit_euler);

static mp_obj_t arit_nprimes(size_t n_args, const mp_obj_t *args) {
  return arit(args,1,8);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_nprimes_obj, 1, 1, arit_nprimes);

static mp_obj_t arit_char(size_t n_args, const mp_obj_t *args) {
  if (MP_OBJ_IS_SMALL_INT(args[0])){
    char buf[2]={MP_OBJ_SMALL_INT_VALUE(args[0]),0};
    return mp_obj_new_str(buf,1);
  }
  mp_obj_t * tab; size_t m=0;
  mp_obj_get_array(args[0],&m,&tab);
  if (m==0)
    raisetypeerr();
  char buf[m+1];
  for (int i=0;i<m;++i){
    if (!MP_OBJ_IS_SMALL_INT(tab[i]))
      raisetypeerr();
    buf[i]=MP_OBJ_SMALL_INT_VALUE(tab[i]);
  }
  return mp_obj_new_str(buf,m);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_char_obj, 1, 1, arit_char);

static mp_obj_t arit_asc(size_t n_args, const mp_obj_t *args) {
  if (!mp_obj_is_str(args[0]))
    raisetypeerr();
  size_t len;
  const char * s=mp_obj_str_get_data(args[0],&len);
  mp_obj_t res = mp_obj_new_list(0, NULL);
  for (int i=0;i<len;++i)
    mp_obj_list_append(res,mp_obj_new_int(s[i]));
  return res;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_asc_obj, 1, 1, arit_asc);

static mp_obj_t arit_powmod(size_t n_args, const mp_obj_t *args) {
  mp_obj_t e=args[0],n=args[1],p=args[2];
  if (!MP_OBJ_IS_INT(n) || !MP_OBJ_IS_INT(p))
    raisetypeerr();
  if (mp_obj_get_type(e)==&mp_type_list){
    size_t len;
    mp_obj_t * elem;
    mp_obj_get_array(e, &len, &elem);
    mp_obj_t res = mp_obj_new_list(0, NULL);
    for (int i=0;i<len;++i){
      if (!MP_OBJ_IS_INT(elem[i]))
	raisetypeerr();
      mp_obj_list_append(res,powmod(elem[i],n,p));
    }
    return res;
  }
  if (!MP_OBJ_IS_INT(e))
    raisetypeerr();
  return powmod(e,n,p);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(arit_powmod_obj, 3, 3, arit_powmod);
//
static const mp_map_elem_t arit_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_isprime), (mp_obj_t) &arit_isprime_obj },
	{ MP_ROM_QSTR(MP_QSTR_nextprime), (mp_obj_t) &arit_nextprime_obj },
	{ MP_ROM_QSTR(MP_QSTR_prevprime), (mp_obj_t) &arit_prevprime_obj },
	{ MP_ROM_QSTR(MP_QSTR_ifactor), (mp_obj_t) &arit_ifactor_obj },
	{ MP_ROM_QSTR(MP_QSTR_gcd), (mp_obj_t) &arit_gcd_obj },
	{ MP_ROM_QSTR(MP_QSTR_lcm), (mp_obj_t) &arit_lcm_obj },
	{ MP_ROM_QSTR(MP_QSTR_iegcd), (mp_obj_t) &arit_iegcd_obj },
	{ MP_ROM_QSTR(MP_QSTR_char), (mp_obj_t) &arit_char_obj },
	{ MP_ROM_QSTR(MP_QSTR_asc), (mp_obj_t) &arit_asc_obj },
	{ MP_ROM_QSTR(MP_QSTR_powmod), (mp_obj_t) &arit_powmod_obj },
        { MP_ROM_QSTR(MP_QSTR_euler), (mp_obj_t) &arit_euler_obj },
        { MP_ROM_QSTR(MP_QSTR_nprimes), (mp_obj_t) &arit_nprimes_obj },
};


static MP_DEFINE_CONST_DICT(arit_locals_dict, arit_locals_dict_table);

const mp_obj_type_t arit_type = {
    { &mp_type_type },
    .name = MP_QSTR_arit,
    .locals_dict = (mp_obj_t)&arit_locals_dict
};


STATIC const mp_map_elem_t mp_module_arit_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__arit) },
    { MP_ROM_QSTR(MP_QSTR_isprime), (mp_obj_t) &arit_isprime_obj },
    { MP_ROM_QSTR(MP_QSTR_nextprime), (mp_obj_t) &arit_nextprime_obj },
    { MP_ROM_QSTR(MP_QSTR_prevprime), (mp_obj_t) &arit_prevprime_obj },
    { MP_ROM_QSTR(MP_QSTR_ifactor), (mp_obj_t) &arit_ifactor_obj },
    { MP_ROM_QSTR(MP_QSTR_gcd), (mp_obj_t) &arit_gcd_obj },
    { MP_ROM_QSTR(MP_QSTR_lcm), (mp_obj_t) &arit_lcm_obj },
    { MP_ROM_QSTR(MP_QSTR_iegcd), (mp_obj_t) &arit_iegcd_obj },
    { MP_ROM_QSTR(MP_QSTR_char), (mp_obj_t) &arit_char_obj },
    { MP_ROM_QSTR(MP_QSTR_asc), (mp_obj_t) &arit_asc_obj },
    { MP_ROM_QSTR(MP_QSTR_powmod), (mp_obj_t) &arit_powmod_obj },
    { MP_ROM_QSTR(MP_QSTR_euler), (mp_obj_t) &arit_euler_obj },
    { MP_ROM_QSTR(MP_QSTR_nprimes), (mp_obj_t) &arit_nprimes_obj },
};

STATIC const mp_obj_dict_t mp_module_arit_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_arit_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_arit_globals_table),
        .table = (mp_map_elem_t*)mp_module_arit_globals_table,
    },
};

const mp_obj_module_t mp_module_arit = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_arit_globals,
};

/* LINALG (with some NUMPY compatibility) */
static mp_obj_t linalg_matrix(size_t n_args, const mp_obj_t *args) {
  double *x=0;size_t n,m;
  if (mp_array2doubletab(args[n_args-1],&x,&n,&m)){
    if (n_args==1){
      free(x);
      return args[0];
    }
    if (n_args==3 && MP_OBJ_IS_SMALL_INT(args[0]) &&  MP_OBJ_IS_SMALL_INT(args[1])){
      int n1=MP_OBJ_SMALL_INT_VALUE(args[0]),m1=MP_OBJ_SMALL_INT_VALUE(args[1]);
      int N=m?n*m:n,k=0;
      mp_obj_t r = mp_obj_new_list(0, NULL);
      for (int i=0;i<n1;++i){
	mp_obj_t l = mp_obj_new_list(0, NULL);
	for (int j=0;j<m1;++j){
	  mp_obj_list_append(l,mp_obj_new_float(k<N?x[k]:0));
	  ++k;
	}
	mp_obj_list_append(r,l);
      }
      free(x);
      return r;
    }
    if (n_args==2 && MP_OBJ_IS_SMALL_INT(args[0]) ){
      int n1=MP_OBJ_SMALL_INT_VALUE(args[0]);
      int N=m?n*m:n,k=0;
      mp_obj_t r = mp_obj_new_list(0, NULL);
      for (int i=0;i<n1;++i){
	mp_obj_list_append(r,mp_obj_new_float(k<N?x[k]:0));
	++k;
      }
      free(x);
      return r;
    }
    free(x);
  }
  if (n_args==2 && MP_OBJ_IS_SMALL_INT(args[0])){
    int n1=MP_OBJ_SMALL_INT_VALUE(args[0]);
    mp_obj_t r = mp_obj_new_list(0, NULL);
    for (int i=0;i<n1;++i){
      mp_obj_t arg=mp_obj_new_int(i);
      if (mp_obj_get_type(args[1])==&mp_type_fun_bc)
	mp_obj_list_append(r,fun_bc_call(args[1],2,0,arg));
      else
	mp_obj_list_append(r,args[1]);	  
    }
    return r;
  }
  if (n_args==3 && MP_OBJ_IS_SMALL_INT(args[0]) && MP_OBJ_IS_SMALL_INT(args[1])){
    int n1=MP_OBJ_SMALL_INT_VALUE(args[0]);
    int m1=MP_OBJ_SMALL_INT_VALUE(args[1]);
    mp_obj_t r = mp_obj_new_list(0, NULL);
    mp_obj_t arg[2];
    for (int i=0;i<n1;++i){
      arg[0]=mp_obj_new_int(i);
      mp_obj_t l = mp_obj_new_list(0, NULL);
      for (int j=0;j<m1;++j){
	arg[1]=mp_obj_new_int(j);
	if (mp_obj_get_type(args[2])==&mp_type_fun_bc)
	  mp_obj_list_append(l,fun_bc_call(args[2],2,0,arg));
	else
	  mp_obj_list_append(l,args[2]);
      }
      mp_obj_list_append(r,l);
    }
    return r;
  }
  const char val[]="Bad argument value";
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_matrix_obj, 1, 3, linalg_matrix);

static mp_obj_t linalg_arange(size_t n_args, const mp_obj_t *args) {
  mp_obj_t l = mp_obj_new_list(0, NULL);
  double step=1,a,b;
  if (n_args==3)
    mp_int_float(args[2],&step);
  if (step>0 && mp_int_float(args[0],&a) && mp_int_float(args[1],&b)){
    for (;a<=b;a+=step){
      mp_obj_list_append(l,mp_obj_new_float(a));
    }
    return l;
  }
  const char val[]="Bad argument value";
  return mp_obj_new_str(val,strlen(val));  
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_arange_obj, 2, 3, linalg_arange);

static mp_obj_t linalg_linspace(size_t n_args, const mp_obj_t *args) {
  mp_obj_t l = mp_obj_new_list(0, NULL);
  double a,b;
  if (mp_int_float(args[0],&a) && mp_int_float(args[1],&b) && a!=b && MP_OBJ_IS_SMALL_INT(args[2]) && MP_OBJ_SMALL_INT_VALUE(args[2])>1){
    double step=(b-a)/(MP_OBJ_SMALL_INT_VALUE(args[2])-1);
    if (step*(b-a)<0)
      step=-step;
    for (;a<=b;a+=step){
      mp_obj_list_append(l,mp_obj_new_float(a));
    }
    return l;
  }
  const char val[]="Bad argument value";
  return mp_obj_new_str(val,strlen(val));  
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_linspace_obj, 3, 3, linalg_linspace);

void raisedimerr(){
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Dimension mismatch"));
}

mp_obj_t mp_obj_new_complex_real(double a,double b){
  if (b==0)
    return mp_obj_new_float(a);
  return mp_obj_new_complex(a,b);
}

static mp_obj_t linalg_addsub(size_t n_args, const mp_obj_t *args,bool add) {
  c_complex *x,*y;
  size_t n1,m1,n2,m2;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (!mp_array2c_complextab(args[1],&y,&n2,&m2)){
      free(x);
      nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 2nd argument."));
    }
    if (n1!=n2 || m1!=m2){
      free(x); free(y);
      raisedimerr();
    }
    c_complex *x_=x,*y_=y;
    mp_obj_t res = mp_obj_new_list(0, NULL);
    for (int i=0;i<n1;++i){
      if (m1==0){
	if (add)
	  mp_obj_list_append(res,mp_obj_new_complex_real(x->r+y->r,x->i+y->i));
	else
	  mp_obj_list_append(res,mp_obj_new_complex_real(x->r-y->r,x->i-y->i));
	++x; ++y;
	continue;
      }
      mp_obj_t line = mp_obj_new_list(0, NULL);
      if (add){
	for (int j=0;j<m1;++j,++x,++y){
	  mp_obj_list_append(line,mp_obj_new_complex_real(x->r+y->r,x->i+y->i));
	}
      }
      else {
	for (int j=0;j<m1;++j,++x,++y){
	  mp_obj_list_append(line,mp_obj_new_complex_real(x->r-y->r,x->i-y->i));
	}
      }
      mp_obj_list_append(res,line);
    }
    free(x_); free(y_);
    return res;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
static mp_obj_t linalg_add(size_t n_args, const mp_obj_t *args) {
  return linalg_addsub(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_add_obj, 2, 2, linalg_add);
static mp_obj_t linalg_sub(size_t n_args, const mp_obj_t *args) {
  return linalg_addsub(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_sub_obj, 2, 2, linalg_sub);

// scalar product of vectors x and y
c_complex dot(c_complex *x,c_complex *y,int n){
  c_complex r={0,0};
  for (int i=0;i<n;++i){
    r.r += x[i].r*y[i].r-x[i].i*y[i].i;
    r.i += x[i].r*y[i].i+x[i].i*y[i].r;
  }
  return r;
}

c_complex mult(c_complex a,c_complex b){
  c_complex c={a.r*b.r-a.i*b.i,a.r*b.i+a.i*b.r};
  return c;
}

// transpose matrix (in place for square matrices)
void transpose(c_complex *x,int n,int m){
  if (n==m){
    for (int i=1;i<n;++i){
      for (int j=0;j<i;++j){
	c_complex * a=x+i*m+j;
	c_complex * b=x+j*m+i;
	c_complex t=*a;
	*a=*b;
	*b=t;
      }
    }
    return;
  }
  c_complex * tmp=(c_complex *) malloc(n*m*sizeof(c_complex));
  if (!tmp)
    return; // should error!
  for (int i=0;i<n;++i){
    for (int j=0;j<m;++j){
      *(tmp+j*n+i)=*(x+i*m+j);
    }
  }
  memcpy(x,tmp,n*m*sizeof(c_complex));
  free(tmp);
}


static mp_obj_t linalg_dot(size_t n_args, const mp_obj_t *args) {
  c_complex *x,*y; c_complex a;
  size_t n1,m1,n2,m2;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (mp_get_c_complex(args[1],&a)){
      mp_obj_t res = mp_obj_new_list(0, NULL);
      c_complex * x_=x;
      if (m1==0){
	for (;x_<x+n1;++x_){
	  c_complex c=mult(*x_,a);
	  mp_obj_list_append(res,mp_obj_new_complex_real(c.r,c.i));
	}
	free(x);
	return res;
      }
      for (int i=0;i<n1;++i){
	mp_obj_t line = mp_obj_new_list(0, NULL);
	for (int j=0;j<m1;++j,++x_){
	  c_complex c=mult(*x_,a);
	  mp_obj_list_append(line,mp_obj_new_complex_real(c.r,c.i));
	}
	mp_obj_list_append(res,line);
      }
      free(x);
      return res;      
    }
    if (!mp_array2c_complextab(args[1],&y,&n2,&m2)){
      free(x);
      nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 2nd argument."));
    }
    if (m1==0){ // v*M
      if (m2==0 && n1==n2){
	c_complex c={0,0};
	c_complex * x_=x,*y_=y;
	for (int i=0;i<n1;++i,++x_,++y_){
	  c.r += x_->r*y_->r-x_->i*y_->i;
	  c.i += x_->r*y_->i+x_->i*y_->r;
	}
	free(x);free(y);
	return mp_obj_new_complex_real(c.r,c.i);
      }
      if (n1!=m2)
	raisedimerr();
      mp_obj_t res = mp_obj_new_list(0, NULL);
      for (c_complex * y_=y;y_<y+n2*m2;y_+=m2){
	c_complex c=dot(x,y_,m2);
	mp_obj_list_append(res,mp_obj_new_complex_real(c.r,c.i));
      }
      free(x);free(y);
      return res;
    }
    if (m2==0){ // M*v
      if (n2!=m1)
	raisedimerr();
      mp_obj_t res = mp_obj_new_list(0, NULL);
      for (c_complex * x_=x;x_<x+n1*m1;x_+=m1){
	c_complex c=dot(x_,y,m1);
	mp_obj_list_append(res,mp_obj_new_complex_real(c.r,c.i));
      }
      free(x);free(y);
      return res;
    }
    if (m1!=n2){
      free(x); free(y);
      raisedimerr();
    }
    // M1*M2
    transpose(y,n2,m2);
    mp_obj_t res = mp_obj_new_list(0, NULL);
    for (int i=0;i<n1;++i){
      mp_obj_t line = mp_obj_new_list(0, NULL);
      for (int j=0;j<m2;++j){
	c_complex c=dot(x+m1*i,y+m1*j,m1);
	mp_obj_list_append(line,mp_obj_new_complex_real(c.r,c.i));
      }
      mp_obj_list_append(res,line);
    }
    free(x); free(y);
    return res;
  }
  if (mp_array2c_complextab(args[1],&y,&n2,&m2)){
    if (mp_get_c_complex(args[0],&a)){
      mp_obj_t res = mp_obj_new_list(0, NULL);
      c_complex * x_=y;
      if (m2==0){
	for (;x_<x+n2;++x_){
	  c_complex c=mult(*x_,a);
	  mp_obj_list_append(res,mp_obj_new_complex_real(c.r,c.i));
	}
	free(y);
	return res;
      }
      for (int i=0;i<n2;++i){
	mp_obj_t line = mp_obj_new_list(0, NULL);
	for (int j=0;j<m2;++j,++x_){
	  c_complex c=mult(*x_,a);
	  mp_obj_list_append(line,mp_obj_new_complex_real(c.r,c.i));
	}
	mp_obj_list_append(res,line);
      }
      free(y);
      return res;      
    }
    free(y);
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_dot_obj, 2, 2, linalg_dot);

static mp_obj_t linalg_cross(size_t n_args, const mp_obj_t *args) {
  c_complex *x,*y; 
  size_t n1,m1,n2,m2;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1) && n1==3 && m1==0){
    if (!mp_array2c_complextab(args[1],&y,&n2,&m2)){
      free(x);
      nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 2nd argument."));
    }
    if (n2!=3 || m2!=0){
      free(x);
      free(y);
      nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 2nd argument."));
    }
    c_complex tmp1,tmp2,A,B,C;
    c_mul(x[0],y[1],&tmp1);c_mul(x[1],y[0],&tmp2); c_sub(tmp1,tmp2,&C);
    c_mul(x[2],y[0],&tmp1);c_mul(x[0],y[2],&tmp2); c_sub(tmp1,tmp2,&B);
    c_mul(x[1],y[2],&tmp1);c_mul(x[2],y[1],&tmp2); c_sub(tmp1,tmp2,&A);
    free(x);
    free(y);
    mp_obj_t r = mp_obj_new_list(0, NULL);
    mp_obj_list_append(r,mp_obj_new_complex_real(A.r,A.i));
    mp_obj_list_append(r,mp_obj_new_complex_real(B.r,B.i));
    mp_obj_list_append(r,mp_obj_new_complex_real(C.r,C.i));
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_cross_obj, 2, 2, linalg_cross);

static mp_obj_t linalg_transpose(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    transpose(x,n1,m1);
    mp_obj_t r=c_complextab2mp_array(x,m1,n1);
    free(x);
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_transpose_obj, 1, 1, linalg_transpose);

static mp_obj_t linalg_rref(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (!c_rref(x,n1,m1))
      free(x);
    else {
      mp_obj_t r=c_complextab2mp_array(x,n1,m1);
      free(x);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_rref_obj, 1, 1, linalg_rref);

static mp_obj_t linalg_egv(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (n1!=m1 || !c_egv(x,n1))
      free(x);
    else {
      mp_obj_t r=c_complextab2mp_array(x,n1,m1);
      free(x);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_egv_obj, 1, 1, linalg_egv);

static mp_obj_t linalg_eig(size_t n_args, const mp_obj_t *args) {
  c_complex *x,*d;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    d=(c_complex *) malloc(n1*n1*sizeof(c_complex));
    if (n1!=m1 || !c_eig(x,d,n1)){
      free(x); free(d);
    }
    else {
      mp_obj_t items[2];
      items[0]=c_complextab2mp_array(x,n1,m1);
      items[1]=c_complextab2mp_array(d,n1,m1);
      mp_obj_t r=mp_obj_new_tuple(2, items);
      free(x); free(d);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(giac_linalg_eig_obj, 1, 1, linalg_eig);

static mp_obj_t linalg_inv(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (n1!=m1){
      free(x);
      raisedimerr();
    }
    if (!c_inv(x,n1))
      free(x);
    else {
      mp_obj_t r=c_complextab2mp_array(x,m1,m1);
      free(x);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(giac_linalg_inv_obj, 1, 1, linalg_inv);

static mp_obj_t linalg_re(size_t n_args, const mp_obj_t *args) {
  if (MP_OBJ_IS_INT(args[0]) || mp_obj_is_float(args[0]))
    return args[0];
  if (mp_obj_get_type(args[0])==&mp_type_complex){
    double R,I;
    mp_obj_get_complex(args[0],&R,&I);
    return mp_obj_new_float(R);
  }
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    int N=m1?n1*m1:n1;
    for (int i=0;i<N;++i)
      x[i].i=0;
    mp_obj_t r=c_complextab2mp_array(x,n1,m1);
    free(x);
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_re_obj, 1, 1, linalg_re);

static mp_obj_t linalg_im(size_t n_args, const mp_obj_t *args) {
  if (MP_OBJ_IS_INT(args[0]) || mp_obj_is_float(args[0]))
    return mp_obj_new_int(0);
  if (mp_obj_get_type(args[0])==&mp_type_complex){
    double R,I;
    mp_obj_get_complex(args[0],&R,&I);
    return mp_obj_new_float(I);
  }
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    int N=m1?n1*m1:n1;
    for (int i=0;i<N;++i){
      x[i].r=x[i].i;
      x[i].i=0;
    }
    mp_obj_t r=c_complextab2mp_array(x,n1,m1);
    free(x);
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_im_obj, 1, 1, linalg_im);

static mp_obj_t linalg_conj(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (MP_OBJ_IS_INT(args[0]) || mp_obj_is_float(args[0]))
    return args[0];
  if (mp_obj_get_type(args[0])==&mp_type_complex){
    double R,I;
    mp_obj_get_complex(args[0],&R,&I);
    return mp_obj_new_complex(R,-I);
  }
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    int N=m1?n1*m1:n1;
    for (int i=0;i<N;++i){
      x[i].i=-x[i].i;
    }
    mp_obj_t r=c_complextab2mp_array(x,n1,m1);
    free(x);
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_conj_obj, 1, 1, linalg_conj);

double c_abs(c_complex c){
  double x=c.r<0?-c.r:c.r;
  double y=c.i<0?-c.i:c.i;
  if (x>y){
    double t=x;
    x=y;
    y=t;
  }
  // now x<=y
  if (y==0) return 0;
  double t=x/y;
  return y*sqrt(1+t*t);
}
static mp_obj_t linalg_abs(size_t n_args, const mp_obj_t *args) {
  if (MP_OBJ_IS_SMALL_INT(args[0])){
    long long i=MP_OBJ_SMALL_INT_VALUE(args[0]);
    if (i>=0)
      return args[0];
    return mp_obj_new_int(-i);
  }
  if (MP_OBJ_IS_INT(args[0]) || mp_obj_is_float(args[0])){
    return mp_obj_new_float(fabs(mp_obj_get_float(args[0])));
  }
  if (mp_obj_get_type(args[0])==&mp_type_complex){
    double R,I;
    mp_obj_get_complex(args[0],&R,&I);
    R=fabs(R); I=fabs(I);
    if (R<I){
      double t=R;
      R=I;
      I=t;
    }
    double t=I/R;
    return mp_obj_new_float(R*sqrt(1+t*t));
  }
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    int N=m1?n1*m1:n1;
    for (int i=0;i<N;++i){
      x[i].r=c_abs(x[i]);
      x[i].i=0;
    }
    mp_obj_t r=c_complextab2mp_array(x,n1,m1);
    free(x);
    return r;
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_abs_obj, 1, 1, linalg_abs);

static mp_obj_t linalg_proot(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (m1!=0){
      free(x);
      raisedimerr();
    }
    if (!c_proot(x,n1))
      free(x);
    else {
      mp_obj_t r=c_complextab2mp_array(x,n1-1,0);
      free(x);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_proot_obj, 1, 1, linalg_proot);

static mp_obj_t linalg_pcoeff(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (m1!=0){
      free(x);
      raisedimerr();
    }
    c_complex y[n1+1];
    for (int i=0;i<n1;++i)
      y[i]=x[i];
    free(x);
    if (c_pcoeff(y,n1)){
      mp_obj_t r=c_complextab2mp_array(y,n1+1,0);
      return r;
    }
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_pcoeff_obj, 1, 1, linalg_pcoeff);

static mp_obj_t linalg_fftifft(size_t n_args, const mp_obj_t *args,bool inverse) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    int n2=n1,i;
    for (i=-1;n2;++i){
      n2/=2;
    }
    if (m1!=0 || n1!=(1<<i)){
      free(x);
      raisedimerr();
    }
    if (c_fft(x,n1,inverse)){
      mp_obj_t r=c_complextab2mp_array(x,n1,0);
      free(x);
      return r;
    }
    free(x);    
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
static mp_obj_t linalg_fft(size_t n_args, const mp_obj_t *args) {
  return linalg_fftifft(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_fft_obj, 1, 1, linalg_fft);
static mp_obj_t linalg_ifft(size_t n_args, const mp_obj_t *args) {
  return linalg_fftifft(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_ifft_obj, 1, 1, linalg_ifft);

/* for a more efficient linear solver, use lu inside the CAS*/
static mp_obj_t linalg_solve(size_t n_args, const mp_obj_t *args) {
  mp_obj_t arg[2];
  arg[0]=linalg_inv(1,args);
  arg[1]=args[1];
  return linalg_dot(2,arg);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_solve_obj, 2, 2, linalg_solve);

static mp_obj_t linalg_peval(size_t n_args, const mp_obj_t *args) {
  c_complex *P,x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&P,&n1,&m1)){
    if (m1!=0 || !mp_get_c_complex(args[1],&x)){
      free(P);
      raisedimerr();
    }
    c_complex res={0,0};
    for (int i=0;i<n1;++i){
      double r=res.r*x.r-res.i*x.i+P[i].r;
      res.i=res.r*x.i+res.i*x.r+P[i].i;
      res.r=r;
    }
    if (res.i==0)
      return mp_obj_new_float(res.r);
    else
      return mp_obj_new_complex(res.r,res.i);
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_peval_obj, 2, 2, linalg_peval);

static mp_obj_t linalg_apply(size_t n_args, const mp_obj_t *args) {
  mp_obj_t * elem=0; size_t n=-1;
  bool recur=false;
  if (n_args==3 && args[2]==&linalg_matrix_obj)
    recur=true;
  if (MP_OBJ_IS_TYPE(args[1],&mp_type_list) || MP_OBJ_IS_TYPE(args[1],&mp_type_tuple))
    mp_obj_get_array(args[1], &n, &elem);
  if (MP_OBJ_IS_TYPE(args[0], &mp_type_fun_builtin_1)){
    if (((int) n)>=0){
      mp_obj_t r = mp_obj_new_list(0, NULL);
      mp_obj_t args_[2];
      args_[0]=args[0];
      for (size_t i=0;i<n;++i){
	if (recur){
	  args_[1]=elem[i];
	  mp_obj_list_append(r,linalg_apply(2,args_));
	}
	else
	  mp_obj_list_append(r,fun_builtin_1_call(args[0],1,0,&elem[i]));	  
      }
      return r;
    }
    return fun_builtin_1_call(args[0],1,0,args+1);
  }
  else {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
    if (((int) n)>=0){
      mp_obj_t r = mp_obj_new_list(0, NULL);
      mp_obj_t args_[2];
      args_[0]=args[0];
      for (size_t i=0;i<n;++i){
	if (recur){
	  args_[1]=elem[i];
	  mp_obj_list_append(r,linalg_apply(2,args_));
	}
	else
	  mp_obj_list_append(r,fun_bc_call(args[0],1,0,&elem[i]));	  
      }
      return r;
    }
    return fun_bc_call(args[0],1,0,args+1);
  }
  //nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_apply_obj, 2, 3, linalg_apply);

static mp_obj_t linalg_det(size_t n_args, const mp_obj_t *args) {
  c_complex *x;
  size_t n1,m1;
  if (mp_array2c_complextab(args[0],&x,&n1,&m1)){
    if (n1!=m1){
      free(x);
      raisedimerr();
    }
    c_complex r=c_det(x,n1);
    free(x);
    return mp_obj_new_complex_real(r.r,r.i);
  }
  nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Wrong type of 1st argument."));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(giac_linalg_det_obj, 1, 1, linalg_det);

static mp_obj_t linalg_zerosones(size_t n_args, const mp_obj_t *args,int v0,int v1) {
  size_t n1=0,m1=0;
  if (MP_OBJ_IS_SMALL_INT(args[0]))
    n1= MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args==2 && MP_OBJ_IS_SMALL_INT(args[1]))
    m1= MP_OBJ_SMALL_INT_VALUE(args[1]);
  if (v0==0 && v1==1)
    m1=n1;
  mp_obj_t r = mp_obj_new_list(0, NULL);
  bool alea=v0==-1;
  for (int i=0;i<n1;++i){
    if (m1){
      mp_obj_t l = mp_obj_new_list(0, NULL);
      for (int j=0;j<m1;++j){
	mp_obj_list_append(l, mp_obj_new_float(alea?rand()/(RAND_MAX+1.0):(i==j?v1:v0)));	
      }
      mp_obj_list_append(r, l);
    }
    else
      mp_obj_list_append(r, mp_obj_new_float(alea?rand()/(RAND_MAX+1.0):v0));
  }
  return r;
}
static mp_obj_t linalg_zeros(size_t n_args, const mp_obj_t *args) {
  return linalg_zerosones(n_args,args,0,0);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_zeros_obj, 1, 2, linalg_zeros);
static mp_obj_t linalg_ones(size_t n_args, const mp_obj_t *args) {
  return linalg_zerosones(n_args,args,1,1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_ones_obj, 1, 2, linalg_ones);
static mp_obj_t linalg_eye(size_t n_args, const mp_obj_t *args) {
  return linalg_zerosones(n_args,args,0,1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_eye_obj, 1, 1, linalg_eye);
static mp_obj_t linalg_rand(size_t n_args, const mp_obj_t *args) {
  return linalg_zerosones(n_args,args,-1,0);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_rand_obj, 1, 2, linalg_rand);

static mp_obj_t linalg_sizeshape(size_t n_args, const mp_obj_t *args,bool shape) {
  int l=-1,m=0;
  if (mp_obj_get_type(args[0])==&mp_type_list){
    mp_obj_list_t *o = MP_OBJ_TO_PTR(args[0]);
    l=o->len;
    if (l>0){
      mp_obj_t obj=o->items[0];
      if (mp_obj_get_type(obj)==&mp_type_list){
	o = MP_OBJ_TO_PTR(obj);
	m=o->len;
      }
    }
  }
  if (shape){
    mp_obj_t items[2];
    items[0]=mp_obj_new_int(l);
    items[1]=m==0?mp_const_none:mp_obj_new_int(m);
    mp_obj_t r=mp_obj_new_tuple(2, items);
    return r;
  }
  if (m==0) m=1;
  return mp_obj_new_int(l*m);
}
static mp_obj_t linalg_shape(size_t n_args, const mp_obj_t *args) {
  return linalg_sizeshape(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_shape_obj, 1, 1, linalg_shape);

static mp_obj_t linalg_size(size_t n_args, const mp_obj_t *args) {
  return linalg_sizeshape(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(linalg_size_obj, 1, 1, linalg_size);

//
static const mp_rom_map_elem_t linalg_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pi), mp_const_float_pi },
    { MP_ROM_QSTR(MP_QSTR_matrix), (mp_obj_t) &linalg_matrix_obj },
    { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t) &linalg_arange_obj },
    { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t) &linalg_linspace_obj },
    { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t) &linalg_dot_obj },
    { MP_ROM_QSTR(MP_QSTR_cross), (mp_obj_t) &linalg_cross_obj },
    { MP_ROM_QSTR(MP_QSTR_solve), (mp_obj_t) &linalg_solve_obj },
    { MP_ROM_QSTR(MP_QSTR_peval), (mp_obj_t) &linalg_peval_obj },
    { MP_ROM_QSTR(MP_QSTR_horner), (mp_obj_t) &linalg_peval_obj },
    { MP_ROM_QSTR(MP_QSTR_mul), (mp_obj_t) &linalg_dot_obj },
    { MP_ROM_QSTR(MP_QSTR_transpose), (mp_obj_t) &linalg_transpose_obj },
    { MP_ROM_QSTR(MP_QSTR_rref), (mp_obj_t) &linalg_rref_obj },
    { MP_ROM_QSTR(MP_QSTR_egv), (mp_obj_t) &linalg_egv_obj },
    { MP_ROM_QSTR(MP_QSTR_eigenvects), (mp_obj_t) &linalg_egv_obj },
    { MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t) &giac_linalg_eig_obj },
    { MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t) &giac_linalg_det_obj },
    { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t) &linalg_zeros_obj },
    { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t) &linalg_ones_obj },
    { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_identity), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_idn), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_rand), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_ranv), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_ranm), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_shape), (mp_obj_t) &linalg_shape_obj },
    { MP_ROM_QSTR(MP_QSTR_size), (mp_obj_t) &linalg_size_obj },
    { MP_ROM_QSTR(MP_QSTR_inv), (mp_obj_t) &giac_linalg_inv_obj },
    { MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_proot), (mp_obj_t) &linalg_proot_obj },
    { MP_ROM_QSTR(MP_QSTR_pcoeff), (mp_obj_t) &linalg_pcoeff_obj },
    { MP_ROM_QSTR(MP_QSTR_fft), (mp_obj_t) &linalg_fft_obj },
    { MP_ROM_QSTR(MP_QSTR_ifft), (mp_obj_t) &linalg_ifft_obj },
    { MP_ROM_QSTR(MP_QSTR_apply), (mp_obj_t) &linalg_apply_obj },
    { MP_ROM_QSTR(MP_QSTR_add), (mp_obj_t) &linalg_add_obj },
    { MP_ROM_QSTR(MP_QSTR_sub), (mp_obj_t) &linalg_sub_obj },
    { MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_real), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_im), (mp_obj_t) &linalg_im_obj },
    { MP_ROM_QSTR(MP_QSTR_imag), (mp_obj_t) &linalg_im_obj },
    { MP_ROM_QSTR(MP_QSTR_conj), (mp_obj_t) &linalg_conj_obj },
    { MP_ROM_QSTR(MP_QSTR_abs), (mp_obj_t) &linalg_abs_obj },
};


static MP_DEFINE_CONST_DICT(linalg_locals_dict, linalg_locals_dict_table);

const mp_obj_type_t linalg_type = {
    { &mp_type_type },
    .name = MP_QSTR_linalg,
    .locals_dict = (mp_obj_t)&linalg_locals_dict
};


STATIC const mp_rom_map_elem_t mp_module_linalg_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__linalg) },
    { MP_ROM_QSTR(MP_QSTR_pi), mp_const_float_pi },
    { MP_ROM_QSTR(MP_QSTR_matrix), (mp_obj_t) &linalg_matrix_obj },
    { MP_ROM_QSTR(MP_QSTR_arange), (mp_obj_t) &linalg_arange_obj },
    { MP_ROM_QSTR(MP_QSTR_linspace), (mp_obj_t) &linalg_linspace_obj },
    { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t) &linalg_dot_obj },
    { MP_ROM_QSTR(MP_QSTR_cross), (mp_obj_t) &linalg_cross_obj },
    { MP_ROM_QSTR(MP_QSTR_solve), (mp_obj_t) &linalg_solve_obj },
    { MP_ROM_QSTR(MP_QSTR_peval), (mp_obj_t) &linalg_peval_obj },
    { MP_ROM_QSTR(MP_QSTR_horner), (mp_obj_t) &linalg_peval_obj },
    { MP_ROM_QSTR(MP_QSTR_mul), (mp_obj_t) &linalg_dot_obj },
    { MP_ROM_QSTR(MP_QSTR_transpose), (mp_obj_t) &linalg_transpose_obj },
    { MP_ROM_QSTR(MP_QSTR_rref), (mp_obj_t) &linalg_rref_obj },
    { MP_ROM_QSTR(MP_QSTR_egv), (mp_obj_t) &linalg_egv_obj },
    { MP_ROM_QSTR(MP_QSTR_eigenvects), (mp_obj_t) &linalg_egv_obj },
    { MP_ROM_QSTR(MP_QSTR_eig), (mp_obj_t) &giac_linalg_eig_obj },
    { MP_ROM_QSTR(MP_QSTR_det), (mp_obj_t) &giac_linalg_det_obj },
    { MP_ROM_QSTR(MP_QSTR_zeros), (mp_obj_t) &linalg_zeros_obj },
    { MP_ROM_QSTR(MP_QSTR_ones), (mp_obj_t) &linalg_ones_obj },
    { MP_ROM_QSTR(MP_QSTR_eye), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_identity), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_idn), (mp_obj_t) &linalg_eye_obj },
    { MP_ROM_QSTR(MP_QSTR_rand), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_ranv), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_ranm), (mp_obj_t) &linalg_rand_obj },
    { MP_ROM_QSTR(MP_QSTR_shape), (mp_obj_t) &linalg_shape_obj },
    { MP_ROM_QSTR(MP_QSTR_size), (mp_obj_t) &linalg_size_obj },
    { MP_ROM_QSTR(MP_QSTR_inv), (mp_obj_t) &giac_linalg_inv_obj },
    { MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_proot), (mp_obj_t) &linalg_proot_obj },
    { MP_ROM_QSTR(MP_QSTR_pcoeff), (mp_obj_t) &linalg_pcoeff_obj },
    { MP_ROM_QSTR(MP_QSTR_fft), (mp_obj_t) &linalg_fft_obj },
    { MP_ROM_QSTR(MP_QSTR_ifft), (mp_obj_t) &linalg_ifft_obj },
    { MP_ROM_QSTR(MP_QSTR_apply), (mp_obj_t) &linalg_apply_obj },
    { MP_ROM_QSTR(MP_QSTR_add), (mp_obj_t) &linalg_add_obj },
    { MP_ROM_QSTR(MP_QSTR_sub), (mp_obj_t) &linalg_sub_obj },
    { MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_real), (mp_obj_t) &linalg_re_obj },
    { MP_ROM_QSTR(MP_QSTR_im), (mp_obj_t) &linalg_im_obj },
    { MP_ROM_QSTR(MP_QSTR_imag), (mp_obj_t) &linalg_im_obj },
    { MP_ROM_QSTR(MP_QSTR_conj), (mp_obj_t) &linalg_conj_obj },
    { MP_ROM_QSTR(MP_QSTR_abs), (mp_obj_t) &linalg_abs_obj },
};

STATIC const mp_obj_dict_t mp_module_linalg_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_linalg_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_linalg_globals_table),
        .table = (mp_map_elem_t*)mp_module_linalg_globals_table,
    },
};

const mp_obj_module_t mp_module_linalg = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_linalg_globals,
};


/* TURTLE, using caseval */
void turtle_freeze();

mp_obj_t turtle_ret(const char * val){
  py_ck_ctrl_c();
  if (strncmp(val,"\"Done",5)==0)
    return mp_const_none;
  return mp_obj_new_str(val,strlen(val));  
}

static mp_obj_t turtle_forward(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=10;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args==1 && mp_obj_is_float(args[0])) 
    i=mp_obj_get_float(args[0]);
  c_turtle_forward(i);
  return mp_const_none;
  char buf[256];
  sprintf(buf,"avance(%.4g):;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_forward_obj, 0, 1, turtle_forward);

static mp_obj_t turtle_backward(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=10;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args==1 && mp_obj_is_float(args[0])) 
    i=mp_obj_get_float(args[0]);
  c_turtle_forward(-i);
  return mp_const_none;
  char buf[256];
  sprintf(buf,"recule(%.4g):;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_backward_obj, 0, 1, turtle_backward);

static mp_obj_t turtle_left(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=90;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  else if (n_args==1 && mp_obj_is_float(args[0])) 
    i=mp_obj_get_float(args[0]);
  else if (n_args==1) i=0;  
  c_turtle_left(i);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_left_obj, 0, 1, turtle_left);

static mp_obj_t turtle_pensize(size_t n_args, const mp_obj_t *args) {
  if (n_args==0){
    return mp_obj_new_int(c_turtle_crayon(-128));
  }
  turtle_freeze();
  int i=1;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args==1 && mp_obj_is_float(args[0])) 
    i=mp_obj_get_float(args[0]);
  if (i<1) i=1;
  c_turtle_crayon(-i);
  return mp_const_none;
  char buf[256];
  sprintf(buf,"crayon -%i:;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_pensize_obj, 0, 1, turtle_pensize);

static mp_obj_t turtle_right(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=90;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  else if (n_args==1 && mp_obj_is_float(args[0])) 
    i=mp_obj_get_float(args[0]);
  else if (n_args==1) i=0;
  c_turtle_left(-i);
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_right_obj, 0, 1, turtle_right);

static mp_obj_t turtle_reset(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_clear(1); return mp_const_none;
  const char * val=caseval("efface(0,0):;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_reset_obj, 0, 0, turtle_reset);

static mp_obj_t turtle_clear(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_clear(0); return mp_const_none;
  const char * val=caseval("efface(op(position())):;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_clear_obj, 0, 0, turtle_clear);

static mp_obj_t turtle_dessine_tortue(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  const char * val=caseval("efface(0,0):;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_dessine_tortue_obj, 0, 0, turtle_dessine_tortue);

static mp_obj_t turtle_circle(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int x=0,y=0,z=360;
  if (MP_OBJ_IS_SMALL_INT(args[0])) 
    x=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args>=2 && MP_OBJ_IS_SMALL_INT(args[1])) 
    y=MP_OBJ_SMALL_INT_VALUE(args[1]);
  if (n_args==3 && MP_OBJ_IS_SMALL_INT(args[2])) {
    z=MP_OBJ_SMALL_INT_VALUE(args[2]);
  }
  else {
    if (n_args==2){
      z=y;
      y=0;
    }
  }
  c_turtle_rond(x,y,z);
  return mp_const_none;
  char buf[256];
  sprintf(buf,"rond(%i,%i,%i):;",x,y,z);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_circle_obj, 1, 3, turtle_circle);

static mp_obj_t do_turtle_disque(size_t n_args, const mp_obj_t *args,bool centered) {
  turtle_freeze();
  int x=5,y=0,z=360;
  if (n_args>=1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    x=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args>=2 && MP_OBJ_IS_SMALL_INT(args[1])) 
    y=MP_OBJ_SMALL_INT_VALUE(args[1]);
  if (n_args==3 && MP_OBJ_IS_SMALL_INT(args[2])) {
    z=MP_OBJ_SMALL_INT_VALUE(args[2]);
  }
  else {
    if (n_args==2){
      z=y;
      y=0;
    }
  }
  c_turtle_disque(x,y,z,centered);
  return mp_const_none;
  char buf[256];
  sprintf(buf,centered?"disque_centre(%i,%i,%i):;":"disque(%i,%i,%i):;",x/2,y,z);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
static mp_obj_t turtle_disque(size_t n_args, const mp_obj_t *args) {
  return do_turtle_disque(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_disque_obj, 0, 3, turtle_disque);

static mp_obj_t turtle_dot(size_t n_args, const mp_obj_t *args) {
  return do_turtle_disque(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_dot_obj, 0, 3, turtle_dot);

static mp_obj_t turtle_rectangle_plein(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int x=10,y=10;
  if (MP_OBJ_IS_SMALL_INT(args[0])) 
    y=x=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args>=2 && MP_OBJ_IS_SMALL_INT(args[1])) 
    y=MP_OBJ_SMALL_INT_VALUE(args[1]);
  char buf[256];
  sprintf(buf,"rectangle_plein(%i,%i):;",x,y);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_rectangle_plein_obj, 1, 2, turtle_rectangle_plein);

static mp_obj_t turtle_triangle_plein(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int x=10,y=10,z=60;
  if (MP_OBJ_IS_SMALL_INT(args[0])) 
    y=x=MP_OBJ_SMALL_INT_VALUE(args[0]);
  if (n_args>=2 && MP_OBJ_IS_SMALL_INT(args[1])) 
    y=MP_OBJ_SMALL_INT_VALUE(args[1]);
  if (n_args>=3 && MP_OBJ_IS_SMALL_INT(args[2])) 
    y=MP_OBJ_SMALL_INT_VALUE(args[2]);
  char buf[256];
  sprintf(buf,"triangle_plein(%i,%i,%i):;",x,y,z);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_triangle_plein_obj, 1, 3, turtle_triangle_plein);

static mp_obj_t turtle_showturtle(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_show(1); return mp_const_none;
  const char * val=caseval("montre_tortue():;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_showturtle_obj, 0, 0, turtle_showturtle);

static mp_obj_t turtle_hideturtle(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_show(0); return mp_const_none;
  const char * val=caseval("cache_tortue():;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_hideturtle_obj, 0, 0, turtle_hideturtle);

static mp_obj_t turtle_down(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_up(0);
  return mp_const_none;
  const char * val=caseval("baisse_crayon():;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_down_obj, 0, 0, turtle_down);

static mp_obj_t turtle_up(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_up(1);
  return mp_const_none;
  const char * val=caseval("leve_crayon():;");
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_up_obj, 0, 0, turtle_up);

static mp_obj_t turtle_saute(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int i=90;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  char buf[256];
  sprintf(buf,"saute(%i):;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_saute_obj, 1, 1, turtle_saute);

static mp_obj_t turtle_pas_de_cote(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int i=90;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  char buf[256];
  sprintf(buf,"pas_de_cote(%i):;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_pas_de_cote_obj, 1, 1, turtle_pas_de_cote);

static mp_obj_t turtle_towards(size_t n_args, const mp_obj_t *args) {
  double x,y;
  if (n_args!=2 || !mp_int_float(args[0],&x) || !mp_int_float(args[1],&y))
    mp_raise_TypeError("x,y expected");
  c_turtle_towards(x,y);
#ifndef NUMWORKS
  char buf[256];
  sprintf(buf,"towards(%.4f,%.4f);",x,y);
#else
  char buf[256]="towards(";
  strcat_double(buf,x);
  strcat(buf,",");
  strcat_double(buf,y);
  strcat(buf,");");
#endif
  const char * val=caseval(buf);
  x=strtod(val,0); 
  return mp_obj_new_float(x);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_towards_obj, 2, 2, turtle_towards);

static mp_obj_t turtle_setheading(size_t n_args, const mp_obj_t *args) {
  if (n_args==0){
    return mp_obj_new_int(c_turtle_getcap());
    char buf[]="cap()";
    const char * val=caseval(buf);
    return str2int(val);
  }
  turtle_freeze();
  double i=0;
  if (n_args==1){
    if (!mp_int_float(args[0],&i))
      mp_raise_TypeError("int/float expected");
  }
  c_turtle_cap(i);
  return mp_const_none;
#ifndef NUMWORKS
  char buf[256];
  sprintf(buf,"cap(%.4f):;",i);
#else
  char buf[256]="cap(";
  strcat_double(buf,i);
  strcat(buf,"):;");
#endif
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_setheading_obj, 0, 1, turtle_setheading);

static mp_obj_t turtle_setposition(size_t n_args, const mp_obj_t *args) {
  if (n_args<2){
    double x,y;
    c_turtle_getposition(&x,&y);
    mp_obj_t res = mp_obj_new_list(0, NULL);
    mp_obj_list_append(res,mp_obj_new_int(x));
    mp_obj_list_append(res,mp_obj_new_int(y));
    return res;
    char buf[]="position();";
    const char * val=caseval(buf);
    int l=strlen(val);
    if (l<5 || val[0]!='[' || val[l-1]!=']')
      return mp_obj_new_str(val,l);
    // parse 2 integers separated by a comma
    return str2list2int(val,2);
  }
  turtle_freeze();
  double x=0,y=0;
  if (!mp_int_float(args[0],&x) || !mp_int_float(args[1],&y))
    mp_raise_TypeError("x,y expected");    
  c_turtle_goto(x,y);
  return mp_const_none;
#ifndef NUMWORKS
  char buf[256];
  sprintf(buf,"position(%.4f,%.4f):;",x,y);
#else
  char buf[256]="position(";
  strcat_double(buf,x);
  strcat(buf,",");
  strcat_double(buf,y);
  strcat(buf,"):;");
#endif
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_setposition_obj, 0, 2, turtle_setposition);

static mp_obj_t turtle_setx(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=0;
  if (n_args==1)
    mp_int_float(args[0],&i);
  double x,y; c_turtle_getposition(&x,&y);
  x=i; c_turtle_goto(x,y);
#ifndef NUMWORKS
  char buf[256];
  sprintf(buf,"position(%.4f,position()[1]):;",i);
#else
  char buf[256]="position(";
  strcat_double(buf,i);
  strcat(buf,"position()[1]):;");
#endif
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_setx_obj, 0, 1, turtle_setx);

static mp_obj_t turtle_sety(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  double i=0;
  if (n_args==1)
    mp_int_float(args[0],&i);
  double x,y; c_turtle_getposition(&x,&y);
  y=i; c_turtle_goto(x,y);
#ifndef NUMWORKS
  char buf[256];
  sprintf(buf,"position(position()[0],%.4f):;",i);
#else
  char buf[256]="position(position()[0],";
  strcat_double(buf,i);
  strcat(buf,"):;");
#endif
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_sety_obj, 0, 1, turtle_sety);

static mp_obj_t turtle_write(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  char buf[256];
  sprintf(buf,"ecris(%s):;",mp_obj_str_get_str(args[0]));
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_write_obj, 1, 1, turtle_write);

static mp_obj_t turtle_colormode(size_t n_args, const mp_obj_t *args) {
  if (n_args==0)
    return mp_obj_new_int(255);    
  return turtle_ret("\"Done");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_colormode_obj, 0, 1, turtle_colormode);

static mp_obj_t turtle_fillcolor(size_t n_args, const mp_obj_t *args) {
  int i=0;
  char buf[256]="polygone_rempli()";
  if (n_args==1){
    if (MP_OBJ_IS_STR(args[0]))
      sprintf(buf,"polygone_rempli(%s):;",mp_obj_str_get_str(args[0]));
    if (MP_OBJ_IS_SMALL_INT(args[0])) {
      i=MP_OBJ_SMALL_INT_VALUE(args[0]);
      sprintf(buf,"polygone_rempli([%i]):;",i);
    }
    if (strlen(buf)==17){
      size_t n=0; mp_obj_t * tab=0;
      mp_obj_get_array(args[0],&n,&tab);
      if (n==3 &&  mp_obj_is_float(tab[0]) && mp_obj_is_float(tab[1]) && mp_obj_is_float(tab[2])){
	c_turtle_fillcolor(mp_obj_get_float(tab[0]),mp_obj_get_float(tab[1]),mp_obj_get_float(tab[2]),0);
	return mp_const_none;
#ifdef NUMWORKS
	strcpy(buf,"polygone_rempli(");
	for (int i=0;i<3;++i){
	  strcat_double(buf,mp_obj_get_float(tab[i]));
	  if (i<2)
	    strcat(buf,",");
	}
	strcat(buf,"):;");
#else
	sprintf(buf,"polygone_rempli(%.3f,%.3f,%.3f):;",mp_obj_get_float(tab[0]),mp_obj_get_float(tab[1]),mp_obj_get_float(tab[2]));
#endif
      }
      if (n==3 &&  MP_OBJ_IS_SMALL_INT(tab[0]) && MP_OBJ_IS_SMALL_INT(tab[1]) && MP_OBJ_IS_SMALL_INT(tab[2])){
	c_turtle_fillcolor((int)MP_OBJ_SMALL_INT_VALUE(tab[0]),(int)MP_OBJ_SMALL_INT_VALUE(tab[1]),(int)MP_OBJ_SMALL_INT_VALUE(tab[2]),1);
	return mp_const_none;
	sprintf(buf,"polygone_rempli(%i,%i,%i):;",(int)MP_OBJ_SMALL_INT_VALUE(tab[0]),(int)MP_OBJ_SMALL_INT_VALUE(tab[1]),(int)MP_OBJ_SMALL_INT_VALUE(tab[2]));
      }
    }
  }
  if (n_args==3 &&  MP_OBJ_IS_SMALL_INT(args[0]) && MP_OBJ_IS_SMALL_INT(args[1]) && MP_OBJ_IS_SMALL_INT(args[2])){
    sprintf(buf,"polygone_rempli(%i,%i,%i):;",(int)MP_OBJ_SMALL_INT_VALUE(args[0]),(int)MP_OBJ_SMALL_INT_VALUE(args[1]),(int)MP_OBJ_SMALL_INT_VALUE(args[2]));
  }
  if (n_args==3 && mp_obj_is_float(args[0]) && mp_obj_is_float(args[1]) && mp_obj_is_float(args[2]) ){
#ifdef NUMWORKS
	strcpy(buf,"polygone_rempli(");
	for (int i=0;i<3;++i){
	  strcat_double(buf,mp_obj_get_float(args[i]));
	  if (i<2)
	    strcat(buf,",");
	}
	strcat(buf,"):;");
#else
    sprintf(buf,"polygone_rempli(%.3f,%.3f,%.3f):;",mp_obj_get_float(args[0]),mp_obj_get_float(args[1]),mp_obj_get_float(args[2]));
#endif
  }
  //printf(buf);
  const char * val=caseval(buf);
  if (n_args==0) return mp_obj_new_str(val,strlen(val));
  return turtle_ret(val);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_fillcolor_obj, 0, 3, turtle_fillcolor);

static mp_obj_t turtle_begin_fill(size_t n_args, const mp_obj_t *args) {
  c_turtle_fill(1);
  return mp_const_none;
  const char * val=caseval("polygone_rempli([]):;");
  return turtle_ret(val);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_begin_fill_obj, 0, 0, turtle_begin_fill);

static mp_obj_t turtle_end_fill(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  c_turtle_fill(0);
  return mp_const_none;
  const char * val=caseval("polygone_rempli():;");
  return turtle_ret(val);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_end_fill_obj, 0, 0, turtle_end_fill);

static mp_obj_t turtle_pencolor(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  if (n_args==0)
    return mp_obj_new_int(c_turtle_getcolor());
  if (n_args==2){
    int i=mp_get_color(args[0]);
    c_turtle_fillcolor1(i);
  }
  int j=mp_get_color(args[0]);
  c_turtle_color(j);
  return mp_const_none;
  int i=0;
  char buf[256]="crayon";
  if (n_args==1){
    if (MP_OBJ_IS_STR(args[0]))
      sprintf(buf,"crayon(%s):;",mp_obj_str_get_str(args[0]));
    if (MP_OBJ_IS_SMALL_INT(args[0])) {
      i=MP_OBJ_SMALL_INT_VALUE(args[0]);
      sprintf(buf,"crayon(%i):;",i);
    }
    if (strlen(buf)==6){
      size_t n=0; mp_obj_t * tab=0;
      mp_obj_get_array(args[0],&n,&tab);
      if (n==3 &&  mp_obj_is_float(tab[0]) && mp_obj_is_float(tab[1]) && mp_obj_is_float(tab[2])){
#ifdef NUMWORKS
	strcpy(buf,"crayon(");
	for (int i=0;i<3;++i){
	  strcat_double(buf,mp_obj_get_float(tab[i]));
	  if (i<2)
	    strcat(buf,",");
	}
	strcat(buf,"):;");
#else
	sprintf(buf,"crayon(%.3f,%.3f,%.3f):;",mp_obj_get_float(tab[0]),mp_obj_get_float(tab[1]),mp_obj_get_float(tab[2]));
#endif
      }
      if (n==3 &&  MP_OBJ_IS_SMALL_INT(tab[0]) && MP_OBJ_IS_SMALL_INT(tab[1]) && MP_OBJ_IS_SMALL_INT(tab[2])){
	sprintf(buf,"crayon(%i,%i,%i):;",(int)MP_OBJ_SMALL_INT_VALUE(tab[0]),(int)MP_OBJ_SMALL_INT_VALUE(tab[1]),(int)MP_OBJ_SMALL_INT_VALUE(tab[2]));
      }
    }
  }
  if (n_args==3 &&  MP_OBJ_IS_SMALL_INT(args[0]) && MP_OBJ_IS_SMALL_INT(args[1]) && MP_OBJ_IS_SMALL_INT(args[2])){
    sprintf(buf,"crayon(%i,%i,%i):;",(int)MP_OBJ_SMALL_INT_VALUE(args[0]),(int)MP_OBJ_SMALL_INT_VALUE(args[1]),(int)MP_OBJ_SMALL_INT_VALUE(args[2]));
  }
  if (n_args==3 && mp_obj_is_float(args[0]) && mp_obj_is_float(args[1]) && mp_obj_is_float(args[2]) ){
#ifdef NUMWORKS
	strcpy(buf,"crayon(");
	for (int i=0;i<3;++i){
	  strcat_double(buf,mp_obj_get_float(args[i]));
	  if (i<2)
	    strcat(buf,",");
	}
	strcat(buf,"):;");
#else
    sprintf(buf,"crayon(%.3f,%.3f,%.3f):;",mp_obj_get_float(args[0]),mp_obj_get_float(args[1]),mp_obj_get_float(args[2]));
#endif
  }
  if (n_args==2 ){
    int f=mp_get_color(args[0]),b=mp_get_color(args[1]);
    sprintf(buf,"crayon(%i),polygone_rempli(%i):;",f,b);
  }
  //printf(buf);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_pencolor_obj, 0, 2, turtle_pencolor);

static mp_obj_t turtle_speed(size_t n_args, const mp_obj_t *args) {
  turtle_freeze();
  int i=0;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0])) 
    i=MP_OBJ_SMALL_INT_VALUE(args[0]);
  char buf[256];
  sprintf(buf,"speed(%i):;",i);
  const char * val=caseval(buf);
  return turtle_ret(val);
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(turtle_speed_obj, 0, 1, turtle_speed);

static const mp_map_elem_t turtle_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_forward), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_fd), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_backward), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_bk), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_back), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_left), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_lt), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_right), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_rt), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_circle), (mp_obj_t) &turtle_circle_obj },
	{ MP_ROM_QSTR(MP_QSTR_disque), (mp_obj_t) &turtle_disque_obj },
	{ MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t) &turtle_dot_obj },
	{ MP_ROM_QSTR(MP_QSTR_rectangle_plein), (mp_obj_t) &turtle_rectangle_plein_obj },
	{ MP_ROM_QSTR(MP_QSTR_triangle_plein), (mp_obj_t) &turtle_triangle_plein_obj },
	{ MP_ROM_QSTR(MP_QSTR_reset), (mp_obj_t) &turtle_reset_obj },
	{ MP_ROM_QSTR(MP_QSTR_clear), (mp_obj_t) &turtle_clear_obj },
	{ MP_ROM_QSTR(MP_QSTR_dessine_tortue), (mp_obj_t) &turtle_dessine_tortue_obj },
	{ MP_ROM_QSTR(MP_QSTR_setheading), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_seth), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_setposition), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_towards), (mp_obj_t) &turtle_towards_obj },
	{ MP_ROM_QSTR(MP_QSTR_goto), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_setpos), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_setx), (mp_obj_t) &turtle_setx_obj },
	{ MP_ROM_QSTR(MP_QSTR_sety), (mp_obj_t) &turtle_sety_obj },
	{ MP_ROM_QSTR(MP_QSTR_write), (mp_obj_t) &turtle_write_obj },
	{ MP_ROM_QSTR(MP_QSTR_speed), (mp_obj_t) &turtle_speed_obj },
	{ MP_ROM_QSTR(MP_QSTR_fillcolor), (mp_obj_t) &turtle_fillcolor_obj },
	{ MP_ROM_QSTR(MP_QSTR_end_fill), (mp_obj_t) &turtle_end_fill_obj },
	{ MP_ROM_QSTR(MP_QSTR_begin_fill), (mp_obj_t) &turtle_begin_fill_obj },
	{ MP_ROM_QSTR(MP_QSTR_showturtle), (mp_obj_t) &turtle_showturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_st), (mp_obj_t) &turtle_showturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_hideturtle), (mp_obj_t) &turtle_hideturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_ht), (mp_obj_t) &turtle_hideturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_down), (mp_obj_t) &turtle_down_obj },
	{ MP_ROM_QSTR(MP_QSTR_saute), (mp_obj_t) &turtle_saute_obj },
	{ MP_ROM_QSTR(MP_QSTR_pas_de_cote), (mp_obj_t) &turtle_pas_de_cote_obj },
	{ MP_ROM_QSTR(MP_QSTR_pendown), (mp_obj_t) &turtle_down_obj },
	{ MP_ROM_QSTR(MP_QSTR_pd), (mp_obj_t) &turtle_down_obj },
	{ MP_ROM_QSTR(MP_QSTR_up), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_penup), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_pu), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_pencolor), (mp_obj_t) &turtle_pencolor_obj },
	{ MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t) &turtle_pencolor_obj },
    { MP_ROM_QSTR(MP_QSTR_colormode), (mp_obj_t) &turtle_colormode_obj },
	{ MP_ROM_QSTR(MP_QSTR_avance), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_av), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_recule), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_back), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_tourne_gauche), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_tg), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_tourne_droite), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_td), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_rond), (mp_obj_t) &turtle_circle_obj },
	{ MP_ROM_QSTR(MP_QSTR_efface), (mp_obj_t) &turtle_reset_obj },
	{ MP_ROM_QSTR(MP_QSTR_cap), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_heading), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_pos), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_position), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_ecris), (mp_obj_t) &turtle_write_obj },
	{ MP_ROM_QSTR(MP_QSTR_vitesse_tortue), (mp_obj_t) &turtle_speed_obj },
	{ MP_ROM_QSTR(MP_QSTR_montre_tortue), (mp_obj_t) &turtle_showturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_cache_tortue), (mp_obj_t) &turtle_hideturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_baisse_crayon), (mp_obj_t) &turtle_down_obj },
	{ MP_ROM_QSTR(MP_QSTR_leve_crayon), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_pensize), (mp_obj_t) &turtle_pensize_obj },
	{ MP_ROM_QSTR(MP_QSTR_width), (mp_obj_t) &turtle_pensize_obj },
	/* { MP_ROM_QSTR(MP_QSTR_time), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_sleep), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_monotonic), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_time), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_sleep), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_width), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_isdown), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_isvisible), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t) &turtle_up_obj },*/

};


static MP_DEFINE_CONST_DICT(turtle_locals_dict, turtle_locals_dict_table);

const mp_obj_type_t turtle_type = {
    { &mp_type_type },
    .name = MP_QSTR_turtle,
    .locals_dict = (mp_obj_t)&turtle_locals_dict
};


STATIC const mp_map_elem_t mp_module_turtle_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__turtle) },
    { MP_ROM_QSTR(MP_QSTR_forward), (mp_obj_t) &turtle_forward_obj },
    { MP_ROM_QSTR(MP_QSTR_fd), (mp_obj_t) &turtle_forward_obj },
    { MP_ROM_QSTR(MP_QSTR_backward), (mp_obj_t) &turtle_backward_obj },
    { MP_ROM_QSTR(MP_QSTR_bk), (mp_obj_t) &turtle_backward_obj },
    { MP_ROM_QSTR(MP_QSTR_back), (mp_obj_t) &turtle_backward_obj },
    { MP_ROM_QSTR(MP_QSTR_left), (mp_obj_t) &turtle_left_obj },
    { MP_ROM_QSTR(MP_QSTR_lt), (mp_obj_t) &turtle_left_obj },
    { MP_ROM_QSTR(MP_QSTR_right), (mp_obj_t) &turtle_right_obj },
    { MP_ROM_QSTR(MP_QSTR_rt), (mp_obj_t) &turtle_right_obj },
    { MP_ROM_QSTR(MP_QSTR_reset), (mp_obj_t) &turtle_reset_obj },
    { MP_ROM_QSTR(MP_QSTR_clear), (mp_obj_t) &turtle_clear_obj },
    { MP_ROM_QSTR(MP_QSTR_dessine_tortue), (mp_obj_t) &turtle_dessine_tortue_obj },
    { MP_ROM_QSTR(MP_QSTR_circle), (mp_obj_t) &turtle_circle_obj },
    { MP_ROM_QSTR(MP_QSTR_disque), (mp_obj_t) &turtle_disque_obj },
    { MP_ROM_QSTR(MP_QSTR_dot), (mp_obj_t) &turtle_dot_obj },
    { MP_ROM_QSTR(MP_QSTR_rectangle_plein), (mp_obj_t) &turtle_rectangle_plein_obj },
    { MP_ROM_QSTR(MP_QSTR_triangle_plein), (mp_obj_t) &turtle_triangle_plein_obj },
    { MP_ROM_QSTR(MP_QSTR_setheading), (mp_obj_t) &turtle_setheading_obj },
    { MP_ROM_QSTR(MP_QSTR_goto), (mp_obj_t) &turtle_setposition_obj },
    { MP_ROM_QSTR(MP_QSTR_seth), (mp_obj_t) &turtle_setheading_obj },
    { MP_ROM_QSTR(MP_QSTR_setposition), (mp_obj_t) &turtle_setposition_obj },
    { MP_ROM_QSTR(MP_QSTR_towards), (mp_obj_t) &turtle_towards_obj },
    { MP_ROM_QSTR(MP_QSTR_setpos), (mp_obj_t) &turtle_setposition_obj },
    { MP_ROM_QSTR(MP_QSTR_setx), (mp_obj_t) &turtle_setx_obj },
    { MP_ROM_QSTR(MP_QSTR_sety), (mp_obj_t) &turtle_sety_obj },
    { MP_ROM_QSTR(MP_QSTR_write), (mp_obj_t) &turtle_write_obj },
    { MP_ROM_QSTR(MP_QSTR_speed), (mp_obj_t) &turtle_speed_obj },
    { MP_ROM_QSTR(MP_QSTR_fillcolor), (mp_obj_t) &turtle_fillcolor_obj },
    { MP_ROM_QSTR(MP_QSTR_end_fill), (mp_obj_t) &turtle_end_fill_obj },
    { MP_ROM_QSTR(MP_QSTR_begin_fill), (mp_obj_t) &turtle_begin_fill_obj },
    { MP_ROM_QSTR(MP_QSTR_showturtle), (mp_obj_t) &turtle_showturtle_obj },
    { MP_ROM_QSTR(MP_QSTR_st), (mp_obj_t) &turtle_showturtle_obj },
    { MP_ROM_QSTR(MP_QSTR_hideturtle), (mp_obj_t) &turtle_hideturtle_obj },
    { MP_ROM_QSTR(MP_QSTR_ht), (mp_obj_t) &turtle_hideturtle_obj },
    { MP_ROM_QSTR(MP_QSTR_down), (mp_obj_t) &turtle_down_obj },
    { MP_ROM_QSTR(MP_QSTR_saute), (mp_obj_t) &turtle_saute_obj },
    { MP_ROM_QSTR(MP_QSTR_pas_de_cote), (mp_obj_t) &turtle_pas_de_cote_obj },
    { MP_ROM_QSTR(MP_QSTR_pendown), (mp_obj_t) &turtle_down_obj },
    { MP_ROM_QSTR(MP_QSTR_pd), (mp_obj_t) &turtle_down_obj },
    { MP_ROM_QSTR(MP_QSTR_up), (mp_obj_t) &turtle_up_obj },
    { MP_ROM_QSTR(MP_QSTR_penup), (mp_obj_t) &turtle_up_obj },
    { MP_ROM_QSTR(MP_QSTR_pu), (mp_obj_t) &turtle_up_obj },
    { MP_ROM_QSTR(MP_QSTR_pencolor), (mp_obj_t) &turtle_pencolor_obj },
	{ MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t) &turtle_pencolor_obj },
    { MP_ROM_QSTR(MP_QSTR_colormode), (mp_obj_t) &turtle_colormode_obj },
	{ MP_ROM_QSTR(MP_QSTR_avance), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_av), (mp_obj_t) &turtle_forward_obj },
	{ MP_ROM_QSTR(MP_QSTR_recule), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_re), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_back), (mp_obj_t) &turtle_backward_obj },
	{ MP_ROM_QSTR(MP_QSTR_tourne_gauche), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_tg), (mp_obj_t) &turtle_left_obj },
	{ MP_ROM_QSTR(MP_QSTR_tourne_droite), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_td), (mp_obj_t) &turtle_right_obj },
	{ MP_ROM_QSTR(MP_QSTR_rond), (mp_obj_t) &turtle_circle_obj },
	{ MP_ROM_QSTR(MP_QSTR_efface), (mp_obj_t) &turtle_reset_obj },
	{ MP_ROM_QSTR(MP_QSTR_cap), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_heading), (mp_obj_t) &turtle_setheading_obj },
	{ MP_ROM_QSTR(MP_QSTR_position), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_pos), (mp_obj_t) &turtle_setposition_obj },
	{ MP_ROM_QSTR(MP_QSTR_ecris), (mp_obj_t) &turtle_write_obj },
	{ MP_ROM_QSTR(MP_QSTR_vitesse_tortue), (mp_obj_t) &turtle_speed_obj },
	{ MP_ROM_QSTR(MP_QSTR_montre_tortue), (mp_obj_t) &turtle_showturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_cache_tortue), (mp_obj_t) &turtle_hideturtle_obj },
	{ MP_ROM_QSTR(MP_QSTR_baisse_crayon), (mp_obj_t) &turtle_down_obj },
	{ MP_ROM_QSTR(MP_QSTR_leve_crayon), (mp_obj_t) &turtle_up_obj },
	{ MP_ROM_QSTR(MP_QSTR_pensize), (mp_obj_t) &turtle_pensize_obj },
	{ MP_ROM_QSTR(MP_QSTR_width), (mp_obj_t) &turtle_pensize_obj },
};

STATIC const mp_obj_dict_t mp_module_turtle_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_turtle_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_turtle_globals_table),
        .table = (mp_map_elem_t*)mp_module_turtle_globals_table,
    },
};

const mp_obj_module_t mp_module_turtle = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_turtle_globals,
};

/* MATPLOTL */
static mp_obj_t matplotl_show(size_t n_args, const mp_obj_t *args) {
  const char * val=caseval("show()");
#if defined EMCC && !defined NO_QSTR
  EM_ASM_ARGS({
      var msg = UTF8ToString($0);
      UI.python_output += msg;
    },val);
#endif
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_show_obj, 0, 0, matplotl_show);

static mp_obj_t matplotl_clf(size_t n_args, const mp_obj_t *args) {
  const char * val=caseval("erase()");
  return mp_obj_new_str(val,strlen(val));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_clf_obj, 0, 0, matplotl_clf);

static mp_obj_t matplotl_axis(size_t n_args, const mp_obj_t *args) {
  if (n_args==0)
    turtle_ret("axis: Autoscale");
  double *x=0; size_t n=0,m=0;
  if (mp_array2doubletab(args[0],&x,&n,&m)){
    char * ptr=printtab(x,n,m);
    free(x);
    char buf[strlen(ptr)+256];
    sprintf(buf,"axis(%s):;",ptr);
    free(ptr);
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  if (n_args==4){
    double a=0,b=0,c=0,d=0;
    if (mp_obj_is_float(args[0]))
      a=mp_obj_get_float(args[0]);
    if (MP_OBJ_IS_INT(args[0]))
      a=mp_obj_get_int(args[0]);
    if (mp_obj_is_float(args[1]))
      b=mp_obj_get_float(args[1]);
    if (MP_OBJ_IS_INT(args[1]))
      b=mp_obj_get_int(args[1]);
    if (mp_obj_is_float(args[2]))
      c=mp_obj_get_float(args[2]);
    if (MP_OBJ_IS_INT(args[2]))
      c=mp_obj_get_int(args[2]);
    if (mp_obj_is_float(args[3]))
      d=mp_obj_get_float(args[3]);
    if (MP_OBJ_IS_INT(args[3]))
      d=mp_obj_get_int(args[3]);
    if (a<b && c<d){
      char buf[256]="";
#ifdef NUMWORKS
      strcat(buf,"axis(");
      strcat_double(buf,a);
      strcat(buf,",");
      strcat_double(buf,b);
      strcat(buf,",");
      strcat_double(buf,c);
      strcat(buf,",");
      strcat_double(buf,d);
      strcat(buf,"):;");
#else      
      sprintf(buf,"axis(%.14g,%.14g,%.14g,%.14g):;",a,b,c,d);
#endif
      const char * val=caseval(buf);
      return turtle_ret(val);
    }
  }
  return turtle_ret("axis: Bad argument type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_axis_obj, 0, 4, matplotl_axis);

static mp_obj_t matplotl_text(size_t n_args, const mp_obj_t *args) {
  double a=0,b=0;
  if (mp_obj_is_float(args[0]))
    a=mp_obj_get_float(args[0]);
  if (MP_OBJ_IS_INT(args[0]))
    a=mp_obj_get_int(args[0]);
  if (mp_obj_is_float(args[1]))
    b=mp_obj_get_float(args[1]);
  if (MP_OBJ_IS_INT(args[1]))
    b=mp_obj_get_int(args[1]);
  if (MP_OBJ_IS_STR(args[2])){
    char buf[256]="";
#ifdef NUMWORKS
    strcat(buf,"legend(point(");
    strcat_double(buf,a);
    strcat(buf,",");
    strcat_double(buf,b);
    char buf2[64];
    sprintf(buf2,"),%s):;",mp_obj_str_get_str(args[2]));
    strcat(buf,buf2);
#else      
    sprintf(buf,"legend(point(%.14g,%.14g),%s):;",a,b,mp_obj_str_get_str(args[2]));
#endif
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  return turtle_ret("text: Bad argument type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_text_obj, 3, 3, matplotl_text);

static mp_obj_t matplotl_plot(size_t n_args, const mp_obj_t *args) {
  double *x=0,*y=0; size_t n=0,m=0,n1=0,m1=0;
  int col=0;
  if (n_args>=3)
    col=mp_get_color(args[2]);
  if (mp_array2doubletab(args[0],&x,&n,&m)){ 
    char * ptrx=printtab(x,n,m);
    free(x); 
    char bufx[strlen(ptrx)+1];
    strcpy(bufx,ptrx);
    free(ptrx);
    if (mp_array2doubletab(args[1],&y,&n1,&m1)){
      char * ptry=printtab(y,n1,m1);
      free(y);
      char bufy[strlen(ptry)+1];
      strcpy(bufy,ptry);
      free(ptry);
      if (n1==n && m==0 && m1==0){
	char buf2[strlen(bufx)+strlen(bufy)+256];
	sprintf(buf2,"polygonplot(%s,%s,color=%i):;",bufx,bufy,col>=0?col:0);
	const char * val=caseval(buf2);
	return turtle_ret(val);
      }
    }
  }
  double xx=0,yy=0;
  if (n_args>=2 && mp_int_float(args[0],&xx) && mp_int_float(args[1],&yy)){
    char buf[512]="";
#ifdef NUMWORKS
    strcat(buf,"legend(point(");
    strcat_double(buf,xx);
    strcat(buf,",");
    strcat_double(buf,yy);
#else      
    sprintf(buf,"point(%.14g,%.14g",xx,yy);
#endif
    if (col==-1)
      sprintf(buf,",display=\"%s\"):;",translate_point_type(mp_obj_str_get_str(args[2])));
    else
      sprintf(buf,",color=%i):;",col);
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  return turtle_ret("polygonplot: Bad argument type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_plot_obj, 2, 3, matplotl_plot);

static mp_obj_t matplotl_bar_hist(size_t n_args, const mp_obj_t *args,bool bar) {
  double *x=0; size_t n=0,m=0;
  double largeur=0.8;
  if (mp_array2doubletab(args[0],&x,&n,&m)){
    char * ptr=printtab(x,n,m);
    free(x);
    char buf[strlen(ptr)+256];
    if (n_args==1)
      sprintf(buf,bar?"barplot(%s):;":"histogram(%s):;",ptr);
    else
      sprintf(buf,bar?"barplot(%s":"histogram(%s",ptr);
    free(ptr);
    if (n_args>=2){
      if (mp_obj_is_float(args[1]))
	largeur=mp_obj_get_float(args[1]);
      if (n_args==3 && mp_obj_is_float(args[2]))
	largeur=mp_obj_get_float(args[2]);
      if (mp_array2doubletab(args[1],&x,&n,&m)){
	ptr=printtab(x,n,m);
	free(x);
	char buf2[strlen(buf)+strlen(ptr)+256];
#ifdef NUMWORKS
	sprintf(buf2,"%s,%s,",buf,ptr);
	strcat_double(buf2,largeur);
	strcat(buf2,"):;");
#else
	sprintf(buf2,"%s,%s,%.3g):;",buf,ptr,largeur);
#endif
	free(ptr);
	const char * val=caseval(buf2);
	return turtle_ret(val);
      }
      char buf2[strlen(buf)+256];
#ifdef NUMWORKS
      sprintf(buf2,"%s,",buf);
      strcat_double(buf2,largeur);
      strcat(buf2,"):;");
#else
      sprintf(buf2,"%s,%.3g):;",buf,largeur);
#endif
      const char * val=caseval(buf2);
      return turtle_ret(val);
    }
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  return turtle_ret(bar?"barplot: Bad argument type":"histogram: Bad argument type");
}
static mp_obj_t matplotl_bar(size_t n_args, const mp_obj_t *args) {
  return matplotl_bar_hist(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_bar_obj, 1, 3, matplotl_bar);

static mp_obj_t matplotl_hist(size_t n_args, const mp_obj_t *args) {
  return matplotl_bar_hist(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_hist_obj, 1, 3, matplotl_hist);

static mp_obj_t matplotl_boxplot(size_t n_args, const mp_obj_t *args) {
  double *x=0; size_t n=0,m=0;
  int col=0;
  if (n_args==2)
    col=mp_get_color(args[1]);
  if (mp_array2doubletab(args[0],&x,&n,&m)){
    char * ptr=printtab(x,n,m);
    free(x);
    char buf[strlen(ptr)+256];
    sprintf(buf,"moustache(%s,color=%i):;",ptr,col>=0?col:0);
    free(ptr);
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  return turtle_ret("moustache: Bad argument type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_boxplot_obj, 1, 2, matplotl_boxplot);

static mp_obj_t matplotl_arrow(size_t n_args, const mp_obj_t *args) {
  double a=0,b=0,c=0,d=0;
  int col=0;
  if (n_args==5)
    col=mp_get_color(args[4]);
  if (mp_int_float(args[0],&a) && mp_int_float(args[1],&b) &&
      mp_int_float(args[2],&c) && mp_int_float(args[3],&d)){
    char buf[512]="";
#ifdef NUMWORKS
    strcat(buf,"vector([");
    strcat_double(buf,a);
    strcat(buf,",");
    strcat_double(buf,b);
    strcat(buf,"],[");
    strcat_double(buf,c);
    strcat(buf,",");
    strcat_double(buf,d);
    char buf2[64];
    sprintf(buf2,"],color=%i):;",col);
    strcat(buf,buf2);
#else          
    sprintf(buf,"vector([%.14g,%.14g],[%.14g,%.14g],color=%i):;",a,b,c,d,col);
#endif
    const char * val=caseval(buf);
    return turtle_ret(val);
  }
  return turtle_ret("vector: Bad argument type");
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_arrow_obj, 4, 5, matplotl_arrow);

static mp_obj_t matplotl_grid(size_t n_args, const mp_obj_t *args) {
  bool b=true;
  if (n_args==1 && MP_OBJ_IS_SMALL_INT(args[0]) && MP_OBJ_SMALL_INT_VALUE(args[0])==0)
    b=false;
  char buf[64];
  sprintf(buf,"axes=%i",b?1:0);
  const char * val=caseval(buf);
  return turtle_ret(val);  
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_grid_obj, 0, 1, matplotl_grid);

static mp_obj_t matplotl_scatter_reg(size_t n_args, const mp_obj_t *args,bool scatter) {
  int col=0;
  if (n_args==3)
    col=mp_get_color(args[2]);
  double *x=0,*y=0; size_t n=0,m=0,n1=0,m1=0;
  if (mp_array2doubletab(args[0],&x,&n,&m)){ 
    char * ptrx=printtab(x,n,m);
    free(x); 
    char bufx[strlen(ptrx)+1];
    strcpy(bufx,ptrx);
    free(ptrx);
    if (mp_array2doubletab(args[1],&y,&n1,&m1)){
      char * ptry=printtab(y,n1,m1);
      free(y);
      char bufy[strlen(ptry)+1];
      strcpy(bufy,ptry);
      free(ptry);
      if (n1==n && m==0 && m1==0){
	char buf2[strlen(bufx)+strlen(bufy)+256];
	if (scatter)
	  sprintf(buf2,"scatterplot(%s,%s,color=%i):;",bufx,bufy,col);
	else
	  sprintf(buf2,"linear_regression_plot(%s,%s,color=%i):;",bufx,bufy,col);
	const char * val=caseval(buf2);
	return turtle_ret(val);
      }
    }
  }
  return turtle_ret("scatterplot: Bad argument type");
}
static mp_obj_t matplotl_scatter(size_t n_args, const mp_obj_t *args) {
  return matplotl_scatter_reg(n_args,args,true);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_scatter_obj, 2, 3, matplotl_scatter);
static mp_obj_t matplotl_linear_regression_plot(size_t n_args, const mp_obj_t *args) {
  return matplotl_scatter_reg(n_args,args,false);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(matplotl_linear_regression_plot_obj, 2, 3, matplotl_linear_regression_plot);

//
static const mp_map_elem_t matplotl_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_show), (mp_obj_t) &matplotl_show_obj },
	{ MP_ROM_QSTR(MP_QSTR_clf), (mp_obj_t) &matplotl_clf_obj },
	{ MP_ROM_QSTR(MP_QSTR_axis), (mp_obj_t) &matplotl_axis_obj },
	{ MP_ROM_QSTR(MP_QSTR_text), (mp_obj_t) &matplotl_text_obj },
	{ MP_ROM_QSTR(MP_QSTR_plot), (mp_obj_t) &matplotl_plot_obj },
	{ MP_ROM_QSTR(MP_QSTR_bar), (mp_obj_t) &matplotl_bar_obj },
	{ MP_ROM_QSTR(MP_QSTR_hist), (mp_obj_t) &matplotl_hist_obj },
	{ MP_ROM_QSTR(MP_QSTR_histogram), (mp_obj_t) &matplotl_hist_obj },
	{ MP_ROM_QSTR(MP_QSTR_boxplot), (mp_obj_t) &matplotl_boxplot_obj },
	{ MP_ROM_QSTR(MP_QSTR_boxwhisker), (mp_obj_t) &matplotl_boxplot_obj },
	{ MP_ROM_QSTR(MP_QSTR_arrow), (mp_obj_t) &matplotl_arrow_obj },
	{ MP_ROM_QSTR(MP_QSTR_grid), (mp_obj_t) &matplotl_grid_obj },
	{ MP_ROM_QSTR(MP_QSTR_vector), (mp_obj_t) &matplotl_arrow_obj },
	{ MP_ROM_QSTR(MP_QSTR_barplot), (mp_obj_t) &matplotl_bar_obj },
	{ MP_ROM_QSTR(MP_QSTR_scatter), (mp_obj_t) &matplotl_scatter_obj },
	{ MP_ROM_QSTR(MP_QSTR_linear_regression_plot), (mp_obj_t) &matplotl_linear_regression_plot_obj },
	{ MP_ROM_QSTR(MP_QSTR_scatterplot), (mp_obj_t) &matplotl_scatter_obj },
};


static MP_DEFINE_CONST_DICT(matplotl_locals_dict, matplotl_locals_dict_table);

const mp_obj_type_t matplotl_type = {
    { &mp_type_type },
    .name = MP_QSTR_matplotl,
    .locals_dict = (mp_obj_t)&matplotl_locals_dict
};


STATIC const mp_map_elem_t mp_module_matplotl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__matplotl) },
    { MP_ROM_QSTR(MP_QSTR_show), (mp_obj_t) &matplotl_show_obj },
    { MP_ROM_QSTR(MP_QSTR_clf), (mp_obj_t) &matplotl_clf_obj },
    { MP_ROM_QSTR(MP_QSTR_axis), (mp_obj_t) &matplotl_axis_obj },
    { MP_ROM_QSTR(MP_QSTR_text), (mp_obj_t) &matplotl_text_obj },
    { MP_ROM_QSTR(MP_QSTR_plot), (mp_obj_t) &matplotl_plot_obj },
    { MP_ROM_QSTR(MP_QSTR_boxplot), (mp_obj_t) &matplotl_boxplot_obj },
    { MP_ROM_QSTR(MP_QSTR_boxwhisker), (mp_obj_t) &matplotl_boxplot_obj },
    { MP_ROM_QSTR(MP_QSTR_arrow), (mp_obj_t) &matplotl_arrow_obj },
    { MP_ROM_QSTR(MP_QSTR_grid), (mp_obj_t) &matplotl_grid_obj },
    { MP_ROM_QSTR(MP_QSTR_vector), (mp_obj_t) &matplotl_arrow_obj },
    { MP_ROM_QSTR(MP_QSTR_bar), (mp_obj_t) &matplotl_bar_obj },
    { MP_ROM_QSTR(MP_QSTR_hist), (mp_obj_t) &matplotl_hist_obj },
    { MP_ROM_QSTR(MP_QSTR_histogram), (mp_obj_t) &matplotl_hist_obj },
    { MP_ROM_QSTR(MP_QSTR_barplot), (mp_obj_t) &matplotl_bar_obj },
    { MP_ROM_QSTR(MP_QSTR_scatter), (mp_obj_t) &matplotl_scatter_obj },
    { MP_ROM_QSTR(MP_QSTR_linear_regression_plot), (mp_obj_t) &matplotl_linear_regression_plot_obj },
    { MP_ROM_QSTR(MP_QSTR_scatterplot), (mp_obj_t) &matplotl_scatter_obj },
};

STATIC const mp_obj_dict_t mp_module_matplotl_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .is_fixed = 1,
        .used = MP_ARRAY_SIZE(mp_module_matplotl_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_matplotl_globals_table),
        .table = (mp_map_elem_t*)mp_module_matplotl_globals_table,
    },
};

const mp_obj_module_t mp_module_matplotl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_matplotl_globals,
};

