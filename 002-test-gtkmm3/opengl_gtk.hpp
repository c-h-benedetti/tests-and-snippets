#ifndef OPENGL_COURSE_WITH_GTK_HPP_INCLUDED
#define OPENGL_COURSE_WITH_GTK_HPP_INCLUDED

#include <gtkmm/glarea.h>
#include "shaders.hpp"

class OpenGlCourse : public Gtk::GLArea {

    unsigned int VBO=0;
    unsigned int EBO=0;
    unsigned int VAO=0;
    ProgramShader shaderProgram;
    int vertexColorLocation=0;
    double last=0.0;

    size_t height;
    size_t width;
    uint nBands;
    uint nVtces;
    uint eboSz;

protected:

    void make_plan_geometry(float* vbo, uint* ebo);

public:

    OpenGlCourse(uint v_width, uint v_height);
    virtual ~OpenGlCourse();

    void make_plan();

public:

    void on_realize() override;
    void on_unrealize() override;
    bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
    void on_resize(int w, int h) override;
};


#endif // OPENGL_COURSE_WITH_GTK_HPP_INCLUDED