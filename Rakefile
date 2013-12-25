#!/usr/bin/env rake
# require 'bundle/gemtasks'
require 'rake'
require 'rake/clean'
require 'rake/testtask'


CLEAN.include(
  "**/*.gem",
  "**/*.so",
  "**/Makefile",
  "**/*.o",
  "**/*.def",
  "**/mkmf.log"
)

def make
  CONFIG['host_os'] =~ /mingw|cygwin/i ? "make" : "nmake"
end

# CLOBBER.include("lib")
SRC_BASE = 'ext/win32'
MAKEFILE = SRC_BASE + '/Makefile'
EXTCONF_RB = SRC_BASE + '/extconf.rb'
SRC = FileList.new(SRC_BASE+'/*.cpp', SRC_BASE+'/*.c')
EXT_SO = SRC_BASE+'/symlink.so'

file MAKEFILE => [EXTCONF_RB] do
  Dir.chdir(SRC_BASE) { ruby 'extconf.rb' }
end

file EXT_SO => SRC + [MAKEFILE] do
  Dir.chdir(SRC_BASE) { sh make }
end

task :config do
  Dir.chdir(SRC_BASE) { ruby 'extconf.rb' }
end

task :build => EXT_SO

Rake::TestTask.new do |t|
  task :test => [:build]
  t.libs << 'ext'
  t.warning = true
#  t.verbose = true
end

task :default => [:build, :test]
