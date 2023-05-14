#include "ogl_window.hpp"

OGLWindow::OGLWindow() {
    this->set_title("OpenGL with Gtkmm and Epoxy");
    this->set_default_size(1440, 1440);
    this->add(glArea);
    this->show_all_children();
}


int glAreaEopxy(int argc, char* argv[]) {
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    OGLWindow window;

    return app->run(window);
}