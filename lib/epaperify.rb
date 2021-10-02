# frozen_string_literal: true

require_relative "epaperify/version"

module Epaperify
  VERSION = "0.1"
  DISPLAY_EPD_2IN7B_V2 = 0
  DISPLAY_EPD_5IN83B_V2 = 1
  DISPLAY_EPD_5IN83_V2 = 2

  class Error < StandardError; end
  # Your code goes here...
end

require "epaperify/epaperify"