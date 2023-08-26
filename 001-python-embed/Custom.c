#include "Custom.h"
#include "structmember.h"

// +-----------------------------------------------------------------------------+
// |   Defining the data stored in this object                                   |
// +=============================================================================+

/**
 * NOTE:
 * - Data that should be manipulated on Python side is declared as PyObjects.
 * - Data that should be used on C side is declared with a C-type.
 * - The HEAD is mendatory
*/

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    PyObject* first_name;
    PyObject* last_name;
    int number;
} CustomObject;

// +-----------------------------------------------------------------------------+
// |   What is called when the __del__ function is summoned                      |
// +=============================================================================+

// We start by decreasing the counter of our custom attributes.
// Then, we deallocate the standard members.
static void Custom_del(CustomObject* self) {
    Py_XDECREF(self->first_name);
    Py_XDECREF(self->last_name);
    Py_TYPE(self)->tp_free((PyObject*) self);
}

// +-----------------------------------------------------------------------------+
// |   What is called when the __new__ function is summoned                      |
// +=============================================================================+

/**
 * NOTE:
 * - Here, we just allocate the memory and give a defaut value. Init using parameters is done later.
 * - When we allocate the space for a new var, we check that it is not NULL. If it is, we decref the whole object in a way to mark it as corrupted.
 *   Then we return NULL to terminate the execution and trigger an error.
*/

static PyObject* Custom_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CustomObject *self;
    self = (CustomObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        // init first_name name
        self->first_name = PyUnicode_FromString("");
        if (self->first_name == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        // init last_name name
        self->last_name = PyUnicode_FromString("");
        if (self->last_name == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        // init number
        self->number = 0;
    }
    return (PyObject *) self;
}

// +-----------------------------------------------------------------------------+
// |   How the object's values are initialized from the parameters               |
// +=============================================================================+

/**
 * NOTE:
 * - Memory for this object is already allocated when we reach this function.
*/

static int Custom_init(CustomObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"first_name", "last_name", "number", NULL};
    PyObject *first_name = NULL, *last_name = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist,
                                     &first_name, &last_name,
                                     &self->number))
        return -1;

    /*
    We increment the reference because we give the object straight to our instance.
    We don't get the value from the object to reallocate it later. We keep the original alive.
    */
    if (first_name) {
        tmp = self->first_name;
        Py_INCREF(first_name);
        self->first_name = first_name;
        Py_XDECREF(tmp);
    }
    if (last_name) {
        tmp = self->last_name;
        Py_INCREF(last_name);
        self->last_name = last_name;
        Py_XDECREF(tmp);
    }
    return 0;
}

// +-----------------------------------------------------------------------------+
// |   Member attributes of our object                                           |
// +=============================================================================+

/**
 * NOTE:
 * - C'est grâce à cette structure que le int stocké dans la classe peut être manipulé en C comme en Python sans cast de l'un vers l'autre.
 * - Ce comportement est possible grace au header "structmember"
*/

static PyMemberDef Custom_members[] = {
    {"first_name" , T_OBJECT_EX, offsetof(CustomObject, first_name) , 0, "first_name name"},
    {"last_name"  , T_OBJECT_EX, offsetof(CustomObject, last_name)  , 0, "last_name name"},
    {"number"     , T_INT      , offsetof(CustomObject, number)     , 0, "custom number"},
    {NULL}  /* Sentinel */
};

// +-----------------------------------------------------------------------------+
// |   Defining the behavior of a method called Custom.name()                    |
// +=============================================================================+

static PyObject* Custom_name(CustomObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->first_name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first_name");
        return NULL;
    }
    if (self->last_name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last_name");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S", self->first_name, self->last_name);
}

// +-----------------------------------------------------------------------------+
// |   Member methods of our object                                              |
// +=============================================================================+

static PyMethodDef Custom_methods[] = {
    {"name", (PyCFunction) Custom_name, METH_NOARGS, "Return the name, combining the first_name and last_name."},
    {NULL}  /* Sentinel */
};

// +-----------------------------------------------------------------------------+
// |   Python's type descriptor                                                  |
// +=============================================================================+

static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "custom.Custom",
    .tp_doc = PyDoc_STR("Custom objects handling names."),
    .tp_basicsize = sizeof(CustomObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Custom_new,
    .tp_init = (initproc) Custom_init,
    .tp_dealloc = (destructor) Custom_del,
    .tp_members = Custom_members,
    .tp_methods = Custom_methods
};

// +-----------------------------------------------------------------------------+
// |   Python's module descriptor                                                |
// +=============================================================================+

static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "custom",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1
};

// +-----------------------------------------------------------------------------+
// |   Function initializing the module                                          |
// +=============================================================================+

PyMODINIT_FUNC PyInit_custom(void) {
    PyObject* m;
    if (PyType_Ready(&CustomType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    /*
    We have to add manually every object in the given module.
    When we only had function, we could simply give them to the PyModuleDef object.
    Here, we need to have an instance of Custom (the type) so we can use it to create instances.
    */

    Py_INCREF(&CustomType);
    if (PyModule_AddObject(m, "Custom", (PyObject *) &CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}