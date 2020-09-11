# frozen_string_literal: true

require('bundler/gem_tasks')
require('rspec/core/rake_task')
require('rake/extensiontask')

RSpec::Core::RakeTask.new(:spec)

Rake::ExtensionTask.new('etf') do |ext|
  ext.ext_dir = 'ext/vox'
  ext.lib_dir = 'lib/vox'
end

task(default: :spec)
task spec: ['compile']
