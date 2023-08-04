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



CourseWindow::CourseWindow(size_t h, size_t w): glArea(h, w) {
    this->set_title("OpenGL with Gtkmm and Epoxy");
    this->set_default_size(w, h);
    this->add(glArea);
    this->show_all_children();
    exercice_001(glArea);
}


int courseOpenGL(int argc, char* argv[]) {
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    CourseWindow window(1440, 1854);

    return app->run(window);
}
