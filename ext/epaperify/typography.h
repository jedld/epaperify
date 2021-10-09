#ifndef _TYPOGRAPHYY_h_
#define _TYPOGRAPHYY_h_

#include "epaperify.h"

void allocate_font(VALUE klass);
VALUE initialize_font(VALUE self, int scale, VALUE font_path);
VALUE render_font(VALUE sfont, VALUE codepoint);
void allocate_font_render(VALUE klass);
void free_font_render(efont_render *font_render);
VALUE render_font_to_a(VALUE self);

#endif