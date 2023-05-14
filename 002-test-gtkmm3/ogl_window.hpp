#ifndef OPENGL_WINDOW_BASE_HPP_INCLUDED
#define OPENGL_WINDOW_BASE_HPP_INCLUDED

#include "minimal_gtk_epoxy.hpp"
#include <gtkmm.h>

class OGLWindow final : public Gtk::Window {

    GLArea glArea;

public:

    OGLWindow();
    ~OGLWindow() = default;
};

int glAreaEopxy(int argc, char* argv[]);

#endif // OPENGL_WINDOW_BASE_HPP_INCLUDED