# -*- ruby -*-
require 'rake/extensiontask'
Rake::ExtensionTask.new('win32_symlink', $gemspec) do |ext|
  ext.ext_dir = 'ext/win32'
  ext.lib_dir = 'lib/win32/symlink'
end

