// 04_imgui/main.cpp
//
// New concepts vs 03:
//   - Dear ImGui integrated with SDL2 + GLES2
//   - runtime control of transform (pos, angle, scale_x, scale_y)
//   - runtime control of per-vertex colors via ImGui color pickers
//   - dynamic VBO update with glBufferSubData

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string_view>

// ── shaders (identical to 03) ─────────────────────────────────────────
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

// ── vertex layout (same as 02, 03) ────────────────────────────────────
constexpr int STRIDE       = 5 * sizeof(float);
constexpr int OFFSET_POS   = 0;
constexpr int OFFSET_COLOR = 2 * sizeof(float);

// ── mutable vertex buffer ─────────────────────────────────────────────
// Not constexpr — ImGui will modify colors at runtime.
// Layout: [ x, y, r, g, b ] per vertex, 3 vertices.
static float verts[] = {
     0.0f,  0.6f,  1.0f, 0.0f, 0.0f,   // vertex 0 — top
    -0.6f, -0.4f,  0.0f, 1.0f, 0.0f,   // vertex 1 — left
     0.6f, -0.4f,  0.0f, 0.0f, 1.0f,   // vertex 2 — right
};

// ── math (same as 03) ─────────────────────────────────────────────────
static void make_transform(float out[9],
                            float tx, float ty,
                            float angle_deg,
                            float sx, float sy) {
    const float a = angle_deg * 3.14159265f / 180.0f;
    const float c = std::cos(a);
    const float s = std::sin(a);
    out[0] =  c * sx;  out[1] = s * sx;  out[2] = 0.0f;
    out[3] = -s * sy;  out[4] = c * sy;  out[5] = 0.0f;
    out[6] =  tx;      out[7] = ty;      out[8] = 1.0f;
}

// ── helpers (same as 03) ──────────────────────────────────────────────
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

        SDL_Window *win = SDL_CreateWindow("04 imgui transform",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        sdl_check(win != nullptr, "SDL_CreateWindow");

        SDL_GLContext ctx = SDL_GL_CreateContext(win);
        sdl_check(ctx != nullptr, "SDL_GL_CreateContext");

        SDL_Log("GL_VENDOR:   %s", glGetString(GL_VENDOR));
        SDL_Log("GL_RENDERER: %s", glGetString(GL_RENDERER));
        SDL_Log("GL_VERSION:  %s", glGetString(GL_VERSION));
        SDL_Log("GL_SL_VER:   %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

        SDL_GL_SetSwapInterval(1);

        // ── GL setup ──────────────────────────────────────────────────
        GLuint prog = glCreateProgram();
        glAttachShader(prog, compile_shader(GL_VERTEX_SHADER,   vert_src));
        glAttachShader(prog, compile_shader(GL_FRAGMENT_SHADER, frag_src));
        glLinkProgram(prog);
        glUseProgram(prog);

        GLuint vbo{};
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // GL_DYNAMIC_DRAW signals we will update this buffer every frame
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

        GLint loc_pos = glGetAttribLocation(prog, "a_pos");
        glEnableVertexAttribArray(loc_pos);
        glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_POS));

        GLint loc_col = glGetAttribLocation(prog, "a_color");
        glEnableVertexAttribArray(loc_col);
        glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_COLOR));

        GLint loc_xform = glGetUniformLocation(prog, "u_transform");

        // ── ImGui setup ───────────────────────────────────────────────
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(win, ctx);
        ImGui_ImplOpenGL3_Init("#version 100");
        ImGui::StyleColorsDark();

        // ── app state ─────────────────────────────────────────────────
        float pos_x   =  0.0f, pos_y   = 0.0f;
        float angle   =  0.0f;
        float scale_x =  1.0f, scale_y = 1.0f;

        // per-vertex colors as float[3] — directly writable by ImGui
        float col[3][3] = {
            { 1.0f, 0.0f, 0.0f },   // vertex 0 — red
            { 0.0f, 1.0f, 0.0f },   // vertex 1 — green
            { 0.0f, 0.0f, 1.0f },   // vertex 2 — blue
        };

        // ── loop ──────────────────────────────────────────────────────
        bool running = true;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);
                if (e.type == SDL_QUIT) running = false;
                if (e.type == SDL_KEYDOWN &&
                    e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }

            // ── ImGui frame ───────────────────────────────────────────
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("controls");

            if (ImGui::CollapsingHeader("transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("pos_x",   &pos_x,   -1.0f,  1.0f);
                ImGui::SliderFloat("pos_y",   &pos_y,   -1.0f,  1.0f);
                ImGui::SliderFloat("angle",   &angle,    0.0f, 360.0f);
                ImGui::SliderFloat("scale_x", &scale_x,  0.1f,   2.0f);
                ImGui::SliderFloat("scale_y", &scale_y,  0.1f,   2.0f);
            }

            if (ImGui::CollapsingHeader("vertex colors", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::ColorEdit3("vertex 0 (top)",   col[0]);
                ImGui::ColorEdit3("vertex 1 (left)",  col[1]);
                ImGui::ColorEdit3("vertex 2 (right)", col[2]);
            }

            ImGui::End();

            // ── sync colors into vertex buffer ────────────────────────
            // Write the 3 rgb floats at the color offset of each vertex.
            // glBufferSubData updates only the changed bytes, not the whole buffer.
            for (int i = 0; i < 3; ++i) {
                verts[i * 5 + 2] = col[i][0];
                verts[i * 5 + 3] = col[i][1];
                verts[i * 5 + 4] = col[i][2];
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

            // ── upload transform ──────────────────────────────────────
            float mat[9];
            make_transform(mat, pos_x, pos_y, angle, scale_x, scale_y);
            glUniformMatrix3fv(loc_xform, 1, GL_FALSE, mat);

            // ── draw ──────────────────────────────────────────────────
            int w, h; SDL_GL_GetDrawableSize(win, &w, &h);
            glViewport(0, 0, w, h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(win);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
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
