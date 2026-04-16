# meson.build — root
project('gles2-tutorial', 'cpp',
  version : '0.1.0',
  default_options : ['cpp_std=c++17', 'buildtype=debugoptimized'])

egl_dep  = dependency('egl')
gles_dep = dependency('glesv2')
sdl2_dep = dependency('sdl2', required : false)

# ── Dear ImGui (vendored) ─────────────────────────────────────────────
imgui_inc = include_directories('third_party/imgui',
                                'third_party/imgui/backends')

imgui_lib = static_library('imgui',
  sources : [
    'third_party/imgui/imgui.cpp',
    'third_party/imgui/imgui_draw.cpp',
    'third_party/imgui/imgui_tables.cpp',
    'third_party/imgui/imgui_widgets.cpp',
    'third_party/imgui/imgui_demo.cpp',
    'third_party/imgui/backends/imgui_impl_sdl2.cpp',
    'third_party/imgui/backends/imgui_impl_opengl3.cpp',
  ],
  include_directories : imgui_inc,
  dependencies : [sdl2_dep, gles_dep])

imgui_dep = declare_dependency(
  link_with           : imgui_lib,
  include_directories : imgui_inc)

subdir('00_probe_OpenGLES2')
subdir('01_hello_triangle_SDL2')
subdir('02_triangle_color_attribute')
subdir('03_transform')
subdir('04_imgui')
