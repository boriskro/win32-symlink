# -*- ruby -*-
require File.expand_path('../lib/win32/symlink/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ['Boris Kropivnitsky']
  gem.email         = ['boris.kro@gmail.com']
  gem.description   = <<DESC
Symlink functions for Windows (Vista and above)
DESC
  gem.summary       = 'symlink functions for Windows'
  gem.homepage      = 'https://github.com/boriskro/win32-symlink'

  gem.files         = %w{
Gemfile
LICENSE
README.md
Rakefile
ext/win32/extconf.rb
ext/win32/win32-symlink.cpp
lib/win32/symlink.rb
lib/win32/symlink/version.rb
lib/win32/symlink/win32_symlink.so
tasks/ext.rake
test/test_win32_symlink.rb
win32-symlink.gemspec
}
  gem.extensions    = ['ext/win32/extconf.rb']
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = 'win32-symlink'
  gem.require_paths = ['lib']
  gem.version       = Win32::Symlink::VERSION
  gem.licenses      = ['MIT']
  gem.add_development_dependency 'bundler'
  gem.add_development_dependency 'rake-compiler'
end
