require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_2IN7B_V2, 270)

font = Epaperify::Font.new(8, "FiraGO-Regular.ttf")
font2 = Epaperify::Font.new(8, "ubuntu.ttf")
rendered_font = font.render_font('A'.ord)
rendered_font2 = font2.render_font('A'.ord)
puts rendered_font.to_a.inspect
canvas.print "hello"
canvas.render_font_buffer(10, 10, rendered_font, 0)
canvas.render_font_buffer(80, 10, rendered_font2, 0)
canvas.show
canvas.sleep