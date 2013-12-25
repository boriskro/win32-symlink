# -*- encoding: utf-8 -*-
# require File.expand_path('../lib/win32-symlink/version', __FILE__)

Gem::Specification.new do |gem|
  gem.authors       = ["Boris Kropivnitsky"]
  gem.email         = ["boris.kro@gmail.com"]
  gem.description   = %q{Symlink functions for Windows (Vista and above)}
  gem.summary       = %q{symlinks for Windows}
  gem.homepage      = ""

  gem.files         = `git ls-files`.split($\)
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.name          = "win32-symlink"
  gem.require_paths = ["ext"]
  gem.version       = '0.1' # Win32::Symlink::VERSION
end
