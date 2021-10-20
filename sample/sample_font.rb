require "bundler/setup"
require "epaperify"
require 'pry-byebug'

canvas = Epaperify::PaperCanvas.new(Epaperify::Devices::DISPLAY_EPD_2IN7B_V2, 270, 0)

_font = Epaperify::Font.new(12, "FiraGO-Regular.ttf")
font2 = Epaperify::Font.new(12, "orange juice 2.0.ttf")

font3 = Epaperify::Font.new(12, "Admiration Pains.ttf")
text = Epaperify::Text.new(font2)
text.string = "Hello World!\nI hope everyone is safe!\n"
width, height = text.measure canvas
text.render_string canvas, y: (canvas.height / 2 - height / 2)

canvas.show
canvas.sleep