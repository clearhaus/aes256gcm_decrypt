# frozen_string_literal: true

require 'rake/extensiontask'
require 'rspec/core/rake_task'

spec = Gem::Specification.load('aes256gcm_decrypt.gemspec')
Rake::ExtensionTask.new('aes256gcm_decrypt', spec)

desc ''
RSpec::Core::RakeTask.new(:spec) do |task|
  task.pattern = './spec/**/*_spec.rb'
end

desc 'Compile extension and run specs'
task test: %i[compile spec]
