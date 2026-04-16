# 01_hello_triangle_SDL2/meson.build
if not sdl2_dep.found()
  warning('SDL2 not found, skipping 01_hello_triangle_SDL2')
else
  executable('triangle',
    sources      : 'main.cpp',
    dependencies : [egl_dep, gles_dep, sdl2_dep])
endif
