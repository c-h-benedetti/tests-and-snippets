#ifndef SHADERS_LOAD_HPP_INCLUDED
#define SHADERS_LOAD_HPP_INCLUDED

#include <filesystem>
#include <string>

class ShaderLoader {

protected:

    unsigned int id;
    bool loaded;

protected:

    virtual void create_shader(const std::string& s) = 0;
    void compile(const std::string& c);
    bool check_compilation();

public:

    virtual const char* who() const = 0;

    bool from_file(const std::filesystem::path& p);
    inline unsigned int get_raw_id() const { return this->id; }

    virtual ~ShaderLoader();

    ShaderLoader();
};


class VertexShaderLoader : public ShaderLoader {
public:
    void create_shader(const std::string& s) override;

    const char* who() const override { return "Vertex Shader"; }

    VertexShaderLoader(): ShaderLoader() {}
};


class FragmentShaderLoader : public ShaderLoader {
public:
    void create_shader(const std::string& s) override;

    const char* who() const override { return "Fragment Shader"; }

    FragmentShaderLoader(): ShaderLoader() {}
};


class ProgramShader {
    
    unsigned int id;
    bool loaded;

private:

    void link();
    bool check_compilation();

public:

    void use();

    void attach_shader(const ShaderLoader& sl);
    void release();

    ~ProgramShader();

    ProgramShader();
    ProgramShader(const VertexShaderLoader& vsl, const FragmentShaderLoader& fsl);
    ProgramShader(const std::filesystem::path& vs_path, const std::filesystem::path& fs_path);
};

#endif // SHADERS_LOAD_HPP_INCLUDED

/*

- [ ] Ajouter une méthode pour pouvoir caster ces objets en booléens.
- [ ] Ajouter une map pour stocker la position des uniforms dans le programme.

*/