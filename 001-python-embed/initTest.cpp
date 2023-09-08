#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "Custom.h"
#include "Point2D.h"
#include "spammodule.hpp"
#include <string>
#include <filesystem>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <stdexcept>

// Separator character used in the PATH envionment variable according to the OS.
#ifdef _WIN32
    #define DELIM ';'
#else
    #define DELIM ':'
#endif


using Path = std::filesystem::path;

/**
 * @brief Creates an environment path from the chunks passed as parameters.
 */
std::string makeFullPath(const std::vector<Path>& chunks) {
    if (std::empty(chunks)) { throw std::invalid_argument("Cannot build path from empty list of chunks."); }
    std::string buffer = chunks[0];
    
    for (size_t i = 1 ; i < chunks.size() ; i++) {
        buffer += DELIM;
        buffer += chunks[i];
    }

    return buffer;
}


/**
 * @brief Creates valid context for a Python interpreter.
 * @param appPath Absolute path of the folder in which the binary is located. Corresponds to `argv[0]`.
 * @param name Program name (facultative)
 */
void init_python(const char* appPath, const char* name=nullptr) {
    
    // = Définition de tous les chemins que l'interpréteur Python doit connaitre =
    Path software_path    = std::filesystem::absolute(std::filesystem::path(appPath).parent_path());
    Path python_home_path = software_path / CPYTHON_BUILD_DIR;
    Path executable_path  = python_home_path / "python";

    std::string env_path    = makeFullPath({
        python_home_path / "Lib",                        // built-in modules.
        python_home_path / "Python",                     // built-in modules.
        software_path    / "custom",                     // folder for user's modules.
        python_home_path / "build/lib.linux-x86_64-3.10" // binaries (.so) supporting built-in modules.
    });    

    // = Construction de la configuration =
    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    // 1. Set Python's home path
    status = PyConfig_SetBytesString(
        &config, 
        &config.home, 
        python_home_path.c_str()
    );
    if (PyStatus_Exception(status)) { goto exception; }

    // 2. Set Python's libraries directory name
    status = PyConfig_SetBytesString(
        &config,
        &config.platlibdir,
        "Lib"
    );
    if (PyStatus_Exception(status)) { goto exception; }

    // 3. Change error displaying mode during python path calculation.
    config.pathconfig_warnings = 1;

    // 4. Set program name (if one was provided)
    status = PyConfig_SetBytesString(
        &config, 
        &config.program_name, 
        (name == nullptr) ? "python3.10.10" : name
    );
    if (PyStatus_Exception(status)) { goto exception; }

    // 5. Set Python path (sys.path)
    status = PyConfig_SetBytesString(
        &config,
        &config.pythonpath_env,
        env_path.c_str()
    );
    if (PyStatus_Exception(status)) { goto exception; }
    // codeBlockEnd;
    // codeBlock{"id": "init_python_p2", "other":4};
    // 6. Setting executable location:
    status = PyConfig_SetBytesString(
        &config,
        &config.executable,
        executable_path.c_str()
    );
    if (PyStatus_Exception(status)) { goto exception; }

    // = Initializing the Python interpreter & cleaning =
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) { goto exception; }
    PyConfig_Clear(&config);
    return;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}


/*

(1) https://docs.python.org/3.10/c-api/init_config.html?highlight=pyconfig#c.PyStatus

- Utiliser PyStatus_Exception et Py_ExitStatusException pour gérer les erreurs et 
  les exceptions.
- D'après la doc officielle (1) Python peut être initialisé plusieurs fois.
- The current configuration (PyConfig type) is stored in PyInterpreterState.config

*/

int main(int argc, char* argv[], char* env[]) {

    if (PyImport_AppendInittab("spam", PyInit_spam) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table with `spam`.\n");
        return 1;
    }

    if (PyImport_AppendInittab("custom", PyInit_custom) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table with `custom`.\n");
        return 1;
    }

    if (PyImport_AppendInittab("geo2d", PyInit_geo2d) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table with `geo2d`.\n");
        return 1;
    }

    // Initialisation de l'environnement Python
    init_python(argv[0]);

    // Depuis un script dans un fichier:
    FILE* fp = fopen("../dump.py", "r");
    if (fp == NULL) {
        std::cerr << "File not found" << std::endl;
        return 1;
    }
    PyRun_SimpleFile(fp, "CSVtable.py");
    fclose(fp);
    
    // Finaliser l'interpréteur Python
    if (Py_FinalizeEx() < 0) {
        std::cout << "Something went wrong in the Python execution." << std::endl;
    }
    return 0;
}
