require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_2IN7B_V2, 270)

font = Epaperify::Font.new(8, "FiraGO-Regular.ttf")
font2 = Epaperify::Font.new(8, "ubuntu.ttf")

canvas.y_cursor = 12
text = Epaperify::Text.new(font)
text.render_string canvas, "Hello World!"

canvas.show
canvas.sleep