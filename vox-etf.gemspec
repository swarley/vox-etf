# frozen_string_literal: true

require_relative('lib/vox/etf/version')

Gem::Specification.new do |spec|
  spec.name          = 'vox-etf'
  spec.version       = Vox::ETF::VERSION
  spec.authors       = ['Matthew Carey']
  spec.email         = ['matthew.b.carey@gmail.com']

  spec.summary       = 'ETF decoding/encoding for vox.'
  spec.description   = spec.summary
  spec.homepage      = 'https://github.com/swarley/vox-etf'
  spec.license       = 'MIT'
  spec.required_ruby_version = Gem::Requirement.new('>= 2.3.0')

  spec.metadata['homepage_uri'] = spec.homepage
  spec.metadata['source_code_uri'] = 'https://github.com/swarley/vox-etf'
  spec.metadata['changelog_uri'] = 'https://github.com/swarley/vox-etf/blob/main/CHANGELOG.md'

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added
  # into git.
  spec.files =
    Dir.chdir(File.expand_path(__dir__)) do
      `git ls-files -z`.split("\x0").reject { |f| f.start_with?('spec/') }
    end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']
  spec.extensions << 'ext/vox/extconf.rb'

  spec.add_dependency 'rice', '~> 2.2.0'
  spec.add_development_dependency 'rake', '~> 12.3.3'
  spec.add_development_dependency 'rake-compiler', '~> 1.1.1'
  spec.add_development_dependency 'rspec', '~> 3.9.0'
  spec.add_development_dependency 'rubocop', '~> 0.90.0'
  spec.add_development_dependency 'rubocop-performance', '~> 1.8.0'
  spec.add_development_dependency 'rubocop-rspec', '~> 1.43.2'
end
