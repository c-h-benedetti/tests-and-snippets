#ifndef SEVERAL_WINDOWS_HPP_INCLUDED
#define SEVERAL_WINDOWS_HPP_INCLUDED

#include <gtkmm.h>
#include <map>

class NewWindow : public Gtk::Window {

public:

    NewWindow(int c);
    virtual ~NewWindow() = default;
};

//////////////////////////////////////////////////////////////////
// Ouvrir plusieurs fenêtres avec gtkmm. Une est la principale, l'autre est subordonnée.

class MainWindow : public Gtk::Window {

public:

    MainWindow();
    virtual ~MainWindow() = default;

protected:

    void on_button_clicked();

private:

    std::map<NewWindow*, int> instances;
    int count = 0;
    Gtk::Button button;
};

int severalWindows(int argc, char *argv[]);

#endif // SEVERAL_WINDOWS_HPP_INCLUDED