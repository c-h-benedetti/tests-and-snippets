#include <Python.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <initializer_list>
#include <cstdlib>

// codeBlock{'id': "envPathSeparator", 'other':4};
#ifdef _WIN32
    #define DELIM ';'
#else
    #define DELIM ':'
#endif
// codeBlockEnd;

using Path = std::filesystem::path;

// codeBlock{'id': "makeFullPath", 'other':4};
std::string makeFullPath(const std::initializer_list<Path>& chunks) {
    std::string buffer = "";

    if (std::empty(chunks)) { return buffer; }
    
    for (const Path& p : chunks) {
        buffer += p.c_str();
        buffer += DELIM;
    }
    return buffer.substr(0, buffer.size()-1);
}
// codeBlockEnd;

// codeBlock{'id': "init_python", 'other':4};
void init_python(const char* appPath, const char* name=nullptr) {
    
    // = = = Définition de tous les chemins que l'interpréteur Python doit connaitre = = =
    Path software_path    = std::filesystem::absolute(std::filesystem::path(appPath).parent_path());
    Path python_home_path = software_path / "python";
    Path executable_path  = python_home_path / "python";

    std::string env_path    = makeFullPath({
        python_home_path / "Lib",
        software_path / "custom",
        python_home_path / "build/lib.linux-x86_64-3.10"
    });    

    // = = = Construction de la configuration = = =
    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    // 1. Set Python's home path
    status = PyConfig_SetBytesString(&config, &config.home, python_home_path.c_str());
    if (PyStatus_Exception(status)) { goto exception; }

    // 2. Set Python's libraries directory name
    status = PyConfig_SetBytesString(&config, &config.platlibdir, "Lib");
    if (PyStatus_Exception(status)) { goto exception; }

    // 3. Change error displaying mode during python path calculation.
    config.pathconfig_warnings = 1;

    // 4. Set program name (if one was provided)
    status = PyConfig_SetBytesString(&config, &config.program_name, (name == nullptr) ? "python3.10.10" : name);
    if (PyStatus_Exception(status)) { goto exception; }

    // 5. Set Python path (sys.path)
    // DELIM est une variable de préprocesseur qui représente le séparateur de path (au besoin)
    status = PyConfig_SetBytesString(&config, &config.pythonpath_env, env_path.c_str());
    if (PyStatus_Exception(status)) { goto exception; }

    // 6. Setting executable location:
    status = PyConfig_SetBytesString(&config, &config.executable, executable_path.c_str());
    if (PyStatus_Exception(status)) { goto exception; }

    // = = = Initializing the Python interpreter & cleaning = = =
    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) { goto exception; }
    PyConfig_Clear(&config);
    return;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}
// codeBlockEnd;

/*

(1) https://docs.python.org/3.10/c-api/init_config.html?highlight=pyconfig#c.PyStatus

- Utiliser PyStatus_Exception et Py_ExitStatusException pour gérer les erreurs et les exceptions.
- D'après la doc officielle (1) Python peut être initialisé plusieurs fois.
- The current configuration (PyConfig type) is stored in PyInterpreterState.config

*/

int main(int argc, char* argv[], char* env[]) {
    // Initialisation de l'environnement Python
    init_python(argv[0]);

    // Depuis un script dans un fichier:
    FILE* fp = fopen("./dump.py", "r");
    PyRun_SimpleFile(fp, "CSVtable.py");
    fclose(fp);

    // Depuis un module custom
    PyRun_SimpleString(
        "from thing import theFunction\n"
        "theFunction()\n"
    );
    
    // Finaliser l'interpréteur Python
    Py_Finalize();
    return 0;
}


/*

clear && g++ -Wall -std=c++20 main.cpp -L/media/clement/376DD68B604D847C/python-test/python/Lib -L/media/clement/376DD68B604D847C/python-test/python -lpython3.10 -I/media/clement/376DD68B604D847C/python-test/python/Include -lpthread -o MAIN && ./MAIN
clear && g++ -Wall -std=c++20 main.cpp -L/media/clement/376DD68B604D847C/python-test/python/Lib -L/media/clement/376DD68B604D847C/python-test/python -ldl -lm -lpthread -lutil -lrt -lpython3.10 -I/media/clement/376DD68B604D847C/python-test/python/Include -o MAIN && ./MAIN

clear && g++ -Wall -std=c++20 main.cpp -o MAIN -I/media/clement/376DD68B604D847C/python-test/python/Include -L/media/clement/376DD68B604D847C/python-test/python/Lib -L/media/clement/376DD68B604D847C/python-test/python -lpython3.10 -ldl -lm -lpthread -lutil -lrt && ./MAIN
*/