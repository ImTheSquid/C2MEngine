#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../../../common/types.hpp"
#include "../shaders/shader.hpp"
#include "../../../common/object.hpp"

namespace c2m { namespace client { namespace gl {

class IDrawable: public common::Object {
public:
    virtual void render() = 0;

protected:
    IDrawable(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> outlineShader = nullptr) : shader(shader), outlineShader(outlineShader) {}

    IDrawable(std::shared_ptr<Shader> shader, common::RGBA color = common::RGBA{}, float outlineWidth = 0, std::shared_ptr<Shader> outlineShader = nullptr, common::RGBA outlineColor = common::RGBA{}) {}

    ~IDrawable() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    std::vector<float> vertices;

    GLuint vbo, vao, ebo;

    common::RGBA color;
    common::RGBA outlineColor;

    float outlineWidth;

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> outlineShader;
};

}}} 