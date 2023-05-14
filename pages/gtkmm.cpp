
/*!

\page gtmmpage ⊹ Notes Gtkmm3

- __Tutoriel officiel:__ https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en
- __Tests Epoxy:__ https://github.com/anholt/libepoxy/tree/master/test

## Notes:

- `<gtkmm.h>` inclus tous les headers de gtkmm. Ça peut être utile pour les tests, mais pour les versions finales, il vaut mieux ne garder que les headers nécessaires.
- Le string en troisième argument de la fonction `Gtk::Application::create` correspond à l'identifiant unique de l'application, qui permet de s'assurer qu'une seule instance de l'application est lancée à la fois. Si une deuxième instance est lancée avec le même identifiant, elle ne sera pas lancée et la première instance sera mise en avant-plan. Cet identifiant est généralement une chaîne de caractères unique pour chaque application, par exemple le nom de l'entreprise ou de l'organisation suivi du nom de l'application.
- Quand on veut connecter une fonction de callback à un signal, passer une \link lambdaexprpage lambda expression \endlink a l'air d'être une bien meilleure alternative dans beaucoup de cas, plutôt que d'utiliser `sigc::mem_fun()`. On peut même écrire le corps de la fonction directement dans le code du connect.
- Si on doit passer des arguments à la ligne de commande, il faut que l'objet application reçoive une autre version car sinon il va essayer de lire des paramètres qu'il n'est pas censé voir.

## Objectifs:

- 🔘 Faire un CMakeLists.txt pour utiliser gtkmm.
- 🔘 Pouvoir ouvrir une fenêtre principale et des fenêtres secondaires.
- 🔘 Générer une fenêtre qui contient un contexte OpenGL et Epoxy.
- 🔘 Faire un setup pour utiliser OpenGL.
- ⚪️ Pour zoomer et translater dans la fenêtre affichant une image.
- ⚪️ Être capable d'afficher des boutons dans la barre de titre.
- ⚪️ Être capable d'ouvrir des menus.

---

## 📌 Gtk::GLArea:

Le code minimal pour un setup OpenGL qui dessine un rectangle est dans: `002-test-gtkmm3/minimal_gtk_epoxy.cpp`.

### 1. Setup d'OpenGL

- Le signal `on_realize` est émis quand le widget a bien été assigné et qu'il est prêt à être déssiné. C'est dans cette méthode qu'il faut déclarer et remplir les VBOs, EBOs, ... 
- Cette méthode est virtuelle dans `Gtl::GLArea` et doit être `override` dans notre classe. De plus, la première chose qu'on doit y faire est de faire appel à la méthode du parent.
- Le contexte OpenGL est ici valide.

\code{.cpp}
// header
void on_realize() override;

// implémentation
void GLArea::on_realize() {
    Gtk::GLArea::on_realize(); // À ne pas oublier !!!
    make_current();
    GLenum error = glGetError();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
\endcode

### 2. Setup de rendu basique

#### Étapes pour un rendu

- Créer les données à rendre (vertices, faces, uv, couleurs, ...)
- Créer les shaders, les compiler (vérifier le status de compilation), les assembler en programme et libérer les shaders de base.
- Générer un VAO et le bind avant de continuer.
- Générer un VBO, le bind, et y transférer les données de vertices.
- Générer un EBO, le bind, et y transférer les données de faces.
- Renseigner les informations d'interprétation du contenu du VBO (offset, stride, ...) et activer les attributs qui y sont contenus.
- Unbind le VAO __puis__ tous les autres buffers. Procédure (et ordre à suivre) pour debind les buffers:

\code{.cpp}
glBindVertexArray(0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
\endcode

#### Notes

- Ne pas oublier le tag `GL_DEPTH_BUFFER_BIT` dans `glClear()` si `set_has_depth_buffer(true)` a été utilisé dans le constructeur. Sinon, rien ne sera rendu à l'écran sauf si la fenêtre est resized.
- Quand on fait un setup de rendu, le VAO doit être le premier unbound, sinon, il record le fait qu'on fasse debind les buffers.
- Dans la fonction `glVertexAttribPointer`, le premier index est celui qui doit être donné à `glEnableVertexAttribArray` ainsi que celui dans le shader dans `(layout = N)`.

### 3. Le rendu

- Les buffers sont automatiquement swapped dans `on_render()`.

### 4. Destruction du contexte

- `on_unrealize` est émis quand le contexte OpenGL est encore valide, juste avant que l'objet de soit détruit. C'est le moment de détruire toutes les ressources OpenGL (qui ne seront plus accessible quand le contexte OpenGL ne sera plus valide).

## 📌 Epoxy:

- Équivalent de GLAD ou même GLEW, permet de faciliter l'utilisation de fonctions OpenGL.
- Le header doit absolument être le premier de son unité de compilation. La solution trouvée a été de mettre Epoxy dans le .cpp uniquement. Configuration utilisée:

\code{.cpp}
// main.cpp
#include <vector>
#include <gtkmm.h>
#include "header.hpp"
#include "other-header.hpp"

// header
#include <gtkmm/glarea.h>

// implementation
#include "header.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
\endcode

*/