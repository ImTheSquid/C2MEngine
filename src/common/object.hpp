#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace c2m { namespace common {

class Object {
public:
    Object() : trans(1.0f), pos(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scaleVec(1.0f, 1.0f, 1.0f) {};

    void translate(glm::vec3 amount) {
        pos += amount;
    }

    void translateTo(glm::vec3 target) {
        pos = target;
    };

    void scaleTo(glm::vec3 target) {
        scaleVec = target;
    };

    // Rotate specified number of degrees
    void rotate(glm::vec3 amount) {
        rotation += amount;
    }

    // Rotate to specific degree count
    void rotateTo(glm::vec3 target) {
        rotation = target;
    };

    // Applies position and rotation vectors
    void applyTransforms() {
        // Reset transform
        trans = glm::mat4(1.0f);
        // Apply transformations
        // WARNING: Do not switch the order these are in (translate, scale, rotate), or else the rotation axis (and other stuff) may change
        trans = glm::translate(trans, pos);
        trans = glm::scale(trans, scaleVec);
        if (rotation.x != 0.0f) trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        if (rotation.y != 0.0f) trans = glm::rotate(trans, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        if (rotation.z != 0.0f) trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    };

    void scale(glm::vec3 amount) {
        scaleVec += amount;
    }

    glm::vec3 getPosition() const {
        return pos;
    }

    glm::mat4 getTransform() const {
        return trans;
    }

protected:
    // Position
    glm::vec3 pos;
    // Transform
    glm::mat4 trans;
    // Rotation
    glm::vec3 rotation;
    // Scale
    glm::vec3 scaleVec;
};

}}