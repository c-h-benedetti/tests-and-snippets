#ifndef EXPERIMENTAL_HPP_INCLUDED
#define EXPERIMENTAL_HPP_INCLUDED

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class HelloWorld : public Gtk::Window {

public:

    HelloWorld();
    virtual ~HelloWorld();

protected:

    //Signal handlers:
    void on_button_clicked();

protected:

    //Member widgets:
    Gtk::Button m_button;
};



int experimental (int argc, char *argv[]);

#endif // EXPERIMENTAL_HPP_INCLUDED