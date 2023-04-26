#include <gtkmm.h>
#include "Experimental.hpp"
#include "several-windows.hpp"

// Cours/tuto: https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en

int minimal(int argc, char *argv[], char* env[]) {
  auto app =
    Gtk::Application::create(argc, argv,
        "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    return app->run(window);
}




int main(int argc, char* argv[]) {
  return severalWindows(argc, argv);
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