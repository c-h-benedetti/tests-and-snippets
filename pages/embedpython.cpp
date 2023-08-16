
/*!

\page embedpythonpage ⊹ Notes Python Embeding

|             Topic             |                               Link                                         |
|-------------------------------|----------------------------------------------------------------------------|
| __Releases de CPython__       | https://github.com/python/cpython/tags                                     |
| __Extension et embedding__    | https://docs.python.org/3.10/extending/index.html                          |
| __Empaqueter des binaires__   | https://packaging.python.org/en/latest/guides/packaging-binary-extensions/ |


## 📖 Notes:

- CPython est simplement le code source de Python non-compilé.
- Blender, qui a un des meilleurs Python intégrés, n'utilise rien d'autres que l'API de CPython (pas Cython, cffi, SWIG ou Numba)
- Le header `<Python.h>` doit être inclus avant tout autre module.

## 🚀 Objectifs:

- [X] Arriver à compiler Python et exécuter un script depuis le binaire C++.
- [X] Faire un CMakeLists.txt pour télécharger et compiler CPython.
- [X] Arranger le CMakeLists.txt pour qu'il injecte le build path comme variable de préprocesseur dans le C++.
- [ ] Encapsuler la création et destruction de contexte Python dans un objet plutôt que dans des fonctions.
- [ ] Créer en C++ des objets manipulables depuis Python.
- [ ] Gérer le fait que les paths ne sont pas obligatoirement que du AscII dans `init_python`.
- [ ] Est-il possible d'avoir plusieurs Pythons en même temps vu que les fonctions ont l'air globales ? Cela dépend-il seulement du dernier contexte créé ?
- [ ] Créer un type de données qui peut devenir un tableau `numpy`.
- [ ] Est-ce que certaines données doivent être stockées exclusivement côté C++ ?
- [ ] Fix le CMake pour qu'il n'utilise pas 'configure' à chaque call à `make`.

---

## 🛠️ Compilation de CPython dans du C++:

### 1. Téléchargement de CPython

- La première étape est de télécharger une version stable du code source depuis une release (tag) du dépôt de CPython.
- Ce code est compatible avec l'utilisation de CMake pour la compilation du projet.

\warning Le téléchargement devrait être fait dans le cmake, pas à la volée depuis le github de CPython.   
Il semblerait que ces blocs puissent comprendre plusieurs lignes.   
\code{.cpp}
// header
void GLArea::on_realize();
\endcode

\note Cette fonction doit être appelée avant toute autre opération.

\todo Implémenter la prise en charge de la récupération d'erreurs.

\bug Ce bloc signale la présence d'un bug connu.

\deprecated Ce block indique la présence d'une entité en voie de déprécation (ou déjà supprimée).

\invariant Ceci est un état maintenu par un objet ou un algorithme.

> Ceci est une citation si je ne me trompe pas.

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

### 2. Contenu du CMakeLists.txt

### 3. Configuration des variables d'environnement

#### Création d'entités customs en C++

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