#include "epaperify.h"

/* void EPD_2IN7_V2_Init(void)
{
    UBYTE *BlackImage, *RedImage;
    UWORD Imagesize = ((EPD_2IN7B_V2_WIDTH % 8 == 0)? (EPD_2IN7B_V2_WIDTH / 8 ): (EPD_2IN7B_V2_WIDTH / 8 + 1)) * EPD_2IN7B_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    if((RedImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }
    printf("NewImage:BlackImage and RedImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN7B_V2_WIDTH, EPD_2IN7B_V2_HEIGHT, 270, WHITE);
    Paint_NewImage(RedImage, EPD_2IN7B_V2_WIDTH, EPD_2IN7B_V2_HEIGHT, 270, WHITE);

    //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);

    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 110, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

    //2.Draw red image
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);
    Paint_DrawCircle(160, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(210, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);

    printf("EPD_Display\r\n");
    EPD_2IN7B_V2_Display(BlackImage, RedImage);
    EPD_2IN7B_V2_Sleep();
    free(BlackImage);
    free(RedImage);
    BlackImage = NULL;
    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    printf("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit(); 
} */

VALUE clear() {
    EPD_2IN7B_V2_Clear();
    DEV_Delay_ms(500);
    return Qnil;
}

VALUE reset(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    Paint_SelectImage(canvas->black_image);
    Paint_Clear(WHITE);
    Paint_SelectImage(canvas->red_image);
    Paint_Clear(WHITE);
    return Qnil;
}

VALUE init_paper() {
     printf("init epaper");
     if(DEV_Module_Init()!=0){
        return -1;
    }
    EPD_2IN7B_V2_Init();
}



void free_canvas(ecanvas *canvas);

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

    switch(canvas->font_size) {
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
    }


    Paint_DrawString_EN(x, y, message, font, background_color, color, cursor);
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

VALUE show(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
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
    return INT2NUM(canvas->height);
}

VALUE get_width(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    return INT2NUM(canvas->width);
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
    
    printf("canvas allocated");

    return Data_Wrap_Struct(klass, 0, free_canvas, canvas);
}

VALUE initialize(VALUE self, VALUE model, VALUE rotation) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    printf("allocating canvas");
    DEV_Module_Init();

    switch(NUM2INT(model)) {
        case EPD_2IN7B_V2:
           canvas->interface.init_func = &EPD_2IN7B_V2_Init;
           canvas->interface.display_rb = &EPD_2IN7B_V2_Display;
           canvas->interface.sleep = &EPD_2IN7B_V2_Sleep;
           canvas->width = EPD_2IN7B_V2_WIDTH;
           canvas->height = EPD_2IN7B_V2_HEIGHT;
           break;
        case EPD_5IN83_V2:
           canvas->interface.init_func = &EPD_5in83_V2_Init;
           canvas->interface.display = &EPD_5in83_V2_Display;
           canvas->interface.sleep = &EPD_5in83_V2_Sleep;
           canvas->width = EPD_5in83_V2_WIDTH;
           canvas->height = EPD_5in83_V2_HEIGHT;
           break;
    }

    canvas->interface.init_func();
    UWORD Imagesize = (( canvas->width % 8 == 0)? ( canvas->width / 8 ): ( canvas->width / 8 + 1)) * canvas->height;
    if((canvas->black_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }

    if((canvas->red_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }

    Paint_NewImage(canvas->black_image, canvas->width , canvas->height, NUM2INT(rotation), WHITE);
    Paint_NewImage(canvas->red_image, canvas->width, canvas->height, NUM2INT(rotation), WHITE);
    Paint_SelectImage(canvas->black_image);
    Paint_Clear(WHITE);
    Paint_SelectImage(canvas->red_image);
    Paint_Clear(WHITE);
    return Qnil;
}

void free_canvas(ecanvas *canvas) {
    printf("canvas free");
    if (canvas->black_image!=NULL) free(canvas->black_image);
    if (canvas->red_image!=NULL) free(canvas->red_image);
    free(canvas);
}

void Init_epaperify() {
    VALUE canvasKlass;
    VALUE mod = rb_define_module("Epaperify");
    canvasKlass = rb_define_class_under(mod, "PaperCanvas", rb_cObject);
    rb_define_alloc_func(canvasKlass, allocate);
    rb_define_method(canvasKlass, "initialize", initialize, 2);
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
}
