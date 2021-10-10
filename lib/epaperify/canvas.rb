module Epaperify
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

    def draw_text(font, text)
        text.render_string(self, text)
    end
  end
end