// 00_probe_OpenGLES2/probe.cpp
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstdio>
#include <stdexcept>
#include <string_view>

namespace {

void check(bool ok, std::string_view msg) {
    if (!ok) throw std::runtime_error(std::string(msg));
}

struct EGLProbe {
    EGLDisplay dpy  = EGL_NO_DISPLAY;
    EGLSurface surf = EGL_NO_SURFACE;
    EGLContext ctx  = EGL_NO_CONTEXT;

    EGLProbe() {
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        check(dpy != EGL_NO_DISPLAY, "eglGetDisplay failed");

        EGLint major{}, minor{};
        check(eglInitialize(dpy, &major, &minor), "eglInitialize failed");
        std::printf("EGL %d.%d\n", major, minor);

        const EGLint cfg_attrs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,    EGL_PBUFFER_BIT,
            EGL_NONE
        };
        EGLConfig cfg{}; EGLint n{};
        check(eglChooseConfig(dpy, cfg_attrs, &cfg, 1, &n) && n > 0,
              "no GLES2 config found");

        const EGLint pb_attrs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
        surf = eglCreatePbufferSurface(dpy, cfg, pb_attrs);
        check(surf != EGL_NO_SURFACE, "pbuffer surface failed");

        const EGLint ctx_attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctx_attrs);
        check(ctx != EGL_NO_CONTEXT, "eglCreateContext failed");

        check(eglMakeCurrent(dpy, surf, surf, ctx), "eglMakeCurrent failed");
    }

    void print_info() const {
        auto gs = [](GLenum e) {
            return reinterpret_cast<const char*>(glGetString(e));
        };
        std::printf("GL_VENDOR:   %s\n", gs(GL_VENDOR));
        std::printf("GL_RENDERER: %s\n", gs(GL_RENDERER));
        std::printf("GL_VERSION:  %s\n", gs(GL_VERSION));
        std::printf("GL_SL_VER:   %s\n", gs(GL_SHADING_LANGUAGE_VERSION));
    }

    ~EGLProbe() {
        if (ctx  != EGL_NO_CONTEXT) eglDestroyContext(dpy, ctx);
        if (surf != EGL_NO_SURFACE) eglDestroySurface(dpy, surf);
        if (dpy  != EGL_NO_DISPLAY) eglTerminate(dpy);
    }
};

} // namespace

int main() {
    try {
        EGLProbe probe;
        probe.print_info();
    } catch (const std::exception &ex) {
        std::fprintf(stderr, "error: %s\n", ex.what());
        return 1;
    }
    return 0;
}
