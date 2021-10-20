#include <ruby.h>
#include "extconf.h"
#include <stdlib.h>
#include <signal.h>
#include "EPD_2in7b_V2.h"
#include "EPD_5in83b_V2.h"
#include "EPD_5in83_V2.h"
#include "EPD_IT8951.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include <time.h>

#ifndef _EPAPERIFY_h_
#define _EPAPERIFY_h_

#define EPD_2IN7B_V2 0
#define EPD_5IN83B_V2 1
#define EPD_5IN83_V2 2
#define EPD_IT8951 3

typedef struct epaper_image_buffer {
    UWORD width;
    UWORD height;
    UWORD rotate;
    UWORD color;
    UWORD Bcolor, Wcolor;
    UBYTE *image;
} EPAPER_IMAGE_BUFFER;

typedef struct epaper_model_interface {
   void (*init_func)();
   void (*init_func2)(void *canvas, UWORD *vcom);
   void (*display)(UBYTE *Image);
   void (*display_rb)(UBYTE *Imageblack, UBYTE *Imagered);
   void (*new_image)(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
   void (*sleep)(void);
} epaper_interface;

typedef struct epaper_canvas {
    UBYTE *black_image;
    UBYTE *red_image;
    int width;
    int height;
    uint16_t color;
    uint16_t background_color;
    int font_size;
    UWORD Xcursor;
    UWORD Ycursor;
    int bpp;
    int rotation;
    epaper_interface interface;
    TEXT_OPTIONS text_options;
    void *misc;
} ecanvas;

typedef struct epaper_font {
    SFT sft;
    double ascender;
    double descender;
    double linegap;
    int scale;
} efont;

typedef struct epaper_font_render {
    efont *font;
    unsigned int width;
    unsigned int height;
    unsigned int yoffset;
    double advance_width;
    double leftside_bearing;
    char *render_buffer;
} efont_render;

sFONT *size_to_font(UWORD font_size);

#endif