#include "canvas.h"

VALUE initialize(VALUE self, VALUE model, VALUE rotation) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    printf("allocating canvas");
    DEV_Module_Init();
    int bitsperpixel = 1;
    canvas->rotation = NUM2INT(rotation);

    // Add new model support here!
    switch(NUM2INT(model)) {
        case EPD_2IN7B_V2:
           canvas->interface.init_func = &EPD_2IN7B_V2_Init;
           canvas->interface.display_rb = &EPD_2IN7B_V2_Display;
           canvas->interface.sleep = &EPD_2IN7B_V2_Sleep;
           canvas->bpp = 1;
           canvas->width = EPD_2IN7B_V2_WIDTH;
           canvas->height = EPD_2IN7B_V2_HEIGHT;
           break;
        case EPD_5IN83B_V2:
           canvas->interface.init_func = &EPD_5IN83B_V2_Init;
           canvas->interface.display_rb = &EPD_5IN83B_V2_Display;
           canvas->interface.sleep = &EPD_5IN83B_V2_Sleep;
           canvas->width = EPD_5IN83B_V2_WIDTH;
           canvas->height = EPD_5IN83B_V2_HEIGHT;
           break;
        case EPD_5IN83_V2:
           canvas->interface.init_func = &EPD_5in83_V2_Init;
           canvas->interface.display = &EPD_5in83_V2_Display;
           canvas->interface.sleep = &EPD_5in83_V2_Sleep;
           canvas->bpp = 1;
           canvas->width = EPD_5in83_V2_WIDTH;
           canvas->height = EPD_5in83_V2_HEIGHT;
           break;
    }

    canvas->interface.init_func();
    UWORD Imagesize = ((canvas->width % 8 == 0)? ( canvas->width / 8 ): ( canvas->width / 8 + 1)) * canvas->height;
    if((canvas->black_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }

    if((canvas->red_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }

    Paint_NewImage(canvas->black_image, canvas->width , canvas->height, canvas->rotation, WHITE);
    Paint_NewImage(canvas->red_image, canvas->width, canvas->height, canvas->rotation, WHITE);
    Paint_SelectImage(canvas->black_image);
    Paint_Clear(WHITE);
    Paint_SelectImage(canvas->red_image);
    Paint_Clear(WHITE);
    return Qnil;
}

VALUE allocate(VALUE klass) {
    ecanvas *canvas = (ecanvas*) malloc(sizeof(ecanvas));
    memset(canvas, 0, sizeof(ecanvas));

    canvas->color = BLACK;
    canvas->background_color = WHITE;
    canvas->font_size = 12;
    canvas->Xcursor = 0;
    canvas->Ycursor = 0;
    canvas->black_image = NULL;
    canvas->red_image = NULL;
    canvas->text_options.tabstops = 4;
    canvas->text_options.split_on_word_boundary = 1;
    canvas->text_options.strip_leading_spaces = 1;

    return Data_Wrap_Struct(klass, 0, free_canvas, canvas);
}

VALUE draw_string(VALUE self, VALUE xval, VALUE yval, VALUE message) {
    ecanvas *canvas;

    CURSOR cursor;
    char *our_c_string = StringValueCStr(message);

    Data_Get_Struct(self, ecanvas, canvas);
    int x = NUM2INT(xval);
    int y = NUM2INT(yval);

    paint(canvas, x, y, our_c_string, &cursor);
    return Qnil;
}

VALUE render_pixels(VALUE self, VALUE xcoord, VALUE ycoord, VALUE rb_arr, VALUE color_plane) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);

    int x = NUM2INT(xcoord);
    int y = NUM2INT(ycoord);
    int plane = NUM2INT(color_plane);
    if (color_plane == 0) {
      Paint_SelectImage(canvas->black_image);
    } else {
      Paint_SelectImage(canvas->red_image);
    }

    int cols_length = RARRAY_LEN(rb_arr);
    for (int i = 0; i < cols_length; i++) {
        VALUE cols_arr = rb_ary_entry(rb_arr, i);
        VALUE rows_length = RARRAY_LEN(cols_arr);
        for(int i2 = 0; i2 < rows_length; i2++) {
            VALUE rb_color = rb_ary_entry(cols_arr, i2);
            int color = NUM2INT(rb_color) > 0 ? BLACK : WHITE;
            Paint_SetPixel(x + i, y + i2, color);
        }
    }
    return Qnil;
}

VALUE render_font_buffer(VALUE self, VALUE xcoord, VALUE ycoord, VALUE rb_font_buffer, VALUE color_plane) {
    ecanvas *canvas;
    efont_render *render_buffer;
    Data_Get_Struct(self, ecanvas, canvas);
    Data_Get_Struct(rb_font_buffer, efont_render, render_buffer);

    int x = NUM2INT(xcoord);
    int y = NUM2INT(ycoord);
    int plane = NUM2INT(color_plane);
    if (color_plane == 0) {
      Paint_SelectImage(canvas->black_image);
    } else {
      Paint_SelectImage(canvas->red_image);
    }

    for (int i = 0; i < render_buffer->height; i++) {
        for(int i2 = 0; i2 < render_buffer->width; i2++) {
            int color = NUM2INT(render_buffer->render_buffer[i2*render_buffer->width + i]) > 0 ? BLACK : WHITE;
            Paint_SetPixel(x + i, y + i2, color);
        }
    }
    return Qnil;
}