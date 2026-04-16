# 02_triangle_color_attribute/meson.build
if not sdl2_dep.found()
  warning('SDL2 not found, skipping 02_triangle_color_attribute')
else
  executable('run',
    sources      : 'main.cpp',
    dependencies : [egl_dep, gles_dep, sdl2_dep])
endif
