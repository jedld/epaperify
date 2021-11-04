#include "epaperify.h"
#include "canvas.h"
#include "typography.h"

VALUE clear() {
    EPD_2IN7B_V2_Clear();
    DEV_Delay_ms(500);
    return Qnil;
}

VALUE reset(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    Paint_SelectImage(canvas->black_image);
    canvas->interface.clear(WHITE);
    Paint_SelectImage(canvas->red_image);
    canvas->interface.clear(WHITE);
    return Qnil;
}

VALUE init_paper() {
     printf("init epaper");
     if(DEV_Module_Init()!=0){
        return -1;
    }
    EPD_2IN7B_V2_Init();
}

void paint(ecanvas *canvas, int x, int y, char *message, CURSOR *cursor) {
    sFONT *font;
    uint16_t color;
    uint16_t background_color;

    if (canvas->color < 4) {
       Paint_SelectImage(canvas->black_image);
       color = canvas->color;
    } else {
       Paint_SelectImage(canvas->red_image);
       color = BLACK;
    }

    if (canvas->background_color != WHITE) {
       background_color = canvas->background_color;
    } else {
       background_color = WHITE;
    }

    font = size_to_font(canvas->font_size);
    Paint_DrawString_EN(x, y, message, font, background_color, color, cursor, canvas->text_options);
}

sFONT *size_to_font(UWORD font_size) {
    sFONT *font;
    switch(font_size) {
        case 8:
          font = &Font8;
          break;
        case 12:
          font = &Font12;
          break;
        case 16:
          font = &Font16;
          break;
        case 20:
          font = &Font20;
          break;
        case 24:
          font = &Font24;
          break;
        default:
          printf("unknown font!");
          return NULL;
    }
    return font;
}


VALUE set_display_window(VALUE self, VALUE xstart, VALUE ystart, VALUE xend, VALUE yend) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    EPD_2IN7B_V2_SetWindows(NUM2INT(xstart), NUM2INT(ystart), NUM2INT(xend), NUM2INT(yend));
    return Qnil;
}

VALUE print(VALUE self, VALUE message) {
    ecanvas *canvas;
    CURSOR cursor;
    char *our_c_string = StringValueCStr(message);
    Data_Get_Struct(self, ecanvas, canvas);
    paint(canvas, canvas->Xcursor, canvas->Ycursor, our_c_string, &cursor);
    canvas->Xcursor = cursor.Xcursor;
    canvas->Ycursor = cursor.Ycursor;
    return Qnil;
}

VALUE draw_bitmap(VALUE self, VALUE image_buffer, VALUE startx, VALUE starty) {
    ecanvas *canvas;
    EPAPER_IMAGE_BUFFER *buffer;
    UBYTE color, temp;

    Data_Get_Struct(self, ecanvas, canvas);
    Data_Get_Struct(image_buffer, EPAPER_IMAGE_BUFFER, buffer);
    Paint_SelectImage(canvas->black_image);

    UWORD Image_Width_Byte = (buffer->width % 8 == 0)? (buffer->width / 8): (buffer->width / 8 + 1);
    // Read image data into the cache
    UWORD x, y;
    for(y = 0; y < buffer->height; y++) {
        for(x = 0; x < buffer->width; x++) {
            if(x > Paint.Width || y > Paint.Height) {
                break;
            }
            temp = buffer->image[(x / 8) + (y * Image_Width_Byte)];
            color = (((temp << (x%8)) & 0x80) == 0x80) ? buffer->Bcolor : buffer->Wcolor;
            canvas->interface.set_pixel(NUM2INT(startx) + x, NUM2INT(starty) + y, color);
        }
    }
    return Qnil;
}


VALUE show(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    if (canvas->interface.display2 != NULL) {
        IT8951_Canvas_Info *info = (IT8951_Canvas_Info*)canvas->misc;
        canvas->interface.display2(canvas->black_image, 0, 0, canvas->width, canvas->height, false, info->target_memory_addr, false);
    } else
    if (canvas->interface.display != NULL) {
        canvas->interface.display(canvas->black_image);
    } else {
        canvas->interface.display_rb(canvas->black_image, canvas->red_image);
    }
    return Qnil;
}

VALUE color(VALUE self, VALUE color) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->color = NUM2INT(color);
    return Qnil;
}

