#include "spammodule.hpp"
#include <string>
#include <cstring>
#include <iostream>

namespace ANSI {
    static const char* reset = "\033[0m";

    enum class color: int {
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white
    };

    static const char* codes_fg[] = {
        "\033[30m",
        "\033[31m",
        "\033[32m",
        "\033[33m",
        "\033[34m",
        "\033[35m",
        "\033[36m",
        "\033[37m"
    };

    static const char* codes_bg[] = {
        "\033[40m",
        "\033[41m",
        "\033[42m",
        "\033[43m",
        "\033[44m",
        "\033[45m",
        "\033[46m",
        "\033[47m"
    };
};


/**
 * @brief Function allowing the user to print some text in the standard output given desired colors.
 * @warning fg, sep and bg arguments are optional, and should be passed as keywords if they are required.
 * @todo Modify this function for it to accept an arbitrary number of strings before the keywords.
 */
static PyObject* spam_super_print(PyObject* self, PyObject* args, PyObject *keywds) {
    
    int fg=-1, bg=-1;
    const char* input = nullptr;
    static char* kwlist[] = {"input", "fg", "bg", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|ii", kwlist, &input, &fg, &bg)) {
        return NULL;
    }

    std::string total = input;
    total += ANSI::reset;

    if (fg > -1) {
        total = ANSI::codes_fg[fg] + total;
    }

    if (bg > -1) {
        total = ANSI::codes_bg[bg] + total;
    }

    std::cout << total << std::endl;

    Py_RETURN_NONE;
}


static PyObject* spam_replace_chars(PyObject* self, PyObject* args) {
    int start=-1, end=-1;
    const char *input, *replace;

    if (!PyArg_ParseTuple(args, "s|sii", &input, &replace, &start, &end)) { // The pipe marks the begining of optional parameters.
        PyErr_SetString(PyExc_ValueError, "Invalid arguments found.");
        return NULL;
    }

    std::string input_str = input;
    char c = (replace == nullptr || strlen(replace) == 0) ? (' ') : (replace[0]);
    size_t first = (start < 0) ? 0 : size_t(start);
    size_t last  = (end == -1 || end >= input_str.size()) ? input_str.size() - 1 : size_t(end);

    for (size_t i = first ; i <= last ; i++) {
        input_str[i] = c;
    }

    return PyUnicode_FromString(input_str.c_str());
}


/**
 * @brief [CPython (spam)] Takes a shell command, runs it and returns its status code.
 * @return A number representing the command's execution's status code.
 */
static PyObject* spam_system(PyObject* self, PyObject* args) {
    const char* command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }
    sts = system(command);
    return PyLong_FromLong(sts);
}


/**
 * @brief Function processing the factorial of a given integer. Follows the recursive implementation.
 * @param n The number from which we will process the factorial.
 * @warning There is no safety check in case the input is lower than 0.
 * @return The factorial of the given input.
 */
long int factorial(long int n) {
    if (n == 0) { return 1; }
    return n * factorial(n-1);
}


/**
 * @brief [CPython (spam)]
 */
static PyObject* spam_factorial(PyObject* self, PyObject* args) {
    long int base;
    long int result;

    if (!PyArg_ParseTuple(args, "l", &base)) {
        return NULL;
    }

    if (base < 0) {
        PyErr_SetString(PyExc_ValueError, "Value must be non-negative");
        return NULL;
    }

    result = factorial(base);
    return PyLong_FromLong(result);
}


/**
 * @brief Defines the methods available in the `spam` module.
 * This array is terminated by an element containing null elements.
 * 1. Name that the function will have in Python
 * 2. Which C function is it bound to.
 * 3. ?
 * 4. Description of the function.
 */
static PyMethodDef SpamMethods[] = {
    {"system", spam_system, METH_VARARGS, "Runs a shell command!"},
    {"factorial", spam_factorial, METH_VARARGS, "Calculates the factorial of a number."},
    {"replace_range", spam_replace_chars, METH_VARARGS, "Replaces characters in a certain range."},
    {"super_print", (PyCFunction)(void(*)(void))spam_super_print, METH_VARARGS | METH_KEYWORDS, "Prints some text in std::out with a given color."},
    {NULL, NULL, 0, NULL}
};


/**
 * @brief Description of the module itself (its name, its methods, ...)
 */
static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",
    "This module contains miscellaneous functions illustrating the usage of the main principles from the CPython API.",
    -1,
    SpamMethods
};

/** 
 * @brief Function used to instanciate our module in an instance of Python interpreter.
 * @warning The initialization function must be named PyInit_name() (avec name qui est le nom du module), le nommage est important.
 *          C'est le seul élément non-statique des composants du module.
 */
PyMODINIT_FUNC PyInit_spam(void) {
    return PyModule_Create(&spammodule);
}

/*

- À quoi sert METH_VARARGS dans l'instance de PyMethodDef ?
- Que sont les différents arguments dans l'instance de PyModuleDef ?
- Rappel sur le fonctionement de l'ownership et le compteur d'utilisations ?

-------------------------------------

- [ ] Fonction avec des paramètres optionnels
- [ ] Fonction qui modifie in-place un objet Python
- [ ] Fonction qui appelle une autre fonction accessible depuis Python.
- [ ] Fonction qui utilise des paramètres en keywords.

*/

/*

- [ ] Créer un objet qui est un "contexte" et qui permet d'exécuter à la suite plusieurs fonctions en restant côté C.
      On ne récupèrera que le résultat final côté Python.
      Les opérations dans le context doivent être chainées. (ou graphée ?)
      Dans le contexte du traitement d'images, la chaine sera appliquée à chaque pixel.
      La structure devrait peut-être être une liste chainée avec des conditionnelles possibles ?

*/