#include "canvas.h"

void EPD_IT8951_Init2(ecanvas *canvas, UWORD VCOM) {
    IT8951_Dev_Info info = EPD_IT8951_Init(VCOM);
    printf("w %d h %d", info.Panel_W, info.Panel_H);
    canvas->width = info.Panel_W;
    canvas->height = info.Panel_H;

    IT8951_Canvas_Info *canvas_info = (IT8951_Canvas_Info*)malloc(sizeof(IT8951_Canvas_Info));
    memset(canvas_info, 0, sizeof(IT8951_Canvas_Info));

    canvas->misc = canvas_info;

    canvas_info->target_memory_addr = info.Memory_Addr_L | (info.Memory_Addr_H << 16);
    char* LUT_Version = (char*)info.LUT_Version;

    if( strcmp(LUT_Version, "M641") == 0 ){
        //6inch e-Paper HAT(800,600), 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        canvas_info->A2_Mode = 4;
        canvas_info->Four_Byte_Align = true;
    }else if(strcmp(LUT_Version, "M841_TFAB512") == 0 ){
        //Another firmware version for 6inch HD e-Paper HAT(1448,1072), 6inch HD touch e-Paper HAT(1448,1072)
        canvas_info->A2_Mode = 6;
        canvas_info->Four_Byte_Align = true;
    }else if( strcmp(LUT_Version, "M841") == 0 ){
        //9.7inch e-Paper HAT(1200,825)
        canvas_info->A2_Mode = 6;
    }else if( strcmp(LUT_Version, "M841_TFA2812") == 0 ){
        //7.8inch e-Paper HAT(1872,1404)
        canvas_info->A2_Mode = 6;
    }else if( strcmp(LUT_Version, "M841_TFA5210") == 0 ){
        //10.3inch e-Paper HAT(1872,1404)
        canvas_info->A2_Mode = 6;
    }else{
        //default set to 6 as A2 Mode
        canvas_info->A2_Mode = 6;
    }
    printf("version: %s", LUT_Version);
    EPD_IT8951_Clear_Refresh(info, canvas_info->target_memory_addr, 0);
}

VALUE initialize(VALUE self, VALUE model, VALUE rotation, VALUE extra) {
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    printf("allocating canvas");

    int bitsperpixel = 1;
    canvas->rotation = NUM2INT(rotation);
    UWORD extra_param =  NUM2INT(extra);

    // Add new model support here!
    switch(NUM2INT(model)) {
        case EPD_2IN7B_V2:
           canvas->interface.bcm2835_init = &DEV_Module_Init;
           canvas->interface.init_func = &EPD_2IN7B_V2_Init;
           canvas->interface.display_rb = &EPD_2IN7B_V2_Display;
           canvas->interface.sleep = &EPD_2IN7B_V2_Sleep;
           canvas->interface.new_image = &Paint_NewImage;
           canvas->interface.set_pixel = &Paint_SetPixel;
           canvas->interface.clear = &Paint_Clear;
           canvas->bpp = 1;
           canvas->width = EPD_2IN7B_V2_WIDTH;
           canvas->height = EPD_2IN7B_V2_HEIGHT;
           break;
        case EPD_5IN83B_V2:
           canvas->interface.bcm2835_init = &DEV_Module_Init;
           canvas->interface.init_func = &EPD_5IN83B_V2_Init;
           canvas->interface.display_rb = &EPD_5IN83B_V2_Display;
           canvas->interface.sleep = &EPD_5IN83B_V2_Sleep;
           canvas->interface.new_image = &Paint_NewImage;
           canvas->interface.set_pixel = &Paint_SetPixel;
           canvas->interface.clear = &Paint_Clear;
           canvas->width = EPD_5IN83B_V2_WIDTH;
           canvas->height = EPD_5IN83B_V2_HEIGHT;
           break;
        case EPD_5IN83_V2:
           canvas->interface.bcm2835_init = &DEV_Module_Init;
           canvas->interface.init_func = &EPD_5in83_V2_Init;
           canvas->interface.display = &EPD_5in83_V2_Display;
           canvas->interface.sleep = &EPD_5in83_V2_Sleep;
           canvas->interface.new_image = &Paint_NewImage;
           canvas->interface.set_pixel = &Paint_SetPixel;
           canvas->interface.clear = &Paint_Clear;
           canvas->bpp = 1;
           canvas->width = EPD_5in83_V2_WIDTH;
           canvas->height = EPD_5in83_V2_HEIGHT;
           break;
        case EPD_IT8951:
           canvas->interface.bcm2835_init = &DEV_Module_Init_IT8951;
           canvas->interface.display2 = &EPD_IT8951_4bp_Refresh;
           canvas->interface.init_func2 = &EPD_IT8951_Init2;
           canvas->interface.sleep = &EPD_IT8951_Sleep;
           canvas->interface.new_image = &Paint_NewImage2;
           canvas->interface.set_pixel = &Paint_SetPixel2;
           canvas->interface.clear = &Paint_Clear2;
           canvas->bpp = 4;
           break;
        default:
           printf("unknown !!! %d", NUM2INT(model));
           return Qnil;
    }
    
    canvas->interface.bcm2835_init();

    if (canvas->interface.init_func != NULL ) {
        printf("calling init func");
        canvas->interface.init_func();
    } else {
        printf("calling init func 2");
        canvas->interface.init_func2(canvas, extra_param);
    }

    UWORD Imagesize = 0;
    if (canvas->bpp = 1) {
        Imagesize = ((canvas->width % 8 == 0)? ( canvas->width / 8 ): ( canvas->width / 8 + 1)) * canvas->height;
    } else {
        Imagesize = ((canvas->width * canvas->bpp % 8 == 0)? (canvas->width * canvas->bpp / 8 ): (canvas->width * canvas->bpp / 8 + 1)) * canvas->height;
    }
    
    if((canvas->black_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }

    if((canvas->red_image = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        return -1;
    }

    canvas->interface.new_image(canvas->black_image, canvas->width , canvas->height, canvas->rotation, WHITE);
    canvas->interface.new_image(canvas->red_image, canvas->width, canvas->height, canvas->rotation, WHITE);
    Paint_SelectImage(canvas->black_image);
    canvas->interface.clear(WHITE);
    Paint_SelectImage(canvas->red_image);
    canvas->interface.clear(WHITE);
    return Qnil;
}

void free_canvas(ecanvas *canvas) {
    printf("canvas free");
    if (canvas->black_image!=NULL) free(canvas->black_image);
    if (canvas->red_image!=NULL) free(canvas->red_image);
    if (canvas->misc!=NULL) free(canvas->misc);
    free(canvas);
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
            canvas->interface.set_pixel(x + i, y + i2, color);
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
    if (plane == 0) {
      Paint_SelectImage(canvas->black_image);
    } else {
      Paint_SelectImage(canvas->red_image);
    }
    UBYTE SixteenColorPattern[16] = {0xFF,0xEE,0xDD,0xCC,0xBB,0xAA,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};
    for (int i = 0; i < render_buffer->height; i++) {
        for(int i2 = 0; i2 < render_buffer->width; i2++) {
            int color = 0;
            if (canvas->bpp == 1) {
                color = render_buffer->render_buffer[i*render_buffer->width + i2] > 100 ? BLACK : WHITE;
            } else {
                int index = (render_buffer->render_buffer[i*render_buffer->width + i2] % 256) / 16 ;
                color = SixteenColorPattern[index];
            }
            canvas->interface.set_pixel(x + i2, y + i, color);
        }
    }
    return Qnil;
}
