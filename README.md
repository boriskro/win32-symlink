# Win32::Symlink

Since the standard ruby library lacks symlink functions on Windows, this
little library attempts to fill the gap.

## Installation

Add this line to your application's Gemfile:

    gem 'win32-symlink'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install win32-symlink

## Usage

This library implements the following functons that mimic their ruby
counterparts:

    readlink(link_name) -> file_name
    symlink?(link_name) -> true or false
    symlink(old_name, new_name) -> 0

