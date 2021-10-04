#include <ruby.h>
#include "extconf.h"
#include <stdlib.h>
#include <signal.h> 
#include "EPD_2in7b_V2.h"
#include "EPD_5in83b_V2.h"
#include "EPD_5in83_V2.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include <time.h> 

#ifndef _EPAPERIFY_h_
#define _EPAPERIFY_h_

#define EPD_2IN7B_V2 0
#define EPD_5IN83B_V2 1
#define EPD_5IN83_V2 2

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
   void (*display)(UBYTE *Image);
   void (*display_rb)(UBYTE *Imageblack, UBYTE *Imagered);
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
} ecanvas;

sFONT *size_to_font(UWORD font_size);

#endif