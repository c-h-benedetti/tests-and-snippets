#include "minimal_gtk_epoxy.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <iostream>


GLArea::GLArea() {
    set_has_depth_buffer(true);
}

GLArea::~GLArea() {}

void GLArea::on_realize() {
    Gtk::GLArea::on_realize(); // Don't forget to call the base class implementation
    std::cerr << "realize" << std::endl;
    
    make_current();
    printf("Using OpenGL version: %d\n", epoxy_gl_version());
    printf("Using GLSL version: %d\n", epoxy_glsl_version());
    GLenum error = glGetError();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    this->make_triangle();
}

void GLArea::on_unrealize() {
    make_current();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    Gtk::GLArea::on_unrealize();
}


void GLArea::on_resize(int w, int h) {
    Gtk::GLArea::on_resize(w, h); // Si on call pas cette méthode, rien n'est jamais rendu.
    std::cerr << w << " " << h << std::endl;
}


bool GLArea::on_render(const Glib::RefPtr<Gdk::GLContext>& context) {
    Gtk::GLArea::on_render(context); 
    
    context->make_current();
    glViewport(0, 0, get_allocated_width(), get_allocated_height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ultra important de rajouter gl_depth_buffer_bit

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // revert: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    return true;
}

void GLArea::make_triangle() {

    float vertices[] = {
        -0.9, -0.9, 0.0,
        0.9, -0.9, 0.0,
        0.9, 0.9, 0.0,
        -0.9, 0.9, 0.0
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    }; 

    // Creating Vertex Shader
    const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main() {\n"
        "   gl_Position = vec4(position, 1.0);\n"
        "}\n";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    this->check_compilation(vertexShader, 0, "VertexShader");


    // Creating Fragment Shader
    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   color = vec4(1.0, 0.5, 0.2, 1.0);\n"
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Fonction pour transférer des données dans la VRAM

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbinding buffers, for safety
    // Le VAO doit être le premier à être unbound. Sinon, il va record le unbind des autres buffers.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// type == 0: shader
// type == 1: programm
bool GLArea::check_compilation(uint i, int type, const char* who) const {
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