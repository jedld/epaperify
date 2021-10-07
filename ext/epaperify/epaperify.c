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
            Paint_SetPixel(NUM2INT(startx) + x, NUM2INT(starty) + y, color);
        }
    }
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

VALUE string_metrics(VALUE self, VALUE astring) {
    UWORD Xpoint = 0;
    UWORD Ypoint = 0;
    ecanvas *canvas;
    Data_Get_Struct(self, ecanvas, canvas);
    char *pString = StringValueCStr(astring);
    sFONT *font = size_to_font(canvas->font_size);
    
    if (font == NULL) return Qnil;
    while (* pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + font->Width ) > Paint.Width ) {
            Xpoint = 0;
            Ypoint += font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint  + font->Height ) > Paint.Height ) {
            Xpoint = 0;
            Ypoint = 0;
        }

        if (*pString == '\n') {
            Ypoint += font->Height;
            pString ++;
            continue;
        }

        if (*pString == '\r') {
            Xpoint = 0;
            pString ++;
            continue;
        }

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += font->Width;
    }

    VALUE hash = rb_hash_new();
    rb_hash_aset(hash, rb_str_new2("width"), INT2FIX(Xpoint));
    rb_hash_aset(hash, rb_str_new2("height"), INT2FIX(Ypoint + font->Height));
    return hash;
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
        
    return Data_Wrap_Struct(klass, 0, free_canvas, canvas);
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
    UWORD Imagesize = (( canvas->width % 8 == 0)? ( canvas->width / 8 ): ( canvas->width / 8 + 1)) * canvas->height;
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

void free_canvas(ecanvas *canvas) {
    printf("canvas free");
    if (canvas->black_image!=NULL) free(canvas->black_image);
    if (canvas->red_image!=NULL) free(canvas->red_image);
    free(canvas);
}

void Init_epaperify() {
    VALUE canvasKlass;
    VALUE imageBuffer;
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
    rb_define_method(canvasKlass, "draw_bitmap", draw_bitmap, 3);
    rb_define_method(canvasKlass, "read_bmp", read_bitmap_direct, 3);
    rb_define_method(canvasKlass, "measure", string_metrics, 1);
    rb_define_method(canvasKlass, "text_options", text_options, 1);

    //Image Buffer
    imageBuffer = rb_define_class_under(mod, "ImageBuffer", rb_cObject);
    rb_define_alloc_func(imageBuffer, allocate_image_buffer);
    rb_define_method(imageBuffer, "initialize", initialize_image_buffer, 1);
    rb_define_method(imageBuffer, "read_bmp", read_bitmap, 3);
}
