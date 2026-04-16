# 04_imgui/meson.build
if not sdl2_dep.found()
  warning('SDL2 not found, skipping 04_imgui')
else
  executable('run',
    sources      : 'main.cpp',
    dependencies : [egl_dep, gles_dep, sdl2_dep, imgui_dep])
endif
