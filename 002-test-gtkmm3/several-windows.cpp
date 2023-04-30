#include "several-windows.hpp"
#include <iostream>

NewWindow::NewWindow(int c) {
    set_default_size(400, 100);
    set_title("New Window " + std::to_string(c));
}

//////////////////////////////////////////////////////////////////
// Ouvrir plusieurs fenêtres avec gtkmm. Une est la principale, l'autre est subordonnée.

MainWindow::MainWindow() {
    set_default_size(300, 200);
    set_title("Main Window");
    set_border_width(10);

    button.set_label("Open new window");
    button.signal_clicked().connect([this](){ on_button_clicked(); });


    add(button);
    show_all_children();
}

void MainWindow::on_button_clicked() {
    auto new_window = new NewWindow(++count);
    this->instances[new_window] = count;
    new_window->show();
    
    new_window->signal_hide().connect([this, new_window]() {
        auto it = this->instances.find(new_window);
        std::cout << "Closing window: " << (it->second) << std::endl;
        delete it->first;
        this->instances.erase(it);
        // delete new_window;
    });
}

int severalWindows(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    MainWindow main_win;
    return app->run(main_win);
}