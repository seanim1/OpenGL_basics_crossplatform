// 04_imgui/main.cpp
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <string_view>

constexpr const char *vert_src =
    "attribute vec2 a_pos;\n"
    "attribute vec3 a_color;\n"
    "varying   vec3 v_color;\n"
    "uniform   mat3 u_transform;\n"
    "uniform   float u_aspect_x;\n"
    "uniform   float u_aspect_y;\n"
    "void main() {\n"
    "    vec3 p      = u_transform * vec3(a_pos, 1.0);\n"
    "    gl_Position = vec4(p.x / u_aspect_x, p.y / u_aspect_y, 0.0, 1.0);\n"
    "    v_color     = a_color;\n"
    "}\n";

constexpr const char *frag_src =
    "precision mediump float;\n"
    "varying vec3 v_color;\n"
    "void main() { gl_FragColor = vec4(v_color, 1.0); }\n";

constexpr int STRIDE       = 5 * sizeof(float);
constexpr int OFFSET_POS   = 0;
constexpr int OFFSET_COLOR = 2 * sizeof(float);

static float verts[] = {
     0.0f,  0.6f,  1.0f, 0.0f, 0.0f,
    -0.6f, -0.4f,  0.0f, 1.0f, 0.0f,
     0.6f, -0.4f,  0.0f, 0.0f, 1.0f,
};

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

int main(int, char**) {
    try {
        SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
        sdl_check(SDL_Init(SDL_INIT_VIDEO) == 0, "SDL_Init");

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_Window *win = SDL_CreateWindow("04 imgui transform",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            0, 0,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);

        GLint loc_pos = glGetAttribLocation(prog, "a_pos");
        glEnableVertexAttribArray(loc_pos);
        glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_POS));

        GLint loc_col = glGetAttribLocation(prog, "a_color");
        glEnableVertexAttribArray(loc_col);
        glVertexAttribPointer(loc_col, 3, GL_FLOAT, GL_FALSE,
                              STRIDE, reinterpret_cast<void*>(OFFSET_COLOR));

        GLint loc_xform    = glGetUniformLocation(prog, "u_transform");
        GLint loc_aspect_x = glGetUniformLocation(prog, "u_aspect_x");
        GLint loc_aspect_y = glGetUniformLocation(prog, "u_aspect_y");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForOpenGL(win, ctx);
        ImGui_ImplOpenGL3_Init("#version 100");
        ImGui::StyleColorsDark();

        float pos_x    = 0.0f, pos_y    = 0.0f;
        float angle    = 0.0f;
        float scale_x  = 1.0f, scale_y  = 1.0f;
        float aspect_x = 1.0f, aspect_y = 1.0f;
        float col[3][3] = {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
        };

        bool running = true;
        while (running) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL2_ProcessEvent(&e);
                if (e.type == SDL_QUIT) running = false;
                if (e.type == SDL_KEYDOWN &&
                    e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("controls");
            if (ImGui::CollapsingHeader("transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("pos_x",   &pos_x,   -2.0f,  2.0f);
                ImGui::SliderFloat("pos_y",   &pos_y,   -2.0f,  2.0f);
                ImGui::SliderFloat("angle",   &angle,    0.0f, 360.0f);
                ImGui::SliderFloat("scale_x", &scale_x,  0.1f,   2.0f);
                ImGui::SliderFloat("scale_y", &scale_y,  0.1f,   2.0f);
            }
            if (ImGui::CollapsingHeader("aspect ratio", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("aspect_x", &aspect_x, 0.1f, 4.0f);
                ImGui::SliderFloat("aspect_y", &aspect_y, 0.1f, 4.0f);
                if (ImGui::Button("reset_y_fixed")) {
                    int w, h;
                    SDL_GL_GetDrawableSize(win, &w, &h);
                    aspect_x = (float)w / (float)h;
                    aspect_y = 1.0f;
                }
                ImGui::SameLine();
                if (ImGui::Button("reset_x_fixed")) {
                    int w, h;
                    SDL_GL_GetDrawableSize(win, &w, &h);
                    aspect_x = 1.0f;
                    aspect_y = (float)h / (float)w;
                }
                ImGui::SameLine();
                if (ImGui::Button("no correction")) {
                    aspect_x = 1.0f;
                    aspect_y = 1.0f;
                }
            }
            if (ImGui::CollapsingHeader("vertex colors", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::ColorEdit3("vertex 0 (top)",   col[0], ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit3("vertex 1 (left)",  col[1], ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit3("vertex 2 (right)", col[2], ImGuiColorEditFlags_NoInputs);
            }
            ImGui::Spacing();
            if (ImGui::Button("Quit", ImVec2(-1, 0))) {
                running = false;
            }
            ImGui::End();

            for (int i = 0; i < 3; ++i) {
                verts[i * 5 + 2] = col[i][0];
                verts[i * 5 + 3] = col[i][1];
                verts[i * 5 + 4] = col[i][2];
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

            int w, h;
            SDL_GL_GetDrawableSize(win, &w, &h);
            glViewport(0, 0, w, h);
            glUniform1f(loc_aspect_x, aspect_x);
            glUniform1f(loc_aspect_y, aspect_y);

            float mat[9];
            make_transform(mat, pos_x, pos_y, angle, scale_x, scale_y);
            glUniformMatrix3fv(loc_xform, 1, GL_FALSE, mat);

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