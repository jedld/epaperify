require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_2IN7B_V2, 270)

font = Epaperify::Font.new(12, "FiraGO-Regular.ttf")
font2 = Epaperify::Font.new(12, "ubuntu.ttf")
text = Epaperify::Text.new(font2)
text.render_string canvas, "Hello World!\nI hope everyone is safe!\n"

canvas.show
canvas.sleep