# frozen_string_literal: true

require_relative "epaperify/version"

module Epaperify
  VERSION = "0.1"
  class Error < StandardError; end
  # Your code goes here...
end

require "epaperify/epaperify"