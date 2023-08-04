#ifndef OPENGL_WINDOW_BASE_HPP_INCLUDED
#define OPENGL_WINDOW_BASE_HPP_INCLUDED

#include "minimal_gtk_epoxy.hpp"
#include "opengl_gtk.hpp"
#include <gtkmm.h>

class OGLWindow final : public Gtk::Window {

    GLArea glArea;

public:

    OGLWindow();
    ~OGLWindow() = default;
};


class CourseWindow final : public Gtk::Window {

    OpenGlCourse glArea;
    size_t window_height;
    size_t window_width;

public:

    CourseWindow(size_t h, size_t w);
    ~CourseWindow() = default;
};


int glAreaEopxy(int argc, char* argv[]);
int courseOpenGL(int argc, char* argv[]);

#endif // OPENGL_WINDOW_BASE_HPP_INCLUDED