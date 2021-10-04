require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_2IN7B_V2, 90)
puts canvas.measure "Hello World!"
canvas.print "Hello World!\r\n\tHello World!\r\n"
canvas.show
canvas.sleep