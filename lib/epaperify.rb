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
    def color=(color);end
    def show; end
    def color;end
    def background_color=(color);end
    def background_color;end
    def sleep;end
    def clear;end
    def print(string);end
    def x_cursor=(xpos);end
    def y_cursor=(ypos);end
    
    def render_font_buffer(xcoord, ycoord, font_buffer, color_plane); end
  end

  class Font
    def intiialize(scale, font_path); end
    def render_font(code_point);end
    def linegap; end;
    def descender; end;
    def ascender; end;
    def scale; end;
  end

  class FontRender
    def to_a;end
    def width;end
    def height;end
    def yoffset;end
    def advance_width;end
    def leftside_bearing;end
  end
end

require "epaperify/epaperify"
require "epaperify/text"