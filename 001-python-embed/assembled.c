static PyObject* maFonction(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* str_args;
    char* keyword1;
    int keyword2;

    // Extraire les arguments positionnels (les chaînes)
    if (!PyArg_ParseTuple(args, "O", &str_args)) {
        return NULL;
    }

    // Parcourir les arguments positionnels et les traiter
    if (PyTuple_Check(str_args)) {
        for (Py_ssize_t i = 0; i < PyTuple_Size(str_args); i++) {
            PyObject* str_obj = PyTuple_GetItem(str_args, i);
            if (!PyUnicode_Check(str_obj)) {
                PyErr_SetString(PyExc_TypeError, "Attendu des chaînes");
                return NULL;
            }
            // Traiter chaque chaîne ici...
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Attendu un tuple de chaînes");
        return NULL;
    }

    // Définir les mots-clés attendus
    static char* kwlist[] = {"keyword1", "keyword2", NULL};
    if (!PyArg_ParseTupleAndKeywords(PyTuple_New(0), kwargs, "|si", kwlist, &keyword1, &keyword2)) {
        return NULL;
    }

    // Utilisez keyword1 et keyword2 ici...

    Py_RETURN_NONE;
}

// Définir la spécification de méthode
static PyMethodDef SpamMethods[] = {
    {"maFonction", (PyCFunction)maFonction, METH_VARARGS | METH_KEYWORDS, "Description"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};
