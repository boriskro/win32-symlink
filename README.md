# Win32::Symlink

Since standard ruby library lacks symlink functions on Windows, this simple
library attempts to fill the gap.

## Installation

Add this line to your application's Gemfile:

    gem 'win32-symlink'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install win32-symlink

## Usage

This library provides the following functons:
    readlink
    symlink?
    symlink

These work similar to their counterparts found in the standard `File` module.

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
