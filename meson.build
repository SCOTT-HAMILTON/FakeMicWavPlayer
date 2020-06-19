project('FakeMicWavPlayer', 'cpp', default_options : ['cpp_std=c++17'])

fake_lib_dep = dependency('fake', required: true)
libpulse = dependency('libpulse')
libpulse_simple = dependency('libpulse-simple')
argparse = dependency('argparse')

lib_inc = include_directories('include')
lib_src = [ 'FakeMicWavPlayerLib.cpp', 'player.cpp' ]
lib = static_library('FakeMicWavPlayer', lib_src, 
  include_directories : lib_inc,
  dependencies : [ libpulse, libpulse_simple, fake_lib_dep ], 
  install : true)


lib_dep = declare_dependency(
  link_with: lib,
  include_directories: lib_inc)

install_headers('include/FakeMicWavPlayerLib.h', 'include/player.h')

executable('FakeMicWavPlayer', 'main.cpp', 'FakeMicWavPlayerLib.cpp',
  dependencies : [ lib_dep, argparse ],
  install : true)

