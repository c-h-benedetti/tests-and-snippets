#include "gui-test.hpp"
#include <gtkmm.h>
#include <iostream>

void on_menu_item_activate() {
  // Gestion de la sélection
  std::cout << "Passe dans fonction menu" << std::endl;
}

int gui_test(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    Gtk::Window window;
    window.set_default_size(200, 100);

    Gtk::Box box(Gtk::ORIENTATION_VERTICAL, 5);
    window.add(box);

    // Créer un bouton pour ouvrir le menu
    Gtk::Button button("Ouvrir le menu");
    box.pack_start(button);

    // Créer un menu avec des sous-menus
    Gtk::Menu menu;

    Gtk::MenuItem item1("Option 1");
    menu.append(item1);
    item1.signal_activate().connect(sigc::ptr_fun(on_menu_item_activate));

    Gtk::MenuItem item2("Option 2");
    menu.append(item2);
    item2.signal_activate().connect(sigc::ptr_fun(on_menu_item_activate));

    // Créer un sous-menu pour l'option 3
    Gtk::MenuItem item3("Option 3");
    menu.append(item3);

    Gtk::Menu sub_menu;
    item3.set_submenu(sub_menu);

    Gtk::MenuItem sub_item1("Sous-option 1");
    sub_menu.append(sub_item1);
    sub_item1.signal_activate().connect(sigc::ptr_fun(on_menu_item_activate));

    Gtk::MenuItem sub_item2("Sous-option 2");
    sub_menu.append(sub_item2);
    sub_item2.signal_activate().connect(sigc::ptr_fun(on_menu_item_activate));

    // Connecter le bouton à l'ouverture du menu
    button.signal_clicked().connect([&]() {
    menu.popup_at_widget(&button, Gdk::GRAVITY_SOUTH_WEST, Gdk::GRAVITY_NORTH_WEST, nullptr);
    });

    box.show_all();

    return app->run(window);
}