VALUE background_color(VALUE self, VALUE color) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->background_color = NUM2INT(color);
    return Qnil;
}

VALUE font_size(VALUE self, VALUE size) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->font_size = NUM2INT(size);
    return Qnil;
}

VALUE xcursor(VALUE self, VALUE xvalue) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->Xcursor =  NUM2INT(xvalue);
    return Qnil;
}

VALUE ycursor(VALUE self, VALUE yvalue) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->Ycursor =  NUM2INT(yvalue);
    return Qnil;
}

VALUE get_xcursor(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->Xcursor);
}

VALUE get_ycursor(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->Ycursor);
}

VALUE get_height(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(Paint.Height);
}

VALUE get_width(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);

    return INT2NUM(Paint.Width);
}

VALUE get_font_size(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->font_size);
}

VALUE get_color(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->color);
}

VALUE get_background_color(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->background_color);
}

VALUE screen_sleep(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    canvas->interface.sleep();
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
    return Qnil;
}

VALUE string_metrics(VALUE self, VALUE astring) {
    ecanvas *canvas;
    WORD_BOUNDARY *curword = NULL, *word_head = NULL;
    int index = 0;

    Data_Get_Struct(self, ecanvas, canvas);
    char *pString = StringValueCStr(astring);
    sFONT *font = size_to_font(canvas->font_size);
    TEXT_OPTIONS text_options = canvas->text_options;
    CURSOR cursor;

    draw_string_handler(0, 0, pString,
                         font, canvas->color, canvas->background_color, &cursor, text_options,
                         NULL);
    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, rb_str_new2("width"), INT2FIX(cursor.Xcursor - text_options.margin_left));
    rb_hash_aset(hash, rb_str_new2("height"), INT2FIX(cursor.Ycursor + font->Height - text_options.margin_top ));
    return hash;
}

VALUE text_options(VALUE self, VALUE settings) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);

    VALUE keys_v = rb_funcall(settings, rb_intern("keys"), 0);
    int length = RARRAY_LEN(keys_v);
    for (int i = 0; i < length; i++) {
        VALUE k = rb_ary_entry(keys_v, i);
        char *c_key = StringValueCStr(k);
        int val = NUM2INT(rb_hash_aref(settings, k));
        if (strcasecmp(c_key, "margin_left") == 0) {
            canvas->text_options.margin_left = val;
        } else
        if (strcasecmp(c_key, "margin_right") == 0) {
            canvas->text_options.margin_right = val;
        } else
        if (strcasecmp(c_key, "margin_bottom") == 0) {
            canvas->text_options.margin_bottom = val;
        } else
        if (strcasecmp(c_key, "margin_top") == 0) {
            canvas->text_options.margin_top = val;
        } else
        if (strcasecmp(c_key, "line_padding") == 0) {
            canvas->text_options.line_padding = val;
        } else
        if (strcasecmp(c_key, "tabstops") == 0) {
            canvas->text_options.tabstops = val;
        } else
        if (strcasecmp(c_key, "strip_leading_spaces") == 0) {
            canvas->text_options.strip_leading_spaces = val;
        } else
        if (strcasecmp(c_key, "split_on_word_boundary") == 0) {
            canvas->text_options.split_on_word_boundary = val;
        }
    }
    return Qnil;
}

void free_image_buffer(EPAPER_IMAGE_BUFFER *buffer) {
    if (buffer->image != NULL) {
        free(buffer->image);
    }
    free(buffer);
}

VALUE allocate_image_buffer(VALUE klass) {
    EPAPER_IMAGE_BUFFER *image_buffer = (EPAPER_IMAGE_BUFFER*)malloc(sizeof(EPAPER_IMAGE_BUFFER));
    memset(image_buffer, 0, sizeof(EPAPER_IMAGE_BUFFER));
    return Data_Wrap_Struct(klass, 0, free_image_buffer, image_buffer);
}


VALUE initialize_image_buffer(VALUE self, VALUE filename) {
    EPAPER_IMAGE_BUFFER *image_buffer;
    Data_Get_Struct(self, EPAPER_IMAGE_BUFFER, image_buffer);
    FILE *fp;                     //Define a file pointer
    BMPFILEHEADER bmpFileHeader;  //Define a bmp file header structure
    BMPINFOHEADER bmpInfoHeader;  //Define a bmp info header structure
  // Binary file open
    char *path = StringValueCStr(filename);
    if((fp = fopen(path, "rb")) == NULL) {
        Debug("Cann't open the file!\n");
        exit(0);
    }

    // Set the file pointer from the beginning
    fseek(fp, 0, SEEK_SET);
    fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp);    //sizeof(BMPFILEHEADER) must be 14
    fread(&bmpInfoHeader, sizeof(BMPINFOHEADER), 1, fp);    //sizeof(BMPFILEHEADER) must be 50
    printf("pixel = %d * %d\r\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);

    UWORD Image_Width_Byte = (bmpInfoHeader.biWidth % 8 == 0)? (bmpInfoHeader.biWidth / 8): (bmpInfoHeader.biWidth / 8 + 1);
    UWORD Bmp_Width_Byte = (Image_Width_Byte % 4 == 0) ? Image_Width_Byte: ((Image_Width_Byte / 4 + 1) * 4);
    image_buffer->image = (UBYTE*)malloc(Image_Width_Byte * bmpInfoHeader.biHeight);
    memset(image_buffer->image, 0xFF, Image_Width_Byte * bmpInfoHeader.biHeight);

    // Determine if it is a monochrome bitmap
    int readbyte = bmpInfoHeader.biBitCount;
    if(readbyte != 1) {
        Debug("the bmp Image is not a monochrome bitmap!\n");
        exit(0);
    }

    // Determine black and white based on the palette
    UWORD i;
    UWORD Bcolor, Wcolor;
    UWORD bmprgbquadsize = pow(2, bmpInfoHeader.biBitCount);// 2^1 = 2
    BMPRGBQUAD bmprgbquad[bmprgbquadsize];        //palette
    // BMPRGBQUAD bmprgbquad[2];        //palette

    for(i = 0; i < bmprgbquadsize; i++){
    // for(i = 0; i < 2; i++) {
        fread(&bmprgbquad[i], sizeof(BMPRGBQUAD), 1, fp);
    }
    if(bmprgbquad[0].rgbBlue == 0xff && bmprgbquad[0].rgbGreen == 0xff && bmprgbquad[0].rgbRed == 0xff) {
        Bcolor = BLACK;
        Wcolor = WHITE;
    } else {
        Bcolor = WHITE;
        Wcolor = BLACK;
    }

    // Read image data into the cache
    UWORD x, y;
    UBYTE Rdata;
    fseek(fp, bmpFileHeader.bOffset, SEEK_SET);
    for(y = 0; y < bmpInfoHeader.biHeight; y++) {//Total display column
        for(x = 0; x < Bmp_Width_Byte; x++) {//Show a line in the line
            if(fread((char *)&Rdata, 1, readbyte, fp) != readbyte) {
                perror("get bmpdata:\r\n");
                break;
            }
            if(x < Image_Width_Byte) { //bmp
                image_buffer->image[x + (bmpInfoHeader.biHeight - y - 1) * Image_Width_Byte] =  Rdata;
                // printf("rdata = %d\r\n", Rdata);
            }
        }
    }
    fclose(fp);

    image_buffer->height = bmpInfoHeader.biHeight;
    image_buffer->width = bmpInfoHeader.biWidth;
    image_buffer->Bcolor = Bcolor;
    image_buffer->Wcolor = Wcolor;

    return Qnil;
}

VALUE read_bitmap(VALUE self, VALUE filename, VALUE xstart, VALUE ystart) {
    EPAPER_IMAGE_BUFFER *image_buffer;
    Data_Get_Struct(self, EPAPER_IMAGE_BUFFER, image_buffer);
    Paint_SelectImage(image_buffer->image);
    char *path = StringValueCStr(filename);
    GUI_ReadBmp(path, NUM2INT(xstart), NUM2INT(ystart));
    return Qnil;
}

VALUE read_bitmap_direct(VALUE self, VALUE filename, VALUE xstart, VALUE ystart) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    Paint_SelectImage(canvas->black_image);
    char *path = StringValueCStr(filename);
    GUI_ReadBmp(path, NUM2INT(xstart), NUM2INT(ystart));
    return Qnil;
}


VALUE read_bitmap_4g(VALUE self, VALUE filename, VALUE xstart, VALUE ystart) {
    EPAPER_IMAGE_BUFFER *image_buffer;
    Data_Get_Struct(self, EPAPER_IMAGE_BUFFER, image_buffer);
    Paint_SelectImage(image_buffer->image);
    char *path = StringValueCStr(filename);
    GUI_ReadBmp_4Gray(path, NUM2INT(xstart), NUM2INT(ystart));
    return Qnil;
}


void Init_epaperify() {
    VALUE mod = rb_define_module("Epaperify");
    VALUE canvasKlass = rb_define_class_under(mod, "PaperCanvas", rb_cObject);
    rb_define_alloc_func(canvasKlass, allocate);
    rb_define_method(canvasKlass, "initialize", initialize, 3);
    rb_define_method(canvasKlass, "init_paper", init_paper, 0);
    rb_define_method(canvasKlass, "draw_string", draw_string, 3);
    rb_define_method(canvasKlass, "font_size=", font_size, 1);
    rb_define_method(canvasKlass, "font_size", get_font_size, 0);
    rb_define_method(canvasKlass, "show", show, 0);
    rb_define_method(canvasKlass, "color=", color, 1);
    rb_define_method(canvasKlass, "color", get_color, 0);
    rb_define_method(canvasKlass, "background_color=", background_color, 1);
    rb_define_method(canvasKlass, "background_color", get_background_color, 0);
    rb_define_method(canvasKlass, "sleep", screen_sleep, 0);
    rb_define_method(canvasKlass, "clear", reset, 0);
    rb_define_method(canvasKlass, "print", print, 1);
    rb_define_method(canvasKlass, "x_cursor=", xcursor, 1);
    rb_define_method(canvasKlass, "y_cursor=", ycursor, 1);
    rb_define_method(canvasKlass, "x_cursor", get_xcursor, 0);
    rb_define_method(canvasKlass, "y_cursor", get_ycursor, 0);
    rb_define_method(canvasKlass, "height", get_height, 0);
    rb_define_method(canvasKlass, "width", get_width, 0);
    rb_define_method(canvasKlass, "set_window", set_display_window, 4);
    rb_define_method(canvasKlass, "draw_bitmap", draw_bitmap, 3);
    rb_define_method(canvasKlass, "read_bmp", read_bitmap_direct, 3);
    rb_define_method(canvasKlass, "measure", string_metrics, 1);
    rb_define_method(canvasKlass, "text_options", text_options, 1);
    rb_define_method(canvasKlass, "render_pixels", render_pixels, 4);
    rb_define_method(canvasKlass, "render_font_buffer", render_font_buffer, 4);

    //Image Buffer
    VALUE imageBuffer = rb_define_class_under(mod, "ImageBuffer", rb_cObject);
    rb_define_alloc_func(imageBuffer, allocate_image_buffer);
    rb_define_method(imageBuffer, "initialize", initialize_image_buffer, 1);
    rb_define_method(imageBuffer, "read_bmp", read_bitmap, 3);

    VALUE fontRender = rb_define_class_under(mod, "FontRender", rb_cObject);
    rb_define_alloc_func(fontRender, allocate_font_render);
    rb_define_method(fontRender, "to_a", render_font_to_a, 0);
    rb_define_method(fontRender, "width", font_render_width, 0);
    rb_define_method(fontRender, "height", font_render_height, 0);
    rb_define_method(fontRender, "yoffset", font_render_yoffset, 0);
    rb_define_method(fontRender, "advance_width", font_render_advance_width, 0);
    rb_define_method(fontRender, "leftside_bearing", font_render_leftside_bearing, 0);

    VALUE fontKlass = rb_define_class_under(mod, "Font", rb_cObject);
    rb_define_alloc_func(fontKlass, allocate_font);
    rb_define_method(fontKlass, "initialize", initialize_font, 2);
    rb_define_method(fontKlass, "render_font", render_font, 1);
    rb_define_method(fontKlass, "linegap", font_linegap, 0);
    rb_define_method(fontKlass, "descender", font_descender, 0);
    rb_define_method(fontKlass, "ascender", font_ascender, 0);
    rb_define_method(fontKlass, "scale", font_scale, 0);
}
