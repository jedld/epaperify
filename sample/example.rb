require "bundler/setup"
require "epaperify"

canvas = Epaperify::PaperCanvas.new(Epaperify::DISPLAY_EPD_5IN83_V2, 90)
canvas.font_size = 16
canvas.print `motivate --no-color`
canvas.show
canvas.sleep
