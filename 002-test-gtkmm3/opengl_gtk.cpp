#include "opengl_gtk.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include "stb_image.h"


using namespace std::chrono;

OpenGlCourse::OpenGlCourse(uint v_width, uint v_height): distribution(0.0f, 0.03f) {
    set_has_depth_buffer(true);

    width  = (v_width%2 == 0) ? v_width+1 : v_width;
    height = (v_height%2 == 0) ? v_height+1 : v_height;

    nBands = height - 1;
    nVtces = height * width;
    eboSz  = (2 * height - 2) * width;

    add_tick_callback(sigc::mem_fun(*this, &OpenGlCourse::animate));
}

OpenGlCourse::~OpenGlCourse() {}

void OpenGlCourse::on_realize() {
    Gtk::GLArea::on_realize(); // Don't forget to call the base class implementation
    
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
    shaderProgram.release();

    Gtk::GLArea::on_unrealize();
}


void OpenGlCourse::on_resize(int w, int h) {
    Gtk::GLArea::on_resize(w, h);
}


void OpenGlCourse::set_uniforms() {
    last_x += distribution(generator) - 0.015;
    last_y += distribution(generator) - 0.015;
    th *= 1.0001;
    glUniform3f(ptLoc_location, last_x, last_y, 0.0f);
    glUniform1f(threshold_location, th);
    
    glm::mat4 t(1.0);
    t = glm::scale(t, glm::vec3(0.99, 0.99, 0.99));
    // t = glm::translate(t, glm::vec3(0.1, 0.0, 0.0));
    // loc = glm::rotate(loc, 0.00001f, glm::vec3(0.0, 0.0, 1.0));
    t = loc * t;  
    
    glUniformMatrix4fv(srt_mat_location, 1, GL_FALSE, glm::value_ptr(t));
}


bool OpenGlCourse::animate(const Glib::RefPtr<Gdk::FrameClock>& frame_clock) {
    this->queue_render();
    return true;
}


bool OpenGlCourse::on_render(const Glib::RefPtr<Gdk::GLContext>& context) {
    Gtk::GLArea::on_render(context);
    context->make_current();
    glViewport(0, 0, get_allocated_width(), get_allocated_height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ultra important de rajouter gl_depth_buffer_bit

    shaderProgram.use();
    this->set_uniforms();
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, points_texture);

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
    float x = 0.0, y = 0.0, z = 0.0;

    for (size_t l = 0 ; l < height ; l++) {
        bool inv = (l%2 == 0);
        for (size_t c = 0 ; c < width ; c++) {
            x = inv ? (-1.0 + static_cast<float>(c) * incre_x) : (1.0 - static_cast<float>(c) * incre_x);
            y = 1.0 - static_cast<float>(l) * incre_y;
            z = 0.0;
            vbo[5*index+0] = x;
            vbo[5*index+1] = y;
            vbo[5*index+2] = z;
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

    // Link shaders into a program
    shaderProgram = ProgramShader(
        "../shaders/vertex_shader.glsl", 
        "../shaders/fragment_shader.glsl"
    );

    shaderProgram.use();

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

    ptLoc_location = glGetUniformLocation(shaderProgram.get_id(), "ptLoc");
    threshold_location = glGetUniformLocation(shaderProgram.get_id(), "threshold");

    stbi_set_flip_vertically_on_load(true);

    // Texture
    int t_width, t_height, nChannels;
    u_char* data = stbi_load("/home/clement/Desktop/yeast-cells.png", &t_width, &t_height, &nChannels, 3);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    data = nullptr;

    t_width = 0; t_height = 0; nChannels = 0;
    u_char* data2 = stbi_load("/home/clement/Desktop/points.png", &t_width, &t_height, &nChannels, 0);
    glGenTextures(1, &points_texture);
    glBindTexture(GL_TEXTURE_2D, points_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    stbi_image_free(data2);

    int a = glGetUniformLocation(shaderProgram.get_id(), "txt");
    int b = glGetUniformLocation(shaderProgram.get_id(), "pts");

    glUniform1i(a, 0);
    glUniform1i(b, 1);

    srt_mat_location = glGetUniformLocation(shaderProgram.get_id(), "transfo");

    
    // Unbinding buffers, for safety
    // Le VAO doit être le premier à être unbound. Sinon, il va record le unbind des autres buffers.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*

- [ ] Bouger le code pour obtenir l'emplacement des uniforms à l'intérieur de la classe des shaders.
- [ ] Activer automatiquement le shader quand on veut changer la valeur d'un uniform.

*/