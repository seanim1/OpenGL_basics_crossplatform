# meson.build — root
project('gles2-tutorial', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++17', 'buildtype=debugoptimized'])

# shared deps resolved once, reused by subprojects
egl_dep  = dependency('egl')
gles_dep = dependency('glesv2')
sdl2_dep = dependency('sdl2', required : false)

subdir('00_probe_OpenGLES2')
subdir('01_hello_triangle_SDL2')
