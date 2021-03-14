#pragma once

#include <map>

#include "shader.hpp"

namespace c2m { namespace client { namespace gl {

class ShaderManager {
public:
    ShaderManager() : shaders(std::map<std::string, std::shared_ptr<Shader>>()) {}

    ~ShaderManager() {
        activeShader.reset();

        for (auto shader = shaders.begin(); shader != shaders.end(); ++shader) {
            shader->second.reset();
        }
    }

    void addShader(const std::string& key, Shader *shader) {
        shaders.insert({key, std::shared_ptr<Shader>(shader)});
    }

    bool activateShader(const std::string& key) {
        if (key == activeShaderKey) {
            return true;
        }
        auto location = shaders.find(key);
        if (location == shaders.end()) {
            return false;
        }
        location->second->useShader();
        activeShader = location->second;
        activeShaderKey = location->first;
        return true;
    }

    std::shared_ptr<Shader> getShader(const std::string& key) {
        if (key == activeShaderKey) {
            return getActiveShader();
        }
        auto location = shaders.find(key);
        if (location == shaders.end()) {
            return nullptr;
        }
        return location->second;
    }

    template<class UniformT>
    void setUniforms(void(Shader::* fp)(const std::string&, const UniformT&), const std::string& name, const UniformT& value) {
        for (auto iter = shaders.cbegin(); iter != shaders.cend(); ++iter) {
            Shader *temp = iter->second.get();
            temp->useShader();
            (temp->*fp)(name, value);
            temp = NULL;
        }
    }

    std::shared_ptr<Shader> getActiveShader() {
        return !activeShader.expired() ? activeShader.lock() : nullptr;
    }

    std::string& getCurrentKey() { 
        return activeShaderKey; 
    }

private:
    std::map<std::string, std::shared_ptr<Shader>> shaders;

    std::weak_ptr<Shader> activeShader;
    std::string activeShaderKey;
};

}}}