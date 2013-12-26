#!/usr/bin/env rake
require 'bundler/gem_tasks'
begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end

# require 'rubygems'
require 'rake/testtask'
require 'rake/clean'

$gemspec = Bundler.load_gemspec('win32-symlink.gemspec')

CLEAN.include "**/*.gem", "**/*.so", "**/Makefile", "**/*.o", "**/*.def", "**/mkmf.log"

Rake::TestTask.new do |t|
  t.warning = true
end

load 'tasks/ext.rake'
task :default => [:compile, :test]
