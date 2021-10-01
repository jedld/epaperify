require "mkmf"

$LOCAL_LIBS << '-lbcm2835 -lm '

$CFLAGS += " -D USE_BCM2835_LIB -D RPI"


create_header
create_makefile "epaperify/epaperify"