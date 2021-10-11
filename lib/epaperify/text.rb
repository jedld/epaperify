module Epaperify
  class Text
    def initialize(font)
      @font = font
      @font_cache = {}
      @line_padding = 5
    end

    def line_padding(padding)
      @line_padding = padding
    end

    def render_string(canvas, str, options = {})
      max_width = options.fetch(:width, canvas.width)
      left_margin = options.fetch(:left_margin, 0)
      right_margin = options.fetch(:right_margin, 0)
      top_margin = options.fetch(:top_margin, 0)

      y_advance = (@font.ascender + @font.descender + @font.linegap)

      x_point = canvas.x_cursor + left_margin
      y_point = canvas.y_cursor + y_advance + top_margin
      
      @font_cache[' '.ord] ||= begin
                                 @font.render_font(' '.ord)
                               end
      space_width = @font_cache[' '.ord].advance_width

      str.each_char do |c|
        @font_cache[c.ord] ||= begin
          @font.render_font(c.ord)
        end
      end

      inside_word = false
      word_boundaries = {}
      current_boundary = nil

      str.each_char.with_index do |c, index|
        if is_word_char?(c)
          font_render = @font_cache[c.ord]
          if !inside_word
            current_boundary = {
              character_count: 1,
              width: font_render.advance_width
            }

            word_boundaries[index] = current_boundary
          else
            current_boundary[:character_count] += 1
            current_boundary[:width] += font_render.advance_width
          end
          inside_word = true
        elsif inside_word
          # measure word width
          inside_word = false
        end
      end

      

      str.each_char.with_index do |c, index|
        if c == "\n"
          x_point = left_margin
          y_point +=  y_advance + @line_padding
        elsif c == "\t"
          x_point += 4 * space_width 
        else
          if word_boundaries.key?(index) && max_width
            if x_point + word_boundaries.dig(index, :width) > max_width - right_margin
              x_point = left_margin
              y_point += y_advance + @line_padding
            end
          end

          font_render = @font_cache[c.ord]
          canvas.render_font_buffer(x_point + font_render.leftside_bearing, y_point + font_render.yoffset, font_render, 0)
          x_point += font_render.advance_width
        end
      end
      canvas.x_cursor = x_point
      canvas.y_cursor = y_point
    end

    private

    def is_word_char?(c)
      !["\n", "\r", " "].include?(c)
    end

  end
end
