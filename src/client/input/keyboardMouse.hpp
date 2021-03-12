#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_keyboard.h>
#include <glm/glm.hpp>
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <set>

#include "../types.hpp"

namespace c2m { namespace client { namespace input {

class IKeyboardEventHandler;

enum KeyEventType: uint8_t {
    DOWN = 0,
    PRESS = 1,
    UP = 2
};

struct KeyEvent {
    KeyEventType targetEvent;
    std::string id;
    SDL_Keycode keycode;
};

struct KeyHandler {
    KeyEventType targetEvent;
    std::string id;
    SDL_Keycode keycode;
    FunctionPointer<KeyEvent, IKeyboardEventHandler> pointer;

    KeyEvent keyEvent() {
        return KeyEvent{targetEvent, id, keycode};
    }
};

class KeyboardMouse {
public:
    KeyboardMouse(std::shared_ptr<SDL_Window>& window) : window(window), poll(std::thread(&KeyboardMouse::poll, this)) {};

    ~KeyboardMouse() {
        shouldContinue = false;
        poll.join();
    };

    void registerKeyHandler(KeyHandler& handler) {
        registeredKeyHandlers.push_back(handler);
    };

    bool keyIsDown(SDL_Keycode keycode) {
        return keysDown.contains(keycode);
    }

    glm::vec<2, int> mousePos(bool glNormalize = true) {
        glm::vec<2, int> mousePos;
        SDL_GetMouseState(&mousePos.x, &mousePos.y);
        if (!glNormalize) {
            return mousePos;
        }
        glm::vec<2, int> winSize;
        SDL_GetWindowSize(window.get(), &winSize.x, &winSize.y);
        winSize.x /= 2;
        winSize.y /= 2;
        return (mousePos - winSize) / winSize;
    }

    bool mouseIsDown(uint32_t button) {
        return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(button);
    }

private:
    // Read keyboard as a thread
    void read() {
        SDL_Event event;
        while (shouldContinue) {
            // Skip loop if window is not focused
            if (!(SDL_GetWindowFlags(window.get()) & SDL_WINDOW_INPUT_FOCUS)) {
                continue;
            }

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                    handleKeyDown(event.key);
                    break;
                    case SDL_KEYUP:
                    handleKeyUp(event.key);
                    break;
                }
            }
        }
    };

    void handleKeyDown(SDL_KeyboardEvent event) {
        if (event.windowID != SDL_GetWindowID(window.get())) {
            return;
        }

        keysDown.insert(event.keysym.sym);

        // Iterate over handlers
        for (auto& handler : registeredKeyHandlers) {
            if (handler.keycode == event.keysym.sym && handler.targetEvent == KeyEventType::DOWN) {
                handler.pointer.sendCallback(handler.keyEvent());
            }
        }
    };

    void handleKeyUp(SDL_KeyboardEvent event) {
        if (event.windowID != SDL_GetWindowID(window.get())) {
            return;
        }

        keysDown.erase(event.keysym.sym);

        // Iterate over handlers
        for (auto& handler : registeredKeyHandlers) {
            if (handler.keycode == event.keysym.sym && (handler.targetEvent >= KeyEventType::PRESS)) {
                handler.pointer.sendCallback(handler.keyEvent());
            }
        }
    };
    
    std::thread poll;
    bool shouldContinue = true;
    std::shared_ptr<SDL_Window> window;
    std::vector<KeyHandler> registeredKeyHandlers;
    std::set<SDL_Keycode> keysDown;
};

// Interface for handling keyboard
class IKeyboardEventHandler {
protected:
    virtual void keyboardCallback(KeyEvent event) = 0;
};

}}}