module Win32
  module Symlink
    module Version
      MAJOR = 0
      MINOR = 1
      PATCH = 3
      BUILD = nil
      STRING = [MAJOR, MINOR, PATCH, BUILD].compact.join '.'
    end
    VERSION = Version::STRING
  end
end
