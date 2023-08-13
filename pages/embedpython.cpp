
/*!

\page embedpythonpage ⊹ Notes Python Embeding

- __Releases de CPython:__ https://github.com/python/cpython/tags
- __Extension et embedding:__ https://docs.python.org/3.10/extending/index.html
- __Empaqueter des binaires:__ https://packaging.python.org/en/latest/guides/packaging-binary-extensions/

## Notes:

- CPython est simplement le code source de Python non-compilé.
- Blender, qui a un des meilleurs Python intégrés, n'utilise rien d'autres que l'API de CPython (pas Cython, cffi, SWIG ou Numba)
- Le header `<Python.h>` doit être inclus avant tout autre module.

## Objectifs:

- 🔘 Arriver à compiler Python et exécuter un script depuis le binaire C++.
- ⚪️ Faire un CMakeLists.txt pour télécharger et compiler CPython.
- ⚪️ Pouvoir télécharger et compiler CPython depuis le CMake.
- ⚪️ Créer des objets manipulables en Python depuis C++.*
- ⚪️ Gérer le fait que les paths ne sont pas obligatoirement que du ascii dans `init_python`.
- ⚪️ Est-il possible d'avoir plusieurs Pythons en même temps vu que les fonctions ont l'air globales ? Cela dépend-il seulement du dernier contexte créé ?
- ⚪️ Créer un type de données qui peut devenir un tableau `numpy`.

---

## 📌 Compilation de CPython:

Le code minimal pour un setup OpenGL qui dessine un rectangle est dans: `002-test-gtkmm3/minimal_gtk_epoxy.cpp`.

### 1. Setup d'OpenGL

- Le signal `on_realize` est émis quand le widget a bien été assigné et qu'il est prêt à être déssiné. C'est dans cette méthode qu'il faut déclarer et remplir les VBOs, EBOs, ... 
- Cette méthode est virtuelle dans `Gtk::GLArea` et doit être `override` dans notre classe. De plus, la première chose qu'on doit y faire est de faire appel à la méthode du parent.
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