# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

RSpec::Core::RakeTask.new(:spec)

require "rubocop/rake_task"

task default: %i[spec]


Rake::ExtensionTask.new "epaperify" do |ext|
  ext.lib_dir = "lib/epaperify"
end