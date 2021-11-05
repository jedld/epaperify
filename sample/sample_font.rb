require "bundler/setup"
require "epaperify"
require 'pry-byebug'

canvas = Epaperify::PaperCanvas.new(Epaperify::Devices::EPD_IT8951, 270, 2270)
puts "canvas size #{canvas.width}, #{canvas.height}"
font2 = Epaperify::Font.new(36, "ubuntu.ttf")
text = Epaperify::Text.new(font2)
text.string = "Hello World!\nI hope everyone is safe!\n"
width, height = text.measure canvas
text.render_string canvas

canvas.show
canvas.sleep
binding.pry


puts "done."
