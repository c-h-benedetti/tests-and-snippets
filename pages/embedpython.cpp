
/*!

\page embedpythonpage ⊹ Notes Python Embeding

|             Topic                       |                               Link                                         |
|-----------------------------------------|----------------------------------------------------------------------------|
| __Releases de CPython__                 | https://github.com/python/cpython/tags                                     |
| __Extension et embedding__              | https://docs.python.org/3.10/extending/index.html                          |
| __Empaqueter des binaires__             | https://packaging.python.org/en/latest/guides/packaging-binary-extensions/ |
| __Explication linking modules CPython__ | https://thomasnyberg.com/what_are_extension_modules.html                   |
| __Tips CPython & C++__                  | https://thomasnyberg.com/cpp_extension_modules.html                        |
| __Utiliser Numpy & C++__                | https://pythonextensionpatterns.readthedocs.io/en/latest/cpp.html          |
| __C++ & CPython__                       | https://geghamjivanyan.medium.com/c-c-extension-for-python-f68444dab472    |


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

\warning Le téléchargement devrait être fait dans le CMakeLists.txt, pas à la volée depuis le github de CPython.

- Un CMakeLists.txt (presque) générique se trouve dans le dossier consacré à CPython de ce repository.

### 2. Contenu du CMakeLists.txt

### 3. Configuration des variables d'environnement

### 4. Création d'entités customs en C/C++

\warning Le nom de la fonction `PyInit_[name]` est important. En effet, lors du chargement des librairies (qui se fait de façon dynamique), la procédure suivante est utilisée:
- **Step 1:** Find and open a shared library file called `[name].so`.
- **Step 2:** Find and load in a function called `PyInit_[name]`.
- **Step 3:** Execute that function.
- **Step 4:** Return control back to interpreter.

*/