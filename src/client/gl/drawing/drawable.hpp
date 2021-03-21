#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <optional>

#include "../../../common/types.hpp"
#include "../shaders/shader.hpp"
#include "../../../common/object.hpp"
// #include "renderQueue.hpp"

namespace c2m { namespace client { namespace gl {

class RenderQueue;

class IDrawable: public common::Object, public std::enable_shared_from_this<IDrawable> {
public:
    void queueForRender(std::optional<RenderQueue*> overrideQueue = std::nullopt);

    virtual void render() = 0;

    inline static std::optional<RenderQueue*> renderQueue = std::nullopt;

    common::RGBA getColor() {
        return color;
    }

protected:
    IDrawable(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> outlineShader = nullptr) : shader(shader), outlineShader(outlineShader) {}

    IDrawable(std::shared_ptr<Shader> shader, common::RGBA color = common::RGBA{}, float outlineWidth = 0, std::shared_ptr<Shader> outlineShader = nullptr, common::RGBA outlineColor = common::RGBA{}) : \
        shader(shader), color(color), outlineWidth(outlineWidth), outlineShader(outlineShader), outlineColor(outlineColor) {}

    ~IDrawable() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    std::vector<float> vertices;
    std::vector<GLint> indices;

    GLuint vbo, vao, ebo;

    common::RGBA color;
    common::RGBA outlineColor;

    float outlineWidth;

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> outlineShader;
};

// A class that handles rendering order to make sure transparency is handled correctly
class RenderQueue {
public:
    void queueDrawable(std::shared_ptr<IDrawable> drawable) {
        queue.push_back(drawable);
    }

    void executeRender() {
        // Sort queues by Z-distance (2D depth)
        std::sort(queue.begin(), queue.end(), [](std::shared_ptr<IDrawable> left, std::shared_ptr<IDrawable> right) { return left->getPosition().z < right->getPosition().z; });

        // Sort into transparent and opaque queues
        using DrawableVector = std::vector<std::shared_ptr<IDrawable>>;
        DrawableVector transparent, opaque;
        for (const auto& drawable : queue) {
            if (drawable->getColor().a == 1) {
                opaque.push_back(drawable);
            }
            else {
                transparent.push_back(drawable);
            }
        }

        // Render all opaque objects
        glEnable(GL_DEPTH_TEST);
        for (const auto& drawable : opaque) {
            drawable->render();
        }

        // Render all transparent objects from front to back
        glDisable(GL_DEPTH_TEST);
        for (const auto& drawable : transparent) {
            drawable->render();
        }
        glEnable(GL_DEPTH_TEST);

        // Clear queue
        queue.clear();
    }

private:
    std::vector<std::shared_ptr<IDrawable>> queue;
};

void IDrawable::queueForRender(std::optional<RenderQueue*> overrideQueue) {
    if (overrideQueue == std::nullopt && renderQueue == std::nullopt) {
        throw new std::exception("No render queue set");
    }

    overrideQueue.value_or(renderQueue.value())->queueDrawable(shared_from_this());
};

}}} 