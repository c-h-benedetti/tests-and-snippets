#include "opengl_gtk.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <iostream>
#include <cmath>
#include <chrono>

using namespace std::chrono;

OpenGlCourse::OpenGlCourse(uint v_width, uint v_height) {
    set_has_depth_buffer(true);

    width  = (v_width%2 == 0) ? v_width+1 : v_width;
    height = (v_height%2 == 0) ? v_height+1 : v_height;

    nBands = height - 1;
    nVtces = height * width;
    eboSz  = (2 * height - 2) * width;
}

OpenGlCourse::~OpenGlCourse() {}

void OpenGlCourse::on_realize() {
    Gtk::GLArea::on_realize(); // Don't forget to call the base class implementation
    std::cerr << "realize" << std::endl;
    
    make_current();
    printf("Using OpenGL version: %d\n", epoxy_gl_version());
    printf("Using GLSL version: %d\n", epoxy_glsl_version());
    printf("Max number of attributes: %d\n", GL_MAX_VERTEX_ATTRIBS);
    GLenum error = glGetError();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->make_plan();
}

void OpenGlCourse::on_unrealize() {
    make_current();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    Gtk::GLArea::on_unrealize();
}


void OpenGlCourse::on_resize(int w, int h) {
    Gtk::GLArea::on_resize(w, h);
}


bool OpenGlCourse::on_render(const Glib::RefPtr<Gdk::GLContext>& context) {
    Gtk::GLArea::on_render(context);
    
    context->make_current();
    glViewport(0, 0, get_allocated_width(), get_allocated_height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ultra important de rajouter gl_depth_buffer_bit

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLE_STRIP, eboSz, GL_UNSIGNED_INT, 0);

    return true;
}

void OpenGlCourse::make_plan_geometry(float* vbo, uint* ebo) {
    size_t index = 0;

    for (size_t b = 0 ; b < nBands ; b++){
        for (size_t i = 0 ; i < width ; i++) {
            ebo[index]   = b * width + i;
            ebo[index+1] = width * (b+2) - 1 - i;
            index += 2;
        }
    }

    float incre_x = 2.0 / (static_cast<float>(width) - 1.0);
    float incre_y = 2.0 / (static_cast<float>(height) - 1.0);
    index = 0;
    float x = 0.0, y = 0.0, z = 0.0, factor=0.95;

    for (size_t l = 0 ; l < height ; l++) {
        bool inv = (l%2 == 0);
        for (size_t c = 0 ; c < width ; c++) {
            x = inv ? (-1.0 + static_cast<float>(c) * incre_x) : (1.0 - static_cast<float>(c) * incre_x);
            y = 1.0 - static_cast<float>(l) * incre_y;
            z = 0.0;
            vbo[5*index+0] = x * factor;
            vbo[5*index+1] = y * factor;
            vbo[5*index+2] = z * factor;
            vbo[5*index+3] = (x + 1.0)*0.5;
            vbo[5*index+4] = (y + 1.0)*0.5;
            index++;
        }
    }
}

void OpenGlCourse::make_plan() {

    float vbo[5*nVtces] = {0.0};
    unsigned int indices[eboSz] = {0};

    this->make_plan_geometry(vbo, indices);

    // Creating Vertex Shader
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 vertex_position;\n"
        "layout (location = 1) in vec2 uv_position;\n"
        "out vec3 v_color;\n"
        "void main() {\n"
        "   gl_Position = vec4(vertex_position, 1.0);\n"
        "   v_color = vec3(uv_position, 0.5);\n"
        "}\n";


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    this->check_compilation(vertexShader, 0, "VertexShader");

    // Creating Fragment Shader
    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "in vec3 v_color;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   color = vec4(v_color, 1.0);\n"
        "}\n";
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    this->check_compilation(fragmentShader, 0, "FragmentShader");


    // Link shaders into a program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    this->check_compilation(shaderProgram, 1, "ShaderProgram");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Creating and binding a VAO, a VBO and a EBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Change la machine à état. Tous les calls sur GL_ARRAY_BUFFER vont affecter ce VBO.
    glBufferData(GL_ARRAY_BUFFER, 5*nVtces*sizeof(float), vbo, GL_STATIC_DRAW); // Fonction pour transférer des données dans la VRAM

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboSz*sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    

    vertexColorLocation = glGetUniformLocation(shaderProgram, "someFloat");

    // Unbinding buffers, for safety
    // Le VAO doit être le premier à être unbound. Sinon, il va record le unbind des autres buffers.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// type == 0: shader
// type == 1: programm
bool OpenGlCourse::check_compilation(uint i, int type, const char* who) const {
    int  success;
    char infoLog[512];

    if (type == 0){
        glGetShaderiv(i, GL_COMPILE_STATUS, &success);
    }
    else {
        glGetProgramiv(i, GL_LINK_STATUS, &success);
    }
    
    if(!success) {
        if (type == 0) {
            glGetShaderInfoLog(i, 512, NULL, infoLog);
        }
        else {
            glGetProgramInfoLog(i, 512, NULL, infoLog);
        }
        
        std::cerr << "ERROR::" << who << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    std::cerr << "Shader or program successfully compiled." << std::endl;
    return true;
}