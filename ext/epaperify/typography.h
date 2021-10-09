#ifndef _TYPOGRAPHYY_h_
#define _TYPOGRAPHYY_h_

#include "epaperify.h"

void allocate_font(VALUE klass);
VALUE initialize_font(VALUE self, int scale, VALUE font_path);
VALUE render_font(VALUE sfont, VALUE codepoint);
void allocate_font_render(VALUE klass);
void free_font_render(efont_render *font_render);
VALUE render_font_to_a(VALUE self);
VALUE font_render_width(VALUE self);
VALUE font_render_height(VALUE self);
VALUE font_render_yoffset(VALUE self);
VALUE font_render_advance_width(VALUE self);
VALUE font_render_leftside_bearing(VALUE self);


VALUE font_linegap(VALUE self);
VALUE font_descender(VALUE self);
VALUE font_ascender(VALUE self);
VALUE font_scale(VALUE self);

#endif