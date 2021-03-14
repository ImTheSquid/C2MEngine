#pragma once

#include <iostream>

#include "drawable.hpp"

namespace c2m { namespace client { namespace gl {

class Triangle: public IDrawable {
public:
    Triangle(std::shared_ptr<Shader> shader, common::RGBA color, std::shared_ptr<Shader> outlineShader = nullptr) : IDrawable(shader, outlineShader) {
        vertices = {
            0.0f, 0.5f,
            0.5f, -0.5f,
            -0.5f, -0.5f
        };

        this->color = color;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices.data()) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void render() {
        // Enable shader and stencil buffer
        if (shader != nullptr) {
            if (outlineWidth > 0) {
                glClearStencil(0);
                //glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
            }

            applyTransforms();
            shader->useShader();
            shader->setMat4("transform", trans);
            shader->setVec4("color", color.asVec4());
        }

        // Rebind the VAO to be able to modify its VBOs
        glBindVertexArray(vao);

        // Reset vertex data to class array
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices.data()) * vertices.size(), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Draw rectangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // If the outline width is 0 return
        if (outlineWidth == 0) {
            return;
        }

        // Draw outline
        if (outlineShader != nullptr) {
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            outlineShader->useShader();

            outlineShader->setVec4("color", outlineColor.asVec4());
            // Temporary transform matrix to prevent pollution of user-set transforms
            glm::mat4 tempTransform = trans;
            tempTransform = glm::scale(tempTransform, glm::vec3(outlineWidth + 1, outlineWidth + 1, 0));
            tempTransform = glm::translate(tempTransform, glm::vec3(0, outlineWidth/7, 0));
            outlineShader->setMat4("transform", tempTransform);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glStencilMask(0xFF);
            glEnable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
        }
    }
};

}}}