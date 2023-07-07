#ifndef OPENGL_COURSE_WITH_GTK_HPP_INCLUDED
#define OPENGL_COURSE_WITH_GTK_HPP_INCLUDED

#include <random>
#include <gtkmm/glarea.h>
#include "shaders.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class OpenGlCourse : public Gtk::GLArea {

    unsigned int VBO=0;
    unsigned int EBO=0;
    unsigned int VAO=0;
    ProgramShader shaderProgram;

    unsigned int texture;
    unsigned int points_texture;
    
    int ptLoc_location=-1;
    int threshold_location=-1;
    float last_x=0.0;
    float last_y=0.0;
    double last=0.0;
    float th=0.05f;
    int srt_mat_location=-1;
    glm::mat4 loc = glm::mat4(1.0f);
    glm::vec3 dir = glm::vec3(0.001f, 0.0f, 0.0f);

    size_t height;
    size_t width;
    uint nBands;
    uint nVtces;
    uint eboSz;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;

protected:

    void make_plan_geometry(float* vbo, uint* ebo);
    void set_uniforms();

public:

    OpenGlCourse(uint v_width, uint v_height);
    virtual ~OpenGlCourse();

    void make_plan();
    bool animate(const Glib::RefPtr<Gdk::FrameClock>& frame_clock);

public:

    void on_realize() override;
    void on_unrealize() override;
    bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
    void on_resize(int w, int h) override;
};


#endif // OPENGL_COURSE_WITH_GTK_HPP_INCLUDED