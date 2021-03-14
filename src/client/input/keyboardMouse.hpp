#pragma once

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <set>

#include "../types.hpp"

namespace c2m { namespace client { namespace input {

template<class HandlerT>
class IKeyboardEventHandler;

enum KeyEventType: uint8_t {
    DOWN = 0,
    PRESS = 1,
    UP = 3
};

struct KeyEvent {
    const KeyEventType targetEvent;
    const std::string id;
    const sf::Keyboard::Key keycode;
};

struct KeyHandler {
    KeyEventType targetEvent;
    std::string id;
    sf::Keyboard::Key keycode;
    FunctionPointer<KeyEvent, IKeyboardEventHandler<KeyEvent>> pointer;

    KeyEvent keyEvent() {
        return KeyEvent{targetEvent, id, keycode};
    }
};

struct TextEvent {
    const std::string id;
    const char character;
};

struct TextHandler {
    std::string id;
    FunctionPointer<TextEvent, IKeyboardEventHandler<TextEvent>> pointer;
};

class KeyboardMouse {
public:
    KeyboardMouse(std::shared_ptr<sf::RenderWindow>& window) : window(window) {};

    void registerKeyHandler(const KeyHandler& handler) {
        switch (handler.targetEvent) {
        using enum KeyEventType;
        case DOWN:
            keyDownHandlers.push_back(handler);
            break;
        case PRESS:
            keyPressHandlers.push_back(handler);
            break;
        case UP:
            keyUpHandlers.push_back(handler);
            break;
        };
    }

    void registerTextHandler(const TextHandler& handler) {
        textHandlers.push_back(handler);
    }

    bool keyIsDown(sf::Keyboard::Key keycode) {
        return keysDown.contains(keycode);
    }

    glm::vec<2, int> mousePos(bool glNormalize = true) {
        sf::Vector2i pos = sf::Mouse::getPosition(*window.get());
        glm::vec2 mousePos(pos.x, pos.y);
        if (!glNormalize) {
            return mousePos;
        }
        sf::Vector2u win = window->getSize();
        glm::vec2 winSize(win.x, win.y);
        winSize.x /= 2;
        winSize.y /= 2;
        return (mousePos - winSize) / winSize;
    }

    bool mouseIsDown(sf::Mouse::Button button) {
        return sf::Mouse::isButtonPressed(button);
    }

    void handleEvent(const sf::Event& event) {
        switch (event.type) {
            using enum sf::Event::EventType;
            case TextEntered:
                handleTextEntered(event);
                break;
            case KeyPressed:
                handleKeyDown(event);
                break;
            case KeyReleased:
                handleKeyUp(event);
                break;
        }
    }

private:
    void handleKeyDown(sf::Event event) {
        if (!window->hasFocus()) {
            return;
        }

        keysDown.insert(event.key.code);

        // Iterate over handlers
        for (auto& handler : keyDownHandlers) {
            if (handler.keycode == event.key.code && handler.targetEvent == KeyEventType::DOWN) {
                handler.pointer.sendCallback(handler.keyEvent());
            }
        }
    }

    void handleKeyUp(sf::Event event) {
        if (!window->hasFocus()) {
            return;
        }

        keysDown.erase(event.key.code);

        // Iterate over handlers
        for (auto& handler : keyUpHandlers) {
            if (handler.keycode == event.key.code && (handler.targetEvent >= KeyEventType::PRESS)) {
                handler.pointer.sendCallback(handler.keyEvent());
            }
        }
    }

    void handleTextEntered(sf::Event event) {
        if (!window->hasFocus()) {
            return;
        }

        for (auto& handler : textHandlers) {
            handler.pointer.sendCallback(TextEvent{handler.id, static_cast<char>(event.text.unicode)});
        }
    }
    
    std::shared_ptr<sf::RenderWindow> window;
    std::vector<KeyHandler> keyDownHandlers, keyUpHandlers, keyPressHandlers;
    std::vector<TextHandler> textHandlers;
    std::set<sf::Keyboard::Key> keysDown;
};

// Interface for handling keyboard
template<class HandlerT>
class IKeyboardEventHandler {
protected:
    virtual void keyboardCallback(HandlerT event) = 0;
};

}}}