// 01_hello_triangle_SDL2/main.cpp
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <string_view>

void sdl_check(bool ok, std::string_view msg) {
    if (!ok) throw std::runtime_error(std::string(msg) + ": " + SDL_GetError());
}

GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok{}; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512]; glGetShaderInfoLog(s, sizeof(buf), nullptr, buf);
        throw std::runtime_error(std::string("shader: ") + buf);
    }
    return s;
}

constexpr const char *vert_src =
    "attribute vec2 pos;\n"
    "uniform float u_aspect;\n"
    "void main() {\n"
    "    gl_Position = vec4(pos.x / u_aspect, pos.y, 0.0, 1.0);\n"
    "}\n";

constexpr const char *frag_src =
    "precision mediump float;\n"
    "void main() { gl_FragColor = vec4(1.0, 0.5, 0.1, 1.0); }\n";

int main(int, char**) {
    try {
        SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
        sdl_check(SDL_Init(SDL_INIT_VIDEO) == 0, "SDL_Init");

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_Window *win = SDL_CreateWindow("01 hello triangle",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            0, 0,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
        sdl_check(win != nullptr, "SDL_CreateWindow");

        SDL_GLContext ctx = SDL_GL_CreateContext(win);
        sdl_check(ctx != nullptr, "SDL_GL_CreateContext");
        SDL_GL_SetSwapInterval(1);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, compile_shader(GL_VERTEX_SHADER,   vert_src));
        glAttachShader(prog, compile_shader(GL_FRAGMENT_SHADER, frag_src));
        glLinkProgram(prog);
        glUseProgram(prog);

        const float verts[] = { 0.0f, 0.6f,  -0.6f, -0.4f,  0.6f, -0.4f };
        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        GLint loc        = glGetAttribLocation(prog,  "pos");
        GLint loc_aspect = glGetUniformLocation(prog, "u_aspect");
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        bool running = true;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;
                if (e.type == SDL_KEYDOWN &&
                    e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
            int w, h;
            SDL_GL_GetDrawableSize(win, &w, &h);
            glViewport(0, 0, w, h);
            glUniform1f(loc_aspect, (float)w / (float)h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            SDL_GL_SwapWindow(win);
        }

        glDeleteBuffers(1, &vbo);
        glDeleteProgram(prog);
        SDL_GL_DeleteContext(ctx);
        SDL_DestroyWindow(win);
        SDL_Quit();
    } catch (const std::exception &ex) {
        std::fprintf(stderr, "error: %s\n", ex.what());
        return 1;
    }
    return 0;
}