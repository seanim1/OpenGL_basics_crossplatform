# CMakeLists.txt — root
cmake_minimum_required(VERSION 3.20)
project(gles2_tutorial CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── dependencies ─────────────────────────────────────────────────────
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(unofficial-angle CONFIG REQUIRED)

    add_library(gles2_deps INTERFACE)
    target_link_libraries(gles2_deps INTERFACE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        unofficial::angle::libGLESv2
        unofficial::angle::libEGL)

elseif(IOS)
    find_package(SDL2 REQUIRED)

    add_library(gles2_deps INTERFACE)
    target_link_libraries(gles2_deps INTERFACE
        ${SDL2_LIBRARIES}
        "-framework OpenGLES"
        "-framework UIKit"
        "-framework CoreGraphics"
        "-framework CoreMotion"
        "-framework GameController")

else()
    find_package(SDL2 REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EGL  REQUIRED egl)
    pkg_check_modules(GLES REQUIRED glesv2)

    add_library(gles2_deps INTERFACE)
    target_include_directories(gles2_deps INTERFACE
        ${SDL2_INCLUDE_DIRS}
        ${EGL_INCLUDE_DIRS}
        ${GLES_INCLUDE_DIRS})
    target_link_libraries(gles2_deps INTERFACE
        ${SDL2_LIBRARIES}
        ${EGL_LIBRARIES}
        ${GLES_LIBRARIES})
    target_link_options(gles2_deps INTERFACE
        ${EGL_LDFLAGS_OTHER}
        ${GLES_LDFLAGS_OTHER})
endif()

# ── GLM (header-only, vendored) ───────────────────────────────────────
set(GLM_DIR ${CMAKE_SOURCE_DIR}/third_party/glm)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_DIR})

# ── Dear ImGui (vendored) ─────────────────────────────────────────────
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends)

target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_ES2)
target_link_libraries(imgui PUBLIC gles2_deps)

# ── subprojects ───────────────────────────────────────────────────────
if(NOT IOS)
    add_subdirectory(00_probe_OpenGLES2)
endif()
add_subdirectory(01_hello_triangle_SDL2)
add_subdirectory(02_triangle_color_attribute)
add_subdirectory(03_transform)
add_subdirectory(04_imgui)

# ── iOS app bundle properties ─────────────────────────────────────────
if(IOS)
    set_target_properties(01_hello_triangle_SDL2 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "hellotriangle"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sean.gles2.hellotriangle"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sean.gles2.hellotriangle")

    set_target_properties(02_triangle_color_attribute PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "trianglecolor"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sean.gles2.trianglecolor"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sean.gles2.trianglecolor")

    set_target_properties(03_transform PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "transform"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sean.gles2.transform"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sean.gles2.transform")

    set_target_properties(04_imgui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "imgui"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sean.gles2.imgui"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sean.gles2.imgui")
endif()