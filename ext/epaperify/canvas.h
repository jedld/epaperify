#ifndef _CANVAS_h_
#define _CANVAS_h_

#include "epaperify.h"

VALUE allocate(VALUE klass);
void free_canvas(ecanvas *canvas);
VALUE initialize(VALUE self, VALUE model, VALUE rotation, VALUE extra);
VALUE draw_string(VALUE self, VALUE xval, VALUE yval, VALUE message);
VALUE render_pixels(VALUE self, VALUE xcoord, VALUE ycoord, VALUE rb_arr, VALUE color_plane);
VALUE render_font_buffer(VALUE self, VALUE xcoord, VALUE ycoord, VALUE rb_font_buffer, VALUE color_plane);

#endif