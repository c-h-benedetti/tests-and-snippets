#include "shaders.hpp"
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include "stb_image.h"

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

void ProgramShader::locate_uniforms(const std::vector<const char*>& variables) {
    this->use();
    for (const char* name : variables) {
        int loc = glGetUniformLocation(this->id, name);
        if (loc == -1) {
            std::cerr << "Failed to locate the uniform variable: " << name << std::endl;
            continue;
        }
        this->uniforms[name] = loc;
    }
}

void ProgramShader::attach_shader(const ShaderLoader& sl) {
    if (id == 0) { return; }
    if (sl.get_raw_id() == 0) { return; }
    glAttachShader(id, sl.get_raw_id());
    std::cerr << "Attached shader: " << sl.get_raw_id() << std::endl;
}

void ProgramShader::release() {
    if (id == 0) { return; }
    
    for (size_t i = 0 ; i < textures.size() ; i++) {
        glDeleteTextures(1, &textures[i]);
    }
    
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

    for (size_t i = 0 ; i < textures.size() ; i++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
}

ProgramShader::ProgramShader(const std::filesystem::path& vs_path, const std::filesystem::path& fs_path) {
    
    id = glCreateProgram();

    if (vs_path.string().size() > 0) {
        VertexShaderLoader vsl;
        vsl.from_file(vs_path);
        this->attach_shader(vsl);
    }

    if (fs_path.string().size() > 0) {
        FragmentShaderLoader fsl;
        fsl.from_file(fs_path);
        this->attach_shader(fsl);
    }
    
    this->link();
}

void ProgramShader::set_texture(const char* var, const char* path) {
    
    int t_width, t_height, nChannels;
    u_char* data = stbi_load(path, &t_width, &t_height, &nChannels, 3);
    unsigned int texture;
    glGenTextures(1, &texture);
    this->textures.push_back(texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    data = nullptr;

    int a = this->uniforms[var];

    glUniform1i(a, this->textures.size()-1);
}

// - [ ] Il devrait plutôt y avoir un TexturesManager pour qu'une même texture soit utilisée par plusieurs objets.
// - [ ] Pour la GUI, il sera préférable d'avoir une image qui contient tous les skins.
//       On pourra en créer une autre au lancement pour les icônes customs des utilisateurs.