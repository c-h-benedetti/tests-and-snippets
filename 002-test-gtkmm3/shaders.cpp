#include "shaders.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <fstream>
#include <cstring>
#include <iostream>

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

bool ShaderLoader::from_file(const std::filesystem::path& p) {
    if (this->loaded) {
        std::cerr << "Shader already loaded" << std::endl;
        return false;
    }

    std::ifstream f(p, std::ios::in);
    if (!f.is_open()) {
        std::cerr << "Impossible to open: " << p << std::endl;
        return false;
    }

    size_t size       = 2048;
    char buffer[size] = {0};
    size_t read_sz    = 0;
    std::string total = "";

    do {
        memset(buffer, 0, size);
        read_sz = f.read(buffer, size).gcount();
        total += buffer;
    } while (read_sz > 0);

    f.close();
    this->create_shader(total);

    return loaded;
}

void ShaderLoader::compile(const std::string& c) {
    if (id == 0) {
        loaded = false;
        return;
    }
    const char* s = c.c_str();
    glShaderSource(id, 1, &s, NULL);
    glCompileShader(id);
    loaded = this->check_compilation();
}

bool ShaderLoader::check_compilation() {
    int  success;
    size_t b_size = 1024;
    char infoLog[b_size];
    memset(infoLog, 0, b_size);

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    
    if(!success) {
        glGetShaderInfoLog(id, b_size, NULL, infoLog);
        std::cerr << "ERROR" << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    std::cerr << "[" << id << "]. Shader successfully compiled." << std::endl;
    return true;
}

ShaderLoader::~ShaderLoader() {
    if (this->id > 0) {
        glDeleteShader(this->id);
        this->id = 0;
        this->loaded = false;
    }
}

ShaderLoader::ShaderLoader(): id(0), loaded(false) {}


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void VertexShaderLoader::create_shader(const std::string& s) {
    this->id = glCreateShader(GL_VERTEX_SHADER);
    this->compile(s);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void FragmentShaderLoader::create_shader(const std::string& s) {
    this->id = glCreateShader(GL_FRAGMENT_SHADER);
    this->compile(s);
}

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

void ProgramShader::attach_shader(const ShaderLoader& sl) {
    if (id == 0) { return; }
    if (sl.get_raw_id() == 0) { return; }
    glAttachShader(id, sl.get_raw_id());
    std::cerr << "Attached shader: " << sl.get_raw_id() << std::endl;
}

void ProgramShader::release() {
    if (id == 0) { return; }
    glDeleteProgram(id);
    id = 0;
}

void ProgramShader::link() {
    glLinkProgram(id);
    this->loaded = this->check_compilation();
}

bool ProgramShader::check_compilation() {
    int  success;
    size_t b_size = 1024;
    char infoLog[b_size];
    memset(infoLog, 0, b_size);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    
    if(!success) {
        glGetProgramInfoLog(id, b_size, NULL, infoLog);
        std::cerr << "ERROR::" << "COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }
    std::cerr << "[" << id << "]. Program successfully linked." << std::endl;
    return true;
}

ProgramShader::~ProgramShader() {}

ProgramShader::ProgramShader(): id(0), loaded(false) {}

ProgramShader::ProgramShader(const VertexShaderLoader& vsl, const FragmentShaderLoader& fsl): ProgramShader() {
    id = glCreateProgram();
    this->attach_shader(vsl);
    this->attach_shader(fsl);
    this->link();
}

void ProgramShader::use() {
    glUseProgram(id);
}

ProgramShader::ProgramShader(const std::filesystem::path& vs_path, const std::filesystem::path& fs_path) {
    // Creating vertex & fragment shader
    VertexShaderLoader vsl;
    vsl.from_file(vs_path);
    FragmentShaderLoader fsl;
    fsl.from_file(fs_path);
    
    id = glCreateProgram();
    this->attach_shader(vsl);
    this->attach_shader(fsl);
    this->link();
}