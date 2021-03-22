#pragma once

#include "drawable.hpp"

namespace c2m { namespace client { namespace gl {

class Quad: public IDrawable {
public:
    Quad(glm::vec4 position, std::shared_ptr<Shader> shader, common::RGBA color, float outlineWidth = 0, std::shared_ptr<Shader> outlineShader = nullptr, common::RGBA outlineColor = common::RGBA{}) : \
        IDrawable(shader, color, outlineWidth, outlineShader, outlineColor) {
        vertices = {
            position.x + position.z, 
            position.y + position.w, 
            position.x + position.z, 
            position.y, 
            position.x, 
            position.y, 
            position.x, 
            position.y + position.w
        };

        indices = {
            0, 1, 3,
            1, 2, 3
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices.data()) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices.data()) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void render() {
        // Enable shader and stencil buffer
        if (shader != nullptr) {
            if (outlineWidth > 0) {
                glClearStencil(0);
                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
            }

            applyTransforms();
            shader->useShader();
            shader->setMat4("transform", trans);
            shader->setVec4("color", color.asVec4());
        }

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices.data()) * vertices.size(), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // If the outline width is 0 return
        if (outlineWidth == 0) {
            return;
        }

        // Draw outline
        if (outlineShader != nullptr) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            //glDisable(GL_DEPTH_TEST);
            outlineShader->useShader();

            outlineShader->setVec4("color", outlineColor.asVec4());
            // Temporary transform matrix to prevent pollution of user-set transforms
            glm::mat4 tempTransform = trans;
            tempTransform = glm::scale(tempTransform, glm::vec3(outlineWidth + 1, outlineWidth + 1, 0));
            outlineShader->setMat4("transform", tempTransform);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glStencilMask(0xFF);
            //glEnable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
        }
    }
};

}}}