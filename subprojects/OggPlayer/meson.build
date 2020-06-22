project('OggPlayer', 'cpp')

libogg = dependency('ogg')
libvorbis = dependency('vorbis')
libpulse_simple = dependency('libpulse-simple')

lib_inc = include_directories('include')
lib_src = [ 'OggPlayer.cpp', 'ogg.cpp', 'pa_player.cpp' ]
libOggPlayer = static_library('OggPlayer', lib_src, 
  include_directories : lib_inc,
  dependencies : [ libogg, libvorbis, libpulse_simple ], 
  install : true)

libOggPlayer_dep = declare_dependency(
  link_with: libOggPlayer,
  include_directories: lib_inc)

install_headers( 'include/OggPlayer.h', 'include/ogg.h', 'include/pa_player.h' )

executable('OggPlayer', 'main.cpp',
  dependencies : [ libOggPlayer_dep, libogg, libvorbis ],
  install : true)

install_data('pkg-config/oggplayer.pc',
  install_dir: get_option('libdir')+'/pkgconfig')
