module Epaperify
  class Text
    def initialize(font)
      @font = font
      @font_cache = {}
    end

    def render_string(canvas, str)
      x_point = canvas.x_cursor
      y_point = canvas.y_cursor

      str.each_char do |c|
        @font_cache[c.ord] ||= begin
          @font.render_font(c.ord)
        end

        font_render = @font_cache[c.ord]
        puts font_render.yoffset

        canvas.render_font_buffer(x_point + font_render.leftside_bearing, y_point + font_render.yoffset, font_render, 0)
        x_point += font_render.advance_width
      end
    end
  end
end