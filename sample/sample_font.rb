require "bundler/setup"
require "epaperify"
require 'pry-byebug'

canvas = Epaperify::PaperCanvas.new(Epaperify::Devices::EPD_IT8951, 0, 2270)
puts "canvas size #{canvas.width}, #{canvas.height}"
font2 = Epaperify::Font.new(12, "orange juice 2.0.ttf")
text = Epaperify::Text.new(font2)
text.string = "Hello World!\nI hope everyone is safe!\n"
width, height = text.measure canvas
text.render_string canvas, y: (canvas.height / 2 - height / 2)

canvas.show
canvas.sleep