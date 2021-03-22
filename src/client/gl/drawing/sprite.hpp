#pragma once

#include "drawable.hpp"
#include "../utils/texture.hpp"
#include "../../../common/object.hpp"

namespace c2m { namespace client { namespace gl {

class Sprite: public IDrawable {
public:
    Sprite(const std::string& texPath, const bool loadImmediately, const std::shared_ptr<Shader> simpleShader, const common::RGBA color, const std::shared_ptr<Shader> framebufferShader, const float outlineWidth = 0, \
        const std::shared_ptr<Shader> outlineShader = nullptr, const common::RGBA outlineColor = common::RGBA{}) : IDrawable(simpleShader, color, outlineWidth, outlineShader, outlineColor) {
        indices = {
            0, 1, 3,
            1, 2, 3
        };

        if (loadImmediately) {
            load();
        }
    }

    void render() override {
        // Make sure load routine has executed before attempting a render
        if (vertices.empty()) {
            load();
        }

        if (outlineWidth > 0) {
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		applyTransforms();
		shader->useShader();
		// Framebuffer will be transformed, this will not
		shader->setMat4("transform", glm::mat4(1.0f));
		shader->setVec4("color", color.asVec4());
    }

    void load() {
        // Initialize OpenGL and load textures
        // Calculate aspect ratio of base quad to correctly fit image
        texture->load();
        sf::Vector2u size = texture->get()->getSize();
        float height = size.y, width = size.x, ratio = width / height;
        if (height > width) {
            height = 1.0f;
            width = 1.0f * ratio;
        }
        else if (width > height) {
            width = 1.0f;
            height = 1.0f / ratio;
        }
        else {
            width = 1.0f;
            height = 1.0f;
        }

        vertices = {
            // positions  // texture coords
            width,  height, 1.0f, 1.0f, // top right
            width, -height, 1.0f, 0.0f, // bottom right
            -width, -height, 0.0f, 0.0f, // bottom left
            -width,  height, 0.0f, 1.0f  // top left
        };

        // Buffers
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices.data()) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices.data()) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // Verticies
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Framebuffer VBO and VAO
        glGenVertexArrays(1, &fbVao);
        glBindVertexArray(fbVao);

        glGenBuffers(1, &fbVbo);
        glBindBuffer(GL_ARRAY_BUFFER, fbVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fbVertices), fbVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Framebuffer
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texBuffer);
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        GLuint sizeX = size.x + (GLuint)(size.x * outlineWidth * 2), sizeY = size.y + (GLuint)(size.y * outlineWidth * 2);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texBuffer, 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sizeX, sizeY);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw new std::exception("Incomplete framebuffer");
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    std::shared_ptr<Texture> texture;

    float fbVertices[16] = {
        1, 1, 1, 1,
        1, -1, 1, 0,
        -1, -1, 0, 0,
        -1, 1, 0, 1
    };

    // Framebuffer shader for outline
    std::shared_ptr<Shader> fbShader;
    GLuint framebuffer = 0, fbVao = 0, fbVbo = 0, texBuffer = 0, rbo = 0;
};

}}}