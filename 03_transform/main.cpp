// 03_transform/main.cpp
//
// New concepts vs 02:
//   - 2D transformation matrix built on the CPU, passed as a uniform mat3
//   - rotation, non-uniform scale (sx, sy), translation
//
// To experiment: change the constants below, then recompile.

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string_view>

// ── tweak these and recompile ─────────────────────────────────────────
constexpr float ANGLE_DEG =  45.0f;
constexpr float SCALE_X   =  0.75f;
constexpr float SCALE_Y   =  0.75f;
constexpr float POS_X     =  0.0f;
constexpr float POS_Y     =  0.0f;

// ── shaders ───────────────────────────────────────────────────────────
constexpr const char *vert_src =
    "attribute vec2 a_pos;\n"
    "attribute vec3 a_color;\n"
    "varying   vec3 v_color;\n"
    "uniform   mat3 u_transform;\n"
    "void main() {\n"
    "    vec3 p      = u_transform * vec3(a_pos, 1.0);\n"
    "    gl_Position = vec4(p.xy, 0.0, 1.0);\n"
    "    v_color     = a_color;\n"
    "}\n";

constexpr const char *frag_src =
    "precision mediump float;\n"
    "varying vec3 v_color;\n"
    "void main() { gl_FragColor = vec4(v_color, 1.0); }\n";

// ── vertex data ───────────────────────────────────────────────────────
//  x      y      r     g     b
constexpr float verts[] = {
     0.0f,  0.6f,  1.0f, 0.0f, 0.0f,
    -0.6f, -0.4f,  0.0f, 1.0f, 0.0f,
     0.6f, -0.4f,  0.0f, 0.0f, 1.0f,
};

constexpr int STRIDE       = 5 * sizeof(float);
constexpr int OFFSET_POS   = 0;
constexpr int OFFSET_COLOR = 2 * sizeof(float);

// ── math ──────────────────────────────────────────────────────────────
//
// Column-major 2D transform with non-uniform scale:
//
//   | c·sx  -s·sy   tx |
//   | s·sx   c·sy   ty |
//   |  0      0      1 |
//
// Stored column by column: col0, col1, col2
static void make_transform(float out[9],
                            float tx, float ty,
                            float angle_deg,
                            float sx, float sy) {
    const float a = angle_deg * 3.14159265f / 180.0f;
    const float c = std::cos(a);
    const float s = std::sin(a);
    out[0] = c * sx;  out[1] = s * sx;  out[2] = 0.0f;  // column 0
    out[3] = -s * sy; out[4] = c * sy;  out[5] = 0.0f;  // column 1
    out[6] = tx;      out[7] = ty;      out[8] = 1.0f;  // column 2
}

// ── helpers ───────────────────────────────────────────────────────────
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

// ── main ──────────────────────────────────────────────────────────────
int main(int, char**) {
    try {
        SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
        sdl_check(SDL_Init(SDL_INIT_VIDEO) == 0, "SDL_Init");

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_Window *win = SDL_CreateWindow("03 transform",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        sdl_check(win != nullptr, "SDL_CreateWindow");

        SDL_GLContext ctx = SDL_GL_CreateContext(win);
        sdl_check(ctx != nullptr, "SDL_GL_CreateContext");
        SDL_GL_SetSwapInterval(1);

        GLuint prog = glCreateProgram();
        glAttachShader(prog, compile_shader(GL_VERTEX_SHADER,   vert_src));
        glAttachShader(prog, compile_shader(GL_FRAGMENT_SHADER, frag_src));
        glLinkProgram(prog);
        glUseProgram(prog);

        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        GLint loc_pos = glGetAttribLocation(prog, "a_pos");
        glEnableVertexAttribArray(loc_pos);
        glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_POS));

        GLint loc_col = glGetAttribLocation(prog, "a_color");
        glEnableVertexAttribArray(loc_col);
        glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_COLOR));

        GLint loc_xform = glGetUniformLocation(prog, "u_transform");
        float mat[9];
        make_transform(mat, POS_X, POS_Y, ANGLE_DEG, SCALE_X, SCALE_Y);
        glUniformMatrix3fv(loc_xform, 1, GL_FALSE, mat);

        bool running = true;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) running = false;
                if (e.type == SDL_KEYDOWN &&
                    e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            SDL_GL_SwapWindow(win);
        }

        glDisableVertexAttribArray(loc_pos);
        glDisableVertexAttribArray(loc_col);
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
