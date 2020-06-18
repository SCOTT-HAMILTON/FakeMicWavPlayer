project('FakeMicWavPlayer', 'cpp', default_options : ['cpp_std=c++17'])

libpulse = dependency('libpulse')
libpulse_simple = dependency('libpulse-simple')

lib_inc = include_directories('include')
lib_src = [ 'FakeLib2.cpp', 'CallBacks.cpp', 'FakeLibImplementation.cpp' ]
lib_fake = static_library('Fake', lib_src, 
  include_directories : lib_inc,
  dependencies : [libpulse, libpulse_simple], 
  install : true)
lib_dep = declare_dependency(
  link_with: lib_fake,
  include_directories: lib_inc)

tool_src = [ 'main.cpp' ]

executable('FakeMicWavPlayer', 'main.cpp', 
  dependencies : [ lib_dep ],
  install : true)

install_headers('include/FakeLib.h', 'include/CallBacks.h', 'include/FakeLibUtils.hpp')

install_data('pkg-config/fake.pc',
  install_dir: get_option('libdir')+'/pkgconfig')


