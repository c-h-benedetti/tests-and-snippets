#ifndef GTKMM_EPOXY_MINIMAL_HPP_INCLUDED
#define GTKMM_EPOXY_MINIMAL_HPP_INCLUDED

#include <gtkmm/glarea.h>

class GLArea : public Gtk::GLArea {

    unsigned int VBO=0;
    unsigned int EBO=0;
    unsigned int VAO=0;
    unsigned int shaderProgram=0;

protected:

    bool check_compilation(uint i, int type, const char* who) const;

public:

    GLArea();
    virtual ~GLArea();
    void make_triangle();

public:

    void on_realize() override;
    void on_unrealize() override;
    bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
    void on_resize(int w, int h) override;
};


#endif // GTKMM_EPOXY_MINIMAL_HPP_INCLUDED