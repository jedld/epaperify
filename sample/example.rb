require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_5IN83_V2, 90)
canvas.font_size = 24
bitmap = Epaperify::ImageBuffer.new("sample/sample.bmp")
canvas.draw_bitmap(bitmap, 0, 0)
puts canvas.measure "Hello World!"
canvas.print " Hello World!\r\nHello World!\n\n"
canvas.draw_string(10,10, "Manual text position")
canvas.show
canvas.sleep