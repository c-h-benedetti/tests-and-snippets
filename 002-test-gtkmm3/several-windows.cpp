#include "several-windows.hpp"


NewWindow::NewWindow() {
    set_default_size(200, 100);
    set_title("New Window");
}

//////////////////////////////////////////////////////////////////
// Ouvrir plusieurs fenêtres avec gtkmm. Une est la principale, l'autre est subordonnée.

MainWindow::MainWindow() {
    set_default_size(300, 200);
    set_title("Main Window");

    button.set_label("Open new window");
    button.signal_clicked().connect([this](){ on_button_clicked(); });


    add(button);
    show_all_children();
}

void MainWindow::on_button_clicked() {
    auto new_window = new NewWindow();
    new_window->show();
    new_window->signal_hide().connect([new_window]() { delete new_window; });
}

int severalWindows(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    MainWindow main_win;
    return app->run(main_win);
}