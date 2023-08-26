#ifndef SPAM_MODULE_PYTHON_INCLUDED
#define SPAM_MODULE_PYTHON_INCLUDED

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C API functions */
#define PySpam_factorial_NUM 0
#define PySpam_factorial_RETURN long int
#define PySpam_factorial_PROTO (long int)

/* Total number of C API pointers */
#define PySpam_API_pointers 1

#ifdef SPAM_MODULE
/* This section is used when compiling spammodule.c */

static PySpam_factorial_RETURN PySpam_factorial PySpam_factorial_PROTO;

#else
/* This section is used in modules that use spammodule's API */

static void **PySpam_API;

#define PySpam_factorial \
 (*(PySpam_factorial_RETURN (*)PySpam_factorial_PROTO) PySpam_API[PySpam_factorial_NUM])

/* Return -1 on error, 0 on success.
 * PyCapsule_Import will set an exception if there's an error.
 */
static int import_spam(void) {
    PySpam_API = (void **)PyCapsule_Import("spam._C_API", 0);
    return (PySpam_API != NULL) ? 0 : -1;
}

#endif

PyMODINIT_FUNC PyInit_spam(void);

#ifdef __cplusplus
}
#endif

#endif //SPAM_MODULE_PYTHON_INCLUDED