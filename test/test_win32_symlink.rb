$LOAD_PATH.unshift File.join(File.dirname(File.dirname(__FILE__)), 'ext')

# puts $LOAD_PATH

require 'test/unit'
require 'win32/symlink'
include Win32

class Errno::E4390 < SystemCallError
  # The file is not a reparse point
end
class Errno::E1314 < SystemCallError
  # A required privilege is not set
end

class TC_Win32Symlink < Test::Unit::TestCase

  def setup
    @users_dir = File.join(ENV['SYSTEMDRIVE'], '/Users')    
  end

  def test_bad_args
    assert_raises(Errno::ENOENT) {Symlink.symlink? ""}
    assert_raises(TypeError) { Symlink.symlink? nil }
    assert_raises(TypeError) { Symlink.symlink?(Object.new) }
    assert_raises(TypeError) { Symlink.symlink "", nil }
    assert_raises(TypeError) { Symlink.symlink nil, "" }
  end

  def test_system_links
    assert(Symlink.symlink?(File.join(@users_dir, "All Users")))
    assert(!Symlink.symlink?(ENV['WINDIR']))
  end

  def test_readlink
    assert_equal(Symlink.readlink(File.join(@users_dir, "All Users")), ENV['ALLUSERSPROFILE'])
  end

  def test_nonlink
    assert_raises(Errno::E4390) { Symlink.readlink(ENV['WINDIR']) }
  end

  def test_makelink
    Dir.chdir(ENV["TMP"]) do
      IO::write("symlink-test-file", "test")
      begin
        Symlink.symlink("symlink-test-file", "symlink-test-symlink") 
        assert(Symlink.symlink? "symlink-test-symlink")
      rescue Errno::E1314 => e
        skip e.message
      end
    end
  end

  def teardown
    Dir.glob(File.join(ENV['TMP'], 'symlink-test*')).each {|f| File::unlink f}
  end

end

