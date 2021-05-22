#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include "./la.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

const char *vert_shader_source =
    "#version 330 core\n"
    "\n"
    "#define PARTICLE_SIZE 1.0\n"
    "\n"
    "uniform vec2 resolution;\n"
    "\n"
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in vec3 color;\n"
    "\n"
    "out vec3 particle_color;\n"
    "\n"
    "vec2 screen_to_ndc(vec2 pos) {\n"
    "    return (pos - resolution * 0.5) / (resolution * 0.5);\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 uv = vec2(\n"
    "        float(gl_VertexID & 1),\n"
    "        float((gl_VertexID >> 1) & 1));\n"
    "    gl_Position = vec4(\n"
    "       screen_to_ndc(position + uv * PARTICLE_SIZE),\n"
    "       0.0,\n"
    "       1.0);\n"
    "    particle_color = color;\n"
    "}\n"
    ;
const char *frag_shader_source =
    "#version 330 core\n"
    "\n"
    "in vec3 particle_color;\n"
    "\n"
    "void main() {\n"
    "    gl_FragColor = vec4(particle_color, 1.0);\n"
    "}\n"
    ;

const char *shader_type_as_cstr(GLuint shader)
{
    switch (shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

bool compile_shader_source(const GLchar *source, GLenum shader_type, GLuint *shader)
{
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile %s\n", shader_type_as_cstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint *program)
{
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window;
    glViewport(
        width / 2 - SCREEN_WIDTH / 2,
        height / 2 - SCREEN_HEIGHT / 2,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
}

GLuint program = 0;
GLint resolutionUniform = 0;

void init_shaders(void)
{
    GLuint vert_shader = 0;
    if(!compile_shader_source(vert_shader_source, GL_VERTEX_SHADER, &vert_shader)) {
        exit(1);
    }

    GLuint frag_shader = 0;
    if (!compile_shader_source(frag_shader_source, GL_FRAGMENT_SHADER, &frag_shader)) {
        exit(1);
    }

    if (!link_program(vert_shader, frag_shader, &program)) {
        exit(1);
    }

    glUseProgram(program);

    resolutionUniform = glGetUniformLocation(program, "resolution");
}

typedef enum {
    POSITION_ATTRIB = 0,
    COLOR_ATTRIB,
    COUNT_ATTRIBS
} Attribs;

typedef struct {
    // POSITION_ATTRIB
    GLfloat x;
    GLfloat y;

    // COLOR_ATTRIB
    GLfloat r;
    GLfloat g;
    GLfloat b;
} Vert;

#define VERTS_CAPACITY (16 * 1024)
Vert verts[VERTS_CAPACITY];
size_t verts_count = 0;

GLuint vao = 0;
GLuint vbo = 0;

void init_buffers(void)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(verts),
                 verts,
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(POSITION_ATTRIB);
    glVertexAttribPointer(
        POSITION_ATTRIB,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(verts[0]),
        (GLvoid*) 0);
    glVertexAttribDivisor(POSITION_ATTRIB, 1);

    glEnableVertexAttribArray(COLOR_ATTRIB);
    glVertexAttribPointer(
        COLOR_ATTRIB,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(verts[0]),
        (GLvoid*) (sizeof(GLfloat) * 2));
    glVertexAttribDivisor(COLOR_ATTRIB, 1);
}

void vert(float x, float y, float r, float g, float b)
{
    assert(verts_count < VERTS_CAPACITY);
    verts[verts_count].x = x;
    verts[verts_count].y = y;
    verts[verts_count].r = r;
    verts[verts_count].g = g;
    verts[verts_count].b = b;
    verts_count += 1;
}

void begin_verts(void)
{
    verts_count = 0;
}

void end_verts(void)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        verts_count * sizeof(verts[0]),
        verts);
}

float rand_float(void)
{
    return (float) rand() / (float) RAND_MAX;
}

typedef struct {
    Vec2 x;                     // position
    Vec2 v;                     // velocity
    float mass;
    float padding;
} Particle;

typedef struct {
    Vec2 v;
    float mass;
    float padding;
} Cell;

#define PARTICLES_CAPACITY VERTS_CAPACITY
Particle particles[PARTICLES_CAPACITY] = {0};
size_t particles_count = 0;
#define GRID_RES 512
Cell grid[GRID_RES][GRID_RES] = {0};

void mpm_start(void)
{
    const float spacing = 1.0f;
    const int box_x = 128, box_y = 128;
    const float sx = GRID_RES / 2.0f, sy = GRID_RES / 2.0f;

    for (float i = sx - box_x / 2; i < sx + box_x / 2; i += spacing) {
        for (float j = sy - box_y / 2; j < sy + box_y / 2; j += spacing) {
            assert(particles_count < PARTICLES_CAPACITY);
            particles[particles_count].x = vec2(i, j);
            particles[particles_count].v =
                vec2_mul(
                    vec2(rand_float() - 0.5f,
                         rand_float() - 0.5f + 2.75f),
                    vec2s(0.5f));
            particles[particles_count].mass = 1.0f;
            particles_count += 1;
        }
    }
}

// dt = the time step of our simulation. the stability of your simulation is going to be limited by how much a particle can
// move in a single time step, and it's a good rule of thumb to choose dt so that no
// particle could move more than 1 grid-cell in a single step. (this would lead to particle tunneling, or other very unstable behaviour)
const float dt = 1.0f;

const float gravity = -0.05f;

void mpm_simulate(void)
{
    Vec2 weights[3] = {0};

    // reset grid scratchpad
    for (size_t i = 0; i < GRID_RES; ++i) {
        for (size_t j = 0; j < GRID_RES; ++j) {
            grid[i][j].mass = 0;
            grid[i][j].v = vec2(0.0f, 0.0f);
        }
    }

    // P2G
    for (size_t i = 0; i < particles_count; ++i) {
        Particle p = particles[i];

        // quadratic interpolation weights
        // uint2 cell_idx = (uint2)p.x;
        iVec2 cell_idx = ivec2_from_vec2(p.x);
        // float2 cell_diff = (p.x - cell_idx) - 0.5f;
        Vec2 cell_diff =
            vec2_sub(
                vec2_sub(p.x, vec2_from_ivec2(cell_idx)),
                vec2(0.5f, 0.5f));
        // weights[0] = 0.5f * math.pow(0.5f - cell_diff, 2);
        weights[0] =
            vec2_mul(
                vec2s(0.5f),
                vec2_pow(
                    vec2_sub(vec2s(0.5f), cell_diff),
                    vec2s(2.0f)));
        // weights[1] = 0.75f - math.pow(cell_diff, 2);
        weights[1] =
            vec2_sub(
                vec2s(0.75f),
                vec2_pow(cell_diff, vec2s(2)));
        // weights[2] = 0.5f * math.pow(0.5f + cell_diff, 2);
        weights[2] =
            vec2_pow(
                vec2s(0.5f),
                vec2_pow(
                    vec2_add(vec2s(0.5f), cell_diff),
                    vec2s(2)));

        for (int gx = 0; gx < 3; ++gx) {
            for (int gy = 0; gy < 3; ++gy) {
                float weight = weights[gx].x * weights[gy].y;

                // uint2 cell_x = math.uint2(cell_idx.x + gx - 1, cell_idx.y + gy - 1);
                iVec2 cell_x = ivec2(cell_idx.x + gx - 1, cell_idx.y + gy - 1);
                // float2 cell_dist = (cell_x - p.x) + 0.5f;
                // Vec2 cell_dist =
                //     vec2_add(
                //         vec2_sub(vec2_from_ivec2(cell_x), p.x),
                //         vec2s(0.5f));
                // float2 Q = math.mul(p.C, cell_dist);
                // ^ ignored

                // MPM course, equation 172
                // float mass_contrib = weight * p.mass;
                float mass_contrib = weight * p.mass;

                // converting 2D index to 1D
                Cell cell = grid[cell_x.x][cell_x.y];
                cell.mass += mass_contrib;

                // cell.v += mass_contrib * (p.v + Q);
                cell.v = vec2_add(cell.v, vec2_mul(vec2s(mass_contrib), p.v));

                grid[cell_x.x][cell_x.y] = cell;
            }
        }
    }

    // grid velocity update
    for (int gx = 0; gx < GRID_RES; ++gx) {
        for (int gy = 0; gy < GRID_RES; ++gy) {
            Cell cell = grid[gx][gy];
            if (cell.mass > 0) {
                // convert momentum to velocity, apply gravity
                // cell.v /= cell.mass;
                cell.v = vec2_div(cell.v, vec2s(cell.mass));
                // cell.v += dt * math.float2(0, gravity);
                cell.v =
                    vec2_add(
                        cell.v,
                        vec2_mul(vec2s(dt), vec2(0.0f, gravity)));

                if (gx < 2 || gx > GRID_RES - 3) {
                    cell.v.x = 0;
                }

                if (gy < 2 || gy > GRID_RES - 3) {
                    cell.v.y = 0;
                }

                grid[gx][gy] = cell;
            }
        }
    }

    // G2P
    for (size_t i = 0; i < particles_count; ++i) {
        Particle p = particles[i];

        // reset particle velocity. we calculate it from scratch each step using the grid
        // p.v = 0;
        p.v = vec2s(0.0f);

        // quadratic interpolation weights
        // uint2 cell_idx = (uint2)p.x;
        iVec2 cell_idx = ivec2_from_vec2(p.x);
        // float2 cell_diff = (p.x - cell_idx) - 0.5f;
        Vec2 cell_diff =
            vec2_sub(
                vec2_sub(p.x, vec2_from_ivec2(cell_idx)),
                vec2(0.5f, 0.5f));
        // weights[0] = 0.5f * math.pow(0.5f - cell_diff, 2);
        weights[0] =
            vec2_mul(
                vec2s(0.5f),
                vec2_pow(
                    vec2_sub(vec2s(0.5f), cell_diff),
                    vec2s(2.0f)));
        // weights[1] = 0.75f - math.pow(cell_diff, 2);
        weights[1] =
            vec2_sub(
                vec2s(0.75f),
                vec2_pow(cell_diff, vec2s(2)));
        // weights[2] = 0.5f * math.pow(0.5f + cell_diff, 2);
        weights[2] =
            vec2_mul(
                vec2s(0.5f),
                vec2_pow(
                    vec2_add(vec2s(0.5f), cell_diff),
                    vec2s(2)));

        for (int gx = 0; gx < 3; ++gx) {
            for (int gy = 0; gy < 3; ++gy) {
                float weight = weights[gx].x * weights[gy].y;
                // uint2 cell_x = math.uint2(cell_idx.x + gx - 1, cell_idx.y + gy - 1);
                iVec2 cell_x = ivec2(cell_idx.x + gx - 1, cell_idx.y + gy - 1);

                Vec2 weighted_velocity =
                    vec2_mul(
                        grid[cell_x.x][cell_x.y].v,
                        vec2s(weight));

                p.v = vec2_add(p.v, weighted_velocity);
            }
        }

        p.x = vec2_add(p.x, vec2_mul(p.v, vec2s(dt)));

        // p.x = math.clamp(p.x, 1, grid_res - 2);
        p.x = vec2_clamp(p.x, vec2s(1), vec2s(GRID_RES - 2));

        particles[i] = p;
    }
}

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not initialize GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * const window = glfwCreateWindow(
                                    SCREEN_WIDTH,
                                    SCREEN_HEIGHT,
                                    "MPM",
                                    NULL,
                                    NULL);
    if (window == NULL) {
        fprintf(stderr, "ERROR: could not create a window.\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        fprintf(stderr, "Could not initialize GLEW!\n");
        exit(1);
    }

    if (!GLEW_EXT_draw_instanced) {
        fprintf(stderr, "Support for EXT_draw_instanced is required!\n");
        exit(1);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, window_size_callback);

    init_shaders();
    init_buffers();

    glUniform2f(resolutionUniform, SCREEN_WIDTH, SCREEN_HEIGHT);

    mpm_start();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        mpm_simulate();

        begin_verts();
        {
            for (size_t i = 0; i < particles_count; ++i) {
                vert(particles[i].x.x, particles[i].x.y, 1.0, 0.0, 0.0);
            }
        }
        end_verts();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP,
            0,
            4,
            verts_count);

        glfwSwapBuffers(window);
        glfwPollEvents();

        usleep(1000000 / 60);
    }

    return 0;
}
