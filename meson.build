project('FakeMicWavPlayer', 'cpp', default_options : ['cpp_std=c++17'])

libfake = dependency('fake', required: true)

argparse = dependency('argparse', required: get_option('USE_SYSTEM_ARGPARSE'))
if not argparse.found()
  cmake = import('cmake')
  argparse_proj = cmake.subproject('argparse')
  argparse = argparse_proj.dependency('argparse')
endif

OggPlayer_proj = subproject('OggPlayer')
libOggPlayer_dep = OggPlayer_proj.get_variable('libOggPlayer_dep')
# libOggPlayer = OggPlayer_proj.get_variable('libOggPlayer')

lib_inc = include_directories('include')
lib_src = [ 'FakeMicWavPlayerLib.cpp' ]

libFakeMicWavPlayer = shared_library('FakeMicWavPlayer', lib_src, 
  include_directories : lib_inc,
  dependencies : [ libfake, libOggPlayer_dep ], 
  install : true)

libFakeMicWavPlayer_dep = declare_dependency(
  link_with: libFakeMicWavPlayer,
  include_directories: lib_inc)

install_headers('include/FakeMicWavPlayerLib.h' )

executable('FakeMicWavPlayer', 'main.cpp', 'FakeMicWavPlayerLib.cpp',
  dependencies : [ libfake, libFakeMicWavPlayer_dep, libOggPlayer_dep, argparse ],
  install : true)

install_data('pkg-config/fakemicwavplayer.pc',
  install_dir: get_option('libdir')+'/pkgconfig')
install_data('pkg-config/fakemicwavplayer.pc',
  install_dir: get_option('datadir')+'/pkgconfig')
