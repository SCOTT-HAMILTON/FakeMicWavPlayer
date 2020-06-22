project('FakeMicWavPlayer', 'cpp', default_options : ['cpp_std=c++17'])

libfake = dependency('fake', required: true)
libpulse = dependency('libpulse')
libpulse_simple = dependency('libpulse-simple')
argparse = dependency('argparse')

OggPlayer_proj = subproject('OggPlayer')
libOggPlayer_dep = OggPlayer_proj.get_variable('libOggPlayer_dep')
# libOggPlayer = OggPlayer_proj.get_variable('libOggPlayer')

lib_inc = include_directories('include')
lib_src = [ 'FakeMicWavPlayerLib.cpp' ]

libFakeMicWavPlayer = static_library('FakeMicWavPlayer', lib_src, 
  include_directories : lib_inc,
  dependencies : [ libpulse, libpulse_simple, libfake, libOggPlayer_dep ], 
  install : true)

libFakeMicWavPlayer_dep = declare_dependency(
  link_with: libFakeMicWavPlayer,
  include_directories: lib_inc)

install_headers('include/FakeMicWavPlayerLib.h' )

executable('FakeMicWavPlayer', 'main.cpp', 'FakeMicWavPlayerLib.cpp',
  dependencies : [ libFakeMicWavPlayer_dep, libOggPlayer_dep, argparse ],
  install : true)

install_data('pkg-config/fakemicwavplayer.pc',
  install_dir: get_option('libdir')+'/pkgconfig')
