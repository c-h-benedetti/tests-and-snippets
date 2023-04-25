#include <gtkmm.h>

// Cours/tutto: https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en

int minimal(int argc, char *argv[], char* env[]) {
  auto app =
    Gtk::Application::create(argc, argv,
        "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    return app->run(window);
}


class NewWindow : public Gtk::Window {
public:
  NewWindow() {
    set_default_size(200, 100);
    set_title("New Window");
  }

  virtual ~NewWindow() {}
};

class MainWindow : public Gtk::Window {
public:
  MainWindow() {
    set_default_size(300, 200);
    set_title("Main Window");

    button.set_label("Open new window");
    button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked));

    add(button);
    show_all_children();
  }

  virtual ~MainWindow() {}

protected:
  void on_button_clicked() {
    auto new_window = new NewWindow();
    new_window->show();
    new_window->signal_hide().connect([new_window]() { delete new_window; });
  }

private:
  Gtk::Button button;
};

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  MainWindow main_win;
  return app->run(main_win);
}



/**
 * \page rootpage TESTS GTKMM
 * 
 * __Tutoriel officiel:__ https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en
 * 
 * ## Notes:
 * 
 * - `<gtkmm.h>` inclus tous les headers de gtkmm. Ça peut être utile pour les tests, mais pour les versions finales, il vaut mieux ne garder que les headers nécessaires.
 * - Le string en troisième argument de la fonction Gtk::Application::create correspond à l'identifiant unique de l'application, qui permet de s'assurer qu'une seule instance de l'application est lancée à la fois. Si une deuxième instance est lancée avec le même identifiant, elle ne sera pas lancée et la première instance sera mise en avant-plan. Cet identifiant est généralement une chaîne de caractères unique pour chaque application, par exemple le nom de l'entreprise ou de l'organisation suivi du nom de l'application
 * - Glade et `Gtk::Builder` permettent de ne pas avoir à gérer l'apparence de la GUI depuis C++.
 * 
 * ## Objectifs:
 * 
 * - ◯ Pouvoir ouvrir une fenêtre principale et des fenêtres secondaires.
 * - ◯ Pour zoomer et translater dans la fenêtre affichant une image.
 * - ◯ Être capable d'afficher des boutons dans la barre de titre.
 * - ◯ Être capable d'ouvrir des menus.
 * - ⬤ Faire un CMakeLists.txt pour utiliser gtkmm.
 * 
 */