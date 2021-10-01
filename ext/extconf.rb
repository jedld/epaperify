require "mkmf"

$LOCAL_LIBS << '-lbcm2835 -lm '

create_header
create_makefile "epaper/epaper"