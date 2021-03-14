#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>

#include "client/engine.hpp"
#include "client/gl/drawing/triangle.hpp"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
};

class Root : public c2m::client::IC2MRoot {
    void init() {
        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);

        std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
        auto manager = c2m::client::Engine::getShaderManager();
        manager->addShader("polygon", new c2m::client::gl::Shader("../../../shaders/polygon.vert", "../../../shaders/singleColor.frag"));
        triangle = new c2m::client::gl::Triangle(manager->getShader("polygon"), c2m::common::RGBA{1, 0, 0, 1});

        manager->setUniforms<const glm::mat4&>(&c2m::client::gl::Shader::setMat4, "projection", c2m::client::gl::Camera::getProjectionMat());
        manager->setUniforms<const glm::mat4&>(&c2m::client::gl::Shader::setMat4, "view", c2m::client::gl::Camera::getLookMat());

        // Enable various OpenGL functions
        // Enable depth testing/z-indexing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // Stencil
        glEnable(GL_STENCIL_TEST);
        // Disable stencil writing by default, to be enabled per draw cycle
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // Alpha
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void update(const c2m::client::RootUpdateData& data) override {
        auto manager = c2m::client::Engine::getShaderManager();
        for (auto& event : data.frameEvents) {
            if (event.type == sf::Event::EventType::Closed) {
               c2m::client::Engine::stop();
            }
            else if (event.type == sf::Event::Resized) {
                // adjust the viewport when the window is resized and change perspective
                glViewport(0, 0, event.size.width, event.size.height);
                c2m::client::gl::Camera::getProjectionAddr() = glm::perspective(glm::radians(45.0f), (float)event.size.width / (float)event.size.height, 0.1f, 100.0f);
                manager->setUniforms<const glm::mat4&>(&c2m::client::gl::Shader::setMat4, "projection", c2m::client::gl::Camera::getProjectionMat());
            }
        }
        manager->setUniforms<const glm::mat4&>(&c2m::client::gl::Shader::setMat4, "view", c2m::client::gl::Camera::getLookMat());
    }

    void render() override {
        auto manager = c2m::client::Engine::getShaderManager();
        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClearStencil(0);
        glStencilMask(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        triangle->render();
    }

    c2m::client::gl::Triangle *triangle;
};