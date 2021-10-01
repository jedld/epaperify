#include <ruby.h>

#include "extconf.h"
#include <stdlib.h>
#include <signal.h> 
#include "EPD_2in7b_V2.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include <time.h> 

void EPD_2IN7_V2_Init(void)
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
    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_CN(130, 20, "΢ѩ����", &Font24CN, WHITE, BLACK);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);

    //2.Draw red image
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);
    Paint_DrawCircle(160, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(210, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawString_CN(130, 0,"���abc", &Font12CN, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
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
}

VALUE clear() {
    EPD_2IN7B_V2_Clear();
    DEV_Delay_ms(500);
}

VALUE init_paper() {
     printf("init epaper");
     if(DEV_Module_Init()!=0){
        return -1;
    }
    EPD_2IN7B_V2_Init();
}


typedef struct epaper_canvas {
    UBYTE *black_image;
    UBYTE *red_image;
    int width;
    int height;
} ecanvas;

void free_canvas(ecanvas *canvas);


VALUE draw_string(VALUE self, VALUE message) {
    ecanvas *canvas;
    char *our_c_string = StringValueCStr(message);
    printf("got message ");

    Data_Get_Struct(canvas, ecanvas, self);
    Paint_SelectImage(canvas->black_image);
    Paint_SelectImage(canvas->red_image);
    Paint_DrawString_EN(10, 20, our_c_string, &Font12, WHITE, BLACK);
    return Qnil;
}

VALUE show(VALUE self) {
    ecanvas *canvas;
    Data_Get_Struct(canvas, ecanvas, self);
    EPD_2IN7B_V2_Display(canvas->black_image, canvas->red_image);
    return Qnil;
}

static VALUE create_canvas(VALUE klass) {
    ecanvas *canvas = (ecanvas*) malloc(sizeof(ecanvas));
    canvas->width = EPD_2IN7B_V2_WIDTH;
    canvas->height = EPD_2IN7B_V2_HEIGHT;
    UWORD Imagesize = (( canvas->width % 8 == 0)? ( canvas->width / 8 ): ( canvas->width / 8 + 1)) * canvas->height;
    if((canvas->black_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }

    if((canvas->red_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }
    printf("canvas allocated");
    return Data_Wrap_Struct(klass, 0, free_canvas, canvas);
}

void free_canvas(ecanvas *canvas) {
    printf("canvas free");
    free(canvas->black_image);
    free(canvas->red_image);
    free(canvas);
}

void Init_epaperify() {
    VALUE canvasKlass;
    
    VALUE mod = rb_define_module("Epaperify");
    canvasKlass = rb_define_class_under(mod, "Canvas", rb_cObject);
    rb_define_alloc_func(canvasKlass, create_canvas);
    rb_define_method(canvasKlass, "init_paper", init_paper, 0);
    rb_define_method(canvasKlass, "draw_string", draw_string, 1);
    rb_define_method(canvasKlass, "show", show, 0);
}