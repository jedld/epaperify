module Epaperify
  class Text
    def initialize(font, canvas)
      @font = font
      @canvas = canvas
      @font_cache = {}
    end

    def render_string(str)
      x_point = @canvas.x_cursor
      y_point = @canvas.y_cursor

      str.each_char do |c|
        if (c == ' ')
          x_point += 15
          next
        end
        @font_cache[c.ord] ||= begin
          @font.render_font(c.ord)
        end

        font = @font_cache[c.ord]
        puts font.yoffset

        @canvas.render_font_buffer(x_point, y_point + font.yoffset, font, 0)
        x_point+= font.width
      end
    end
  end
end