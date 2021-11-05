#!/usr/bin/ruby


require 'epaperify'
canvas = Epaperify::PaperCanvas.new(Epaperify::Devices::EPD_IT8951, 270, 2270)
motivational_string = `/usr/local/bin/motivate --no-colors`
PADDING = 50

font = Epaperify::Font.new(32, "/home/pi/lora.ttf")
text = Epaperify::Text.new(font)
text.line_padding 20
text.string= motivational_string
width, height =  text.measure canvas
text.render_string canvas, y: canvas.height/2 - height/2, left_margin: PADDING, top_margin: PADDING, right_margin: PADDING, line_padding: 40

dateStr =  Time.now.strftime('%A, %B %d %Y')

font2 = Epaperify::Font.new(24, "/home/pi/lato.ttf")
text2 = Epaperify::Text.new(font2)
text2.string = dateStr
width, _ = text2.measure canvas
text2.render_string canvas, x: canvas.width / 2 - width / 2, y: PADDING

canvas.show
canvas.sleep
