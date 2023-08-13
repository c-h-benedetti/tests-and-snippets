#include "spammodule.h"

/*

Dans ce fichier, on veut définir un module spam qui contient la fonction system qui permet de faire un call system.

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

static long int factorial(long int n) {
    if (n == 0) { return 1; }
    return n * factorial(n-1);
}

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


static PyMethodDef SpamMethods[] = {
    {"system", spam_system, METH_VARARGS, "Runs a shell command!"},
    {"factorial", spam_factorial, METH_VARARGS, "Calculates the factorial of a number."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",
    NULL,
    -1,
    SpamMethods
};


PyMODINIT_FUNC PyInit_spam(void) {
    return PyModule_Create(&spammodule);
}
