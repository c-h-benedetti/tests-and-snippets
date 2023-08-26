#include "Point2D.h"
#include "structmember.h"

#include <string.h>
#include <stdbool.h>


/**
 * WARNING:
 * - Si un attribut permet de stocker n'importe quel objet, il faut déclarer au Cyclic Garbage Collector qu'un loop peut avoir lieu.
 * https://docs.python.org/fr/3.10/extending/newtypes_tutorial.html#supporting-cyclic-garbage-collection
 * Par exemple, dans le cas d'une liste, cette dernière peut se stocker elle-même.
 * Ici, on n'a des checks de type qui font que l'objet ne peut pas se stocker lui-même, ça serait inutile.
*/


typedef struct {
    PyObject_HEAD
    double    x;
    double    y;
    char      color[8];  // hexa notation without the '#'.
    PyObject* partition; // If the user wants to make several classes of points in a same collection.
} Point2D_data;


static void Point2D_del(Point2D_data* self) {
    Py_XDECREF(self->partition);
    Py_TYPE(self)->tp_free((PyObject*) self);
}


static PyObject* Point2D_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    Point2D_data* self = (Point2D_data*) type->tp_alloc(type, 0);

    if (self) {
        // init partition
        Py_INCREF(Py_None);
        self->partition = Py_None;

        // init coordinates
        self->x = 0.0;
        self->y = 0.0;

        // init color
        memcpy(self->color, "ffffffff", 8*sizeof(char));
    }
    
    return (PyObject*) self;
}


bool is_valid_hexa(const char* input) {
    if (input == NULL) { return false; }
    
    int s = strlen(input);

    if (s != 9)      { return false; }
    if (input[0] != '#') { return false; }

    char c;
    for (int i = 0 ; i < s ; i++) {
        c = input[i];
        if ((c >= 48) && (c <= 57)) { // We have a digit in [0-9], which is valid.
            continue;
        }
        if ((c >= 65) && (c <= 70)) { // We have a capital letter in [A-F], which is valid.
            continue;
        }
        if ((c >= 97) && (c <= 102)) { // We have a lower case letter in [a-f], which is valid.
            continue;
        }
        return false;
    }
    return true;
}


static int Point2D_init(Point2D_data* self, PyObject* args, PyObject* kwds) {
    static char *kwlist[] = {"coordinates", "color", "partition", NULL};
    
    PyObject* partition = NULL; // Arbitrary object representing a partition to which this point belongs.
    double x = 0.0, y   = 0.0;  // Coordinates of the point.
    const char* color   = NULL; // Color assigned to that point.

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|(dd)UO", kwlist, &x, 
                                                                    &y,
                                                                    &color,
                                                                    &partition)) 
    {
        return -1;
    }
    

    /*
    We increment the reference because we give the object straight to our instance.
    We don't get the value from the object to reallocate it later. We keep the original alive.
    */
    if (partition) {
        if (!PyUnicode_Check(partition) && (partition != Py_None)) {
            PyErr_SetString(PyExc_TypeError, "Invalid partition provided. Expecting a str or None.");
            return -1;
        }
        PyObject* tmp = self->partition;
        Py_INCREF(partition);
        self->partition = partition;
        Py_XDECREF(tmp);
    }

    if (is_valid_hexa(color)) {
        memcpy(self->color, color+1, 8*sizeof(char));
    }

    self->x = x;
    self->y = y;

    return 0;
}


/**
 * If we want to use a getter and a setter for a given attribute, we must remove it from this list.
 * Otherwise, it will still be accessible freely without any constraint.
*/
static PyMemberDef Point2D_attributes[] = {
    {"x"        , T_DOUBLE   , offsetof(Point2D_data, x)        , 0, "X coordinate of the given 2D point."},
    {"y"        , T_DOUBLE   , offsetof(Point2D_data, y)        , 0, "Y coordinate of the given 2D point."},
    {"color"    , T_STRING   , offsetof(Point2D_data, color)    , 0, "Display color assigned to this point."},
    // {"partition", T_OBJECT_EX, offsetof(Point2D_data, partition), 0, "Partition of the given point."},
    {NULL}
};


// +-----------------------------------------------------------------------------+
// |   Methods of Point2D                                                        |
// +=============================================================================+


static PyObject* Point2D_method_getPartition(Point2D_data* self, void* closure) {
    Py_INCREF(self->partition);
    return self->partition;
}


static int Point2D_method_setPartition(Point2D_data* self, PyObject* value, void* closure) {
    PyObject *tmp;
    
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the partition attribute");
        return -1;
    }

    if (!PyUnicode_Check(value) && (value != Py_None)) {
        PyErr_SetString(PyExc_TypeError,
                        "The partition attribute value must be a string or None");
        return -1;
    }

    tmp = self->partition;
    Py_INCREF(value);
    self->partition = value;
    Py_DECREF(tmp);
    return 0;
}


static PyObject* Point2D_method_getLocation(Point2D_data* self, PyObject* Py_UNUSED(ignored)) {
    PyObject* tuple = PyTuple_Pack(2, PyFloat_FromDouble(self->x), PyFloat_FromDouble(self->y));
    return tuple;
}


// +== End of Point2D methods ===================================================+


static PyGetSetDef Point2D_accessors[] = {
    {"partition", (getter) Point2D_method_getPartition, (setter) Point2D_method_setPartition, "Partition of the 2D point.", NULL},
    {NULL}
};


static PyMethodDef Point2D_methods[] = {
    {"getLocation", (PyCFunction) Point2D_method_getLocation, METH_NOARGS, "Returns a tuple containing the coordinates of the point."},
    {NULL}
};


static PyObject* Point2D_str(PyObject* py_obj) {
    Point2D_data* self = (Point2D_data*) py_obj;
    return PyUnicode_FromFormat("[(%R, %R) | %S]", PyFloat_FromDouble(self->x), PyFloat_FromDouble(self->y), self->partition);
}


static PyTypeObject Point2D_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name      = "geo2d.Point2D",
    .tp_doc       = PyDoc_STR("Object representing a 2D point with a color and a partition"),
    .tp_basicsize = sizeof(Point2D_data),
    .tp_itemsize  = 0,
    .tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new       = Point2D_new,
    .tp_init      = (initproc) Point2D_init,
    .tp_dealloc   = (destructor) Point2D_del,
    .tp_members   = Point2D_attributes,
    .tp_methods   = Point2D_methods,
    .tp_getset    = Point2D_accessors,
    .tp_str       = Point2D_str
};


static PyModuleDef geo2d_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "geo2d",
    .m_doc = "Contains objects and functions able to manipulate 2D points.",
    .m_size = -1
};



PyMODINIT_FUNC PyInit_geo2d(void) {
    PyObject* m;

    if (PyType_Ready(&Point2D_type) < 0) { return NULL; }

    m = PyModule_Create(&geo2d_module);

    if (m == NULL) { return NULL; }

    Py_INCREF(&Point2D_type);

    if (PyModule_AddObject(m, "Point2D", (PyObject*) &Point2D_type) < 0) {
        Py_DECREF(&Point2D_type);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}




/**
 * TODO:
 * 
 * - [ ] Fonction `geo2d.distance(p1, p2)` qui renvoie la distance entre 2 instances de Point2D.
 * - [ ] Fonction `geo2d.bbox([p1, p2, ..., pk])` qui renvoie un tuple de 2 Point2D qui sont les coins de la bounding box de la collection.
 * - [ ] Comment construire un gros module avec des objets et fonctions dans plusieurs fichiers C ?
*/