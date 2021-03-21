#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <exception>
#include <memory>
#include <iostream>
#include <GL/glew.h>
#include <thread>
#include <chrono>

#include "input/keyboardMouse.hpp"
#include "gl/shaders/shaderManager.hpp"
#include "gl/camera.hpp"

namespace c2m {
namespace client {

struct RootUpdateData {
    // Delta time in seconds
    float deltaTime;
    std::vector<sf::Event> frameEvents;
};

class IC2MRoot {
public:
    virtual void init() {}
    virtual void update(const RootUpdateData& data) = 0;
    virtual void render() = 0;
};

// All of these functions MUST be called on the same thread
class Engine {
public:
    static bool init(const std::string& title, const int width, const int height, const int x, const int y, const std::shared_ptr<IC2MRoot>& handler, const GLuint style = 7, const sf::ContextSettings settings = sf::ContextSettings(24, 8, 0, 3, 3, sf::ContextSettings::Attribute::Default, false)) {
        window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title, style, settings);

        GLenum error = glewInit();
        if (error != GLEW_OK) {
            std::cout << "An error occured while initializing GLEW: " << glewGetErrorString(error) << std::endl;
            return false;
        }

        auto winSize = window->getSize();
        glViewport(0, 0, winSize.x, winSize.y);
        gl::Camera::getProjectionAddr() = glm::perspective(glm::radians(45.0f), (float)winSize.x / (float)winSize.y, 0.1f, 100.0f);

        input = std::make_shared<input::KeyboardMouse>(getWindow());
        root = handler;

        shaderManager = std::make_shared<gl::ShaderManager>();

        return true;
    }

    static void startAndWait() {
        loop();
    }

    static void stop() {
        run = false;
    }

    static std::shared_ptr<sf::RenderWindow> getWindow() {
        if (window == nullptr) {
            throw new std::exception("Window not initialized.");
        }
        return window;
    }

    static std::shared_ptr<input::KeyboardMouse> getInput() {
        if (input == nullptr) {
            throw new std::exception("Window not initialized.");
        }
        return input;
    }

    static std::shared_ptr<gl::ShaderManager> getShaderManager() {
        if (shaderManager == nullptr) {
            throw new std::exception("Window not initialized.");
        }
        return shaderManager;
    }

private:
    static void loop() {
        root->init();
        sf::Event event;
        auto lastTime = std::chrono::steady_clock::now();
        while(run) {
            // Delta time
            auto currentTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
            float deltaTime = duration.count() / 1000.0f;
            lastTime = currentTime;
            std::vector<sf::Event> events;
            while(window->pollEvent(event)) {
                switch (event.type) {
                using enum sf::Event::EventType;
                case TextEntered:
                case KeyPressed:
                case KeyReleased:
                    input->handleEvent(event);
                    break;
                default:
                    events.push_back(event);
                    break;
                }
            }

            RootUpdateData data{deltaTime, events};
            root->update(data);
            root->render();
            window->display();
        }
    }

    inline static bool run = true;
    inline static std::thread mainThread;
    inline static std::shared_ptr<IC2MRoot> root = nullptr;
    inline static std::shared_ptr<sf::RenderWindow> window = nullptr;
    inline static std::shared_ptr<input::KeyboardMouse> input = nullptr;
    inline static std::shared_ptr<gl::ShaderManager> shaderManager = nullptr;
};

}
}