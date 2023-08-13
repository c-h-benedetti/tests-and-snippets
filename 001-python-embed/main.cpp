#include <Python.h>
#include <string>
#include <filesystem>
#include <iostream>


int main(int argc, char *argv[], char* env[]) {
    
    std::filesystem::path executable_path = std::filesystem::absolute(std::filesystem::path(argv[0]).parent_path());
    
    // Définition des variables d'environnement:
    std::filesystem::path pythonHome = executable_path / "cpython-3.10";
    std::filesystem::path pythonPath = pythonHome / "Lib";
    std::filesystem::path pythonBin  = pythonHome / "build/lib.linux-x86_64-3.10";
        
    setenv("PYTHONHOME", pythonHome.c_str(), 1); // 1 veut dire qu'on overwrite si la variable existe déjà.
    setenv("PYTHONPATH", pythonPath.c_str(), 1);

    // Initialiser l'interpréteur Python
    Py_Initialize();

    if (!Py_IsInitialized()) {
        std::cerr << "Failed to initialize Python" << std::endl;
        return -1;
    }

    PyObject* sysPath = PySys_GetObject("path");
    PyObject* pythonLibPathPy = PyUnicode_FromString(pythonPath.c_str());
    PyObject* pythonBinPathPy = PyUnicode_FromString(pythonBin.c_str());
    PyList_Append(sysPath, pythonLibPathPy);
    PyList_Append(sysPath, pythonBinPathPy);
    Py_DECREF(pythonLibPathPy);
    Py_DECREF(pythonBinPathPy);

    // Exécution de code à la volée:
    PyRun_SimpleString(
        "text = '==> Hello world'\n"
        "prefix = '==> '\n"
        "text_without_prefix = text.removeprefix(prefix)\n"
        "print(text_without_prefix)\n"
    );

    // Depuis un script dans un fichier:
    FILE* fp = fopen("./dump.py", "r");
    PyRun_SimpleFile(fp, "CSVtable.py");
    fclose(fp);

    // Finaliser l'interpréteur Python
    Py_Finalize();

    return 0;
}

/*

clear && g++ -Wall -std=c++20 main.cpp -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Lib -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10 -lpython3.10 -I/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Include -lpthread -o MAIN && ./MAIN
clear && g++ -Wall -std=c++20 main.cpp -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Lib -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10 -ldl -lm -lpthread -lutil -lrt -lpython3.10 -I/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Include -o MAIN && ./MAIN

clear && g++ -Wall -std=c++20 main.cpp -o MAIN -I/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Include -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10/Lib -L/home/clement/Desktop/tests-and-snippets/001-python-embed/cpython-3.10 -lpython3.10 -ldl -lm -lpthread -lutil -lrt && ./MAIN
*/