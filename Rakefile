# -*- ruby -*-
require 'bundler/gem_tasks'
require 'rake/testtask'

begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

$gemspec = Bundler.load_gemspec('win32-symlink.gemspec')

Rake::TestTask.new do |t|
  t.warning = true
end

load 'tasks/ext.rake'

task :default => [:compile, :test]
