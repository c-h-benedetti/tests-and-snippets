
#include <gtkmm.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>

#include <iostream>
#include "Experimental.hpp"
#include "several-windows.hpp"
#include "gui-test.hpp"
#include "glarea.hpp"

// Cours/tuto: https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en

int minimal(int argc, char *argv[], char* env[]) {
  auto app =
    Gtk::Application::create(argc, argv,
        "org.gtkmm.examples.base");

    Gtk::Window window;
    window.set_default_size(200, 200);

    return app->run(window);
}

enum Test {
  NONE,
  WINDOWS,
  GUI,
  OPENGL
};

int _main(int argc, char* argv[]) {
  Test mode = Test::OPENGL;
  int result = 0;

  switch (mode) {
    case(Test::NONE):
      std::cout << "No test was selected" << std::endl;
    break;
    case(Test::WINDOWS):
      result = severalWindows(argc, argv);
    break;
    case(Test::GUI):
      result = gui_test(argc, argv);
    break;
    case(Test::OPENGL):
      result = launchexample(argc, argv);
    break;
    default:
      std::cout << "The provided test is unknwown" << std::endl;
  };

  return result;
}




class GLArea : public Gtk::GLArea {
public:
    GLArea();
    virtual ~GLArea();

public:
    void on_realize() override;
    void on_realize_signal();
    bool on_render(const Glib::RefPtr<Gdk::GLContext>& context) override;
};

GLArea::GLArea() {
    set_has_depth_buffer(true);
}

GLArea::~GLArea() {}

void GLArea::on_realize() {
    Gtk::GLArea::on_realize(); // Don't forget to call the base class implementation
}

void GLArea::on_realize_signal() {
    make_current();
    int version = epoxy_gl_version();
    printf("Using OpenGL Version %d\n", version);
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        std::cerr << "==========" << std::endl;
        std::cerr << "Erreur OpenGL : " << error << std::endl;
        std::cerr << "==========" << std::endl;
    }
}

bool GLArea::on_render(const Glib::RefPtr<Gdk::GLContext>& context) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Les buffers sont automatiquement swapped avec gtkmm 3.

    return true;
}

int main(int argc, char *argv[]) {
    
    std::cout << "Gtkmm version: " << GTKMM_MAJOR_VERSION << "." << GTKMM_MINOR_VERSION << "." << GTKMM_MICRO_VERSION << std::endl;
    std::cout << "Epoxy version: " << epoxy_gl_version() << std::endl;
    
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    Gtk::Window window;
    window.set_title("OpenGL with Gtkmm and Epoxy");
    window.set_default_size(800, 600);

    GLArea glArea;
    glArea.signal_realize().connect(sigc::mem_fun(glArea, &GLArea::on_realize_signal));
    window.add(glArea);
    glArea.show();

    return app->run(window);
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