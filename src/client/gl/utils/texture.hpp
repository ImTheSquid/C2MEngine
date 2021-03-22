#pragma once

#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>

namespace c2m { namespace client { namespace gl {

class Texture {
public:
    Texture(const std::string& path, const bool loadImmediately) : texture(new sf::Texture()) {
        this->path = path;
        if (loadImmediately) {
            sf::Image image;
            if (!image.loadFromFile(path)) {
                throw new std::exception(strcat("Couldn't load texture from: ", path.c_str()));
            }
            image.flipVertically();
            texture->loadFromImage(image);
            texture->generateMipmap();
            isLoaded = true;
        }
    }

    ~Texture() {
        delete texture;
    }

    void load() {
        if (isLoaded) {
            return;
        }
        sf::Image image;
        if (!image.loadFromFile(path)) {
            throw new std::exception(strcat("Couldn't load texture from: ", path.c_str()));
        }
        image.flipVertically();
        texture->loadFromImage(image);
        texture->generateMipmap();
        isLoaded = true;
    }

    void bind() {
        sf::Texture::bind(texture);
    }

    std::shared_ptr<sf::Texture> get() {
        return std::shared_ptr<sf::Texture>(texture);
    }

    void unbind() {
        sf::Texture::bind(nullptr);
    }

private:
    bool isLoaded = false;
    std::string path;
    sf::Texture *texture;
};

}}}