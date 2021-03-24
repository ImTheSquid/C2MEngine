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

    inline bool isTransparent() {
        return color.a < 1 || outlineColor.a < 1;
    }

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
    inline void queueDrawable(std::shared_ptr<IDrawable> drawable) {
        queue.push_back(drawable);
        hasSorted = false;
    }

    void executeRender(bool clearQueue = false) {
        // Only sort if the queue has changed
        if (!hasSorted) {
            // Sort into transparent and opaque queues
            for (const auto& drawable : queue) {
                if (drawable->isTransparent()) {
                    transparent.push_back(drawable);
                }
                else {
                    opaque.push_back(drawable);
                }
            }

            // Sort transparent objects by Z-distance (2D depth), but only if needed
            std::sort(transparent.begin(), transparent.end(), [](std::shared_ptr<IDrawable> left, std::shared_ptr<IDrawable> right) { return left->getPosition().z < right->getPosition().z; });

            hasSorted = true;
        }

        // Render opaque objects
        glDepthMask(GL_TRUE);
        for (const auto& drawable : opaque) {
            drawable->render();
        }

        // Render transparent objects from front to back
        glDepthMask(GL_FALSE);
        for (const auto& drawable : transparent) {
            drawable->render();
        }
        glDepthMask(GL_TRUE);

        // Clear queue
        if (clearQueue) {
            queue.clear();
        }
    }

private:
    std::vector<std::shared_ptr<IDrawable>> transparent, opaque;
    bool hasSorted = false;
    std::vector<std::shared_ptr<IDrawable>> queue;
};

void IDrawable::queueForRender(std::optional<RenderQueue*> overrideQueue) {
    if (overrideQueue == std::nullopt && renderQueue == std::nullopt) {
        throw new std::exception("No render queue set");
    }

    overrideQueue.value_or(renderQueue.value())->queueDrawable(shared_from_this());
};

}}} 