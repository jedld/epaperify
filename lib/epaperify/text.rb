module Epaperify
  class Text
    def initialize(font)
      @font = font
      @font_cache = {}
      @line_padding = 5
      @string = ""

      # pre render space

      @font_cache[' '.ord] ||= begin
        @font.render_font(' '.ord)
      end

      @space_width = @font_cache[' '.ord].advance_width
    end

    def line_padding(padding)
      @line_padding = padding
    end

    def string=(str)
      @string = str

      str.each_char do |c|
        @font_cache[c.ord] ||= begin
          @font.render_font(c.ord)
        end
      end
    end

    def string
      @string
    end

    def measure(canvas, options = {})
      render(0, 0, canvas, options)
    end

    def render_string(canvas, options = {})
      x = options.fetch(:x, canvas.x_cursor)
      y = options.fetch(:y, canvas.y_cursor)

      canvas.x_cursor, canvas.y_cursor = render(x, y, canvas, options) do |x_point, y_point, font_render|
        canvas.render_font_buffer(x_point + font_render.leftside_bearing, y_point + font_render.yoffset, font_render, 0)
      end
    end

    private

    def is_word_char?(c)
      !["\n", "\r", " "].include?(c)
    end

    def render(x, y, canvas, options = {}, &block)
      max_width = options.fetch(:width, canvas.width)
      left_margin = options.fetch(:left_margin, 0)
      right_margin = options.fetch(:right_margin, 0)
      top_margin = options.fetch(:top_margin, 0)

      y_advance = (@font.ascender + @font.descender + @font.linegap)

      x_point = x + left_margin
      y_point = y + y_advance + top_margin

      inside_word = false
      word_boundaries = {}
      current_boundary = nil

      @string.each_char.with_index do |c, index|
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

      @string.each_char.with_index do |c, index|
        if c == "\n"
          x_point = left_margin
          y_point +=  y_advance + @line_padding
        elsif c == "\t"
          x_point += 4 * @space_width
        else
          if word_boundaries.key?(index) && max_width
            if x_point + word_boundaries.dig(index, :width) > max_width - right_margin
              x_point = left_margin
              y_point += y_advance + @line_padding
            end
          end

          font_render = @font_cache[c.ord]

          if block_given?
            yield x_point, y_point, font_render
          end

          x_point += font_render.advance_width
        end
      end

      [x_point, y_point]
    end

  end
end
