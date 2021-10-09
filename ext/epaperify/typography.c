#include "typography.h"

void allocate_font(VALUE klass) {
    efont *font = (efont*) malloc(sizeof(efont));
    memset(font, 0, sizeof(efont));
    return Data_Wrap_Struct(klass, 0, free, font);
}

VALUE font_ascender(VALUE self) {
  efont *font;
  Data_Get_Struct(self, efont, font);
  return DBL2NUM(font->ascender);
}

VALUE font_descender(VALUE self) {
  efont *font;
  Data_Get_Struct(self, efont, font);
  return DBL2NUM(font->descender);
}

VALUE font_linegap(VALUE self) {
  efont *font;
  Data_Get_Struct(self, efont, font);
  return DBL2NUM(font->linegap);
}

void allocate_font_render(VALUE klass) {
    efont_render *font_render = (efont_render*) malloc(sizeof(efont_render));
    memset(font_render, 0, sizeof(efont_render));
    return Data_Wrap_Struct(klass, 0, free_font_render, font_render);
}

void free_font_render(efont_render *font_render) {
    if (font_render->render_buffer != NULL) free(font_render->render_buffer);
    free(font_render);
}

VALUE font_render_width(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);
  return INT2NUM(font_render->width);
}

VALUE font_render_height(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);
  return INT2NUM(font_render->height);
}

VALUE font_render_yoffset(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);
  return INT2NUM(font_render->yoffset);
}

VALUE font_render_advance_width(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);
  return INT2NUM(font_render->advance_width);
}

VALUE font_render_leftside_bearing(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);
  return INT2NUM(font_render->leftside_bearing);
}

VALUE initialize_font(VALUE self, int scale, VALUE font_path) {
    efont *font;
    Data_Get_Struct(self, efont, font);
    char *path = StringValueCStr(font_path);
    font->sft.xScale = scale;
    font->sft.yScale = scale;
    font->sft.flags = SFT_DOWNWARD_Y;
	  font->sft.font = sft_loadfile(path);

    SFT_LMetrics lmtx;
    sft_lmetrics(&font->sft, &lmtx);

    font->ascender = lmtx.ascender;
    font->descender = lmtx.descender;
    font->linegap = lmtx.lineGap;

	  if (font->sft.font == NULL)
		 printf("TTF load failed");
    return Qnil;
}

VALUE render_font(VALUE sfont, VALUE codepoint) {
    efont *font;
    efont_render *font_render;

    Data_Get_Struct(sfont, efont, font);

    SFT_Glyph gid;  //  unsigned long gid;
    int cp = NUM2INT(codepoint);

    VALUE module = rb_const_get(rb_cObject, rb_intern("Epaperify"));
    VALUE font_render_klass = rb_const_get(module, rb_intern("FontRender"));
    VALUE rb_font_render = rb_funcall(font_render_klass, rb_intern("new"), 0);
    Data_Get_Struct(rb_font_render, efont_render, font_render);


    if (sft_lookup(&font->sft, cp, &gid) < 0)
      printf("missing %d", cp);

    SFT_GMetrics mtx;
    if (sft_gmetrics(&font->sft, gid, &mtx) < 0)
      printf("bad glyph metrics %d", cp);

      SFT_Image img = {
      .width  = (mtx.minWidth + 3) & ~3,
      .height = mtx.minHeight,
    };


    font_render->render_buffer = (char*)malloc(img.width * img.height);
    font_render->height = img.height;
    font_render->width = img.width;
    font_render->yoffset = mtx.yOffset;
    font_render->advance_width = mtx.advanceWidth;
    font_render->leftside_bearing = mtx.leftSideBearing;

    img.pixels = (void*)font_render->render_buffer;
    if (sft_render(&font->sft, gid, img) < 0)
      printf("not rendered %d", cp);

    return rb_font_render;
}

VALUE render_font_to_a(VALUE self) {
  efont_render *font_render;
  Data_Get_Struct(self, efont_render, font_render);

  VALUE columns = rb_ary_new_capa(font_render->width);
    for(int i = 0; i < font_render->width; i++) {
        VALUE rows = rb_ary_new_capa(font_render->height);
        for(int i2 = 0; i2 < font_render->height; i2++) {
              rb_ary_push(rows, INT2NUM(font_render->render_buffer[i2 * font_render->width + i]));
        }
        rb_ary_push(columns, rows);
    }
  return columns;
}
