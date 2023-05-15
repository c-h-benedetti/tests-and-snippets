#include <cstring>
#include "ogl_window.hpp"
#include "several-windows.hpp"
#include <iostream>

// Cours/tuto: https://developer-old.gnome.org/gtkmm-tutorial/3.24/sec-basics-simple-example.html.en

enum Test {
    NONE,
    SEVERAL_WINDOWS,
    GLAREA_EPOXY,
    OGL_COURSE
};


Test findTestCase(int argc, char* argv[]) {
    constexpr char msg[] = "Available options: 'windows', 'glarea', 'course'";
    
    if (argc <= 1) {
        std::cerr << msg << std::endl;
        return Test::NONE;
    }

    if (strcmp(argv[1], "windows") == 0) { return Test::SEVERAL_WINDOWS; }
    if (strcmp(argv[1], "glarea") == 0)  { return Test::GLAREA_EPOXY; }
    if (strcmp(argv[1], "course") == 0)  { return Test::OGL_COURSE; }
    
    std::cerr << msg << std::endl;
    return Test::NONE;
}

int main(int argc, char *argv[]) {

    switch (findTestCase(argc, argv)) {
        case (Test::NONE):
            return 0;
        case (Test::SEVERAL_WINDOWS):
            return severalWindows(0, nullptr);
        case (Test::GLAREA_EPOXY):
            return glAreaEopxy(0, nullptr);
        case (Test::OGL_COURSE):
            return courseOpenGL(0, nullptr);
        default:
            return 0;
    };

    return 0;
}
