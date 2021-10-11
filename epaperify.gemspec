# frozen_string_literal: true

require_relative "lib/epaperify/version"

Gem::Specification.new do |spec|
  spec.name          = "epaperify"
  spec.version       = Epaperify::VERSION
  spec.authors       = ["Joseph Emmanuel Dayo"]
  spec.email         = ["joseph.dayo@gmail.com"]

  spec.summary       = "Ruby gem for interfacing with E-paper screens"
  spec.description   = "Ruby gem for interfacing with E-paper screens"
  spec.homepage      = "https://github.com/jedld/epaperify"
  spec.license       = "MIT"
  spec.required_ruby_version = ">= 2.4.0"

  spec.metadata["allowed_push_host"] = "rubygems.org"

  spec.metadata["homepage_uri"] = spec.homepage
  spec.metadata["source_code_uri"] = "https://github.com/jedld/epaperify"
  spec.metadata["changelog_uri"] = "https://github.com/jedld/epaperify/CHANGELOG"

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files = Dir.chdir(File.expand_path(__dir__)) do
    `git ls-files -z`.split("\x0").reject do |f|
      (f == __FILE__) || f.match(%r{\A(?:(?:test|spec|features)/|\.(?:git|travis|circleci)|appveyor)})
    end
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{\Aexe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib"]
  spec.extensions = %w[ext/epaperify/extconf.rb]
  # Uncomment to register a new dependency of your gem
  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "pry-byebug"

  # For more information and examples about making a new gem, checkout our
  # guide at: https://bundler.io/guides/creating_gem.html
end
