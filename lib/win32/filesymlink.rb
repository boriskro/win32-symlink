require_relative 'symlink'


# Extending the File class on windows system, so that its symlink function work as on other sytems.
class File

  def File.symlink(file, symlink)
    Win32::Symlink::symlink(file, symlink)
  end

  def File.symlink?(file)
    Win32::Symlink::symlink?(file)
  end

  def File.readlink(file)
    Win32::Symlink::readlink(file)
  end

end
