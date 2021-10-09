# frozen_string_literal: true

require_relative "epaperify/version"

module Epaperify
  VERSION = "0.2"
  DISPLAY_EPD_2IN7B_V2 = 0
  DISPLAY_EPD_5IN83B_V2 = 1
  DISPLAY_EPD_5IN83_V2 = 2

  class Error < StandardError; end

  # stubs for auto complete
  class PaperCanvas
    def initialize(model, rotation); end
    def draw_string(xval, yval, message);end
    def render_font_buffer(xcoord, ycoord, font_buffer, color_plane); end
  end

  class Font
    def intiialize(scale, font_path); end
    def render_font(code_point);end
  end

  class FontRender
    def to_a;end
    def width;end
    def height;end
  end
end

require "epaperify/epaperify"
require "epaperify/text"