require 'mkmf'

if CONFIG['CXXFLAGS'] == '$(cxxflags)'
  CONFIG['CXXFLAGS'] = CONFIG['cxxflags'].clone
end

# these are not relevant for C++
CONFIG['warnflags'].sub! '-Wdeclaration-after-statement', ''
CONFIG['warnflags'].sub! '-Wimplicit-function-declaration', ''

try_cppflags $CPPFLAGS.sub(/_WIN32_WINNT=\S+/, '_WIN32_WINNT=0x600')
create_makefile 'win32/symlink'
