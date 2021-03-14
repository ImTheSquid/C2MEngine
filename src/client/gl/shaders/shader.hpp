#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <fstream>
#include <exception>
#include <vector>

namespace c2m { namespace client { namespace gl {

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::optional<const std::string>& geometryPath = std::nullopt) {
		if (!std::filesystem::exists(vertexPath)) {
			throw new std::exception(strcat("Invalid vertex shader path: ", vertexPath.c_str()));
		}
		std::ifstream vertPath;
		vertPath.open(vertexPath);
		std::string vertData((std::istreambuf_iterator<char>(vertPath)), std::istreambuf_iterator<char>());
		vertPath.close();

		if (!std::filesystem::exists(fragmentPath)) {
			throw new std::exception(strcat("Invalid fragment shader path: ", fragmentPath.c_str()));
		}
		std::ifstream fragPath;
		fragPath.open(fragmentPath);
		std::string fragData((std::istreambuf_iterator<char>(fragPath)), std::istreambuf_iterator<char>());
		fragPath.close();

		std::optional<const char*> geomData = std::nullopt;
		if (geometryPath != std::nullopt) {
			if (!std::filesystem::exists(geometryPath.value())) {
				throw new std::exception(strcat("Invalid geometry shader path: ", geometryPath.value().c_str()));
			}
			std::ifstream geomPath;
			geomPath.open(geometryPath.value());
			std::string tempGeom((std::istreambuf_iterator<char>(geomPath)), std::istreambuf_iterator<char>());
			geomPath.close();
			geomData = tempGeom.c_str();
		}

        generateHandles(vertData.c_str(), fragData.c_str(), geomData);
    }

    ~Shader() {
        glDeleteProgram(shaderProgram);
    }

    void useShader() {
        glUseProgram(shaderProgram);
    }

    GLuint getHandle() {
        return shaderProgram;
    }

    /*
    Methods for setting uniforms
    */

    void setBool(const std::string& name, bool value) {
        glUseProgram(shaderProgram);
        glUniform1i(getUniformLocation(name), (int)value);
    }

    void setInt(const std::string& name, int value) {
        glUseProgram(shaderProgram);
        glUniform1i(getUniformLocation(name), value);
    }

    void setFloat(const std::string& name, float value) {
        glUseProgram(shaderProgram);
        glUniform1f(getUniformLocation(name), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) {
        glUseProgram(shaderProgram);
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec3(const std::string& name, const glm::vec3& value) {
        glUseProgram(shaderProgram);
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec4(const std::string& name, const glm::vec4& value) {
        glUseProgram(shaderProgram);
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    void setMat2(const std::string& name, const glm::mat2& mat) {
        glUseProgram(shaderProgram);
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string& name, const glm::mat3& mat) {
        glUseProgram(shaderProgram);
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    GLint getUniformLocation(const std::string& name) {
        glUseProgram(shaderProgram);

        GLint location = glGetUniformLocation(shaderProgram, name.c_str());
        if (location == -1) {
            throw new std::exception(strcat("Uniform not found. Name: ", name.c_str()));
        }
        return location; 
    }

private:
    void generateHandles(const char *vertexData, const char *fragmentData, const std::optional<const char*>& geometryData) {
        // vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexData, NULL);
        glCompileShader(vertexShader);

        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            throw new std::exception(strcat("Vertex shader compilation failed: ", infoLog));
        }
        
        // fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentData, NULL);
        glCompileShader(fragmentShader);

        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            throw new std::exception(strcat("Fragment shader compilation failed: ", infoLog));
        }

        // geometry shader (if given)
        GLuint geometryShader = 0;
        if (geometryData != std::nullopt) {
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &geometryData.value(), NULL);
            glCompileShader(geometryShader);
            // check for shader compile errors
            glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
                throw new std::exception(strcat("Geometry shader compilation failed: ", infoLog));
            }
        }
        
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        if (geometryData != std::nullopt) glAttachShader(shaderProgram, geometryShader);
        glLinkProgram(shaderProgram);
        
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
        if (geometryData != std::nullopt) glDetachShader(shaderProgram, geometryShader);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (geometryData != std::nullopt) glDeleteShader(geometryShader);

        glValidateProgram(shaderProgram);
        GLint status;
        glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &status);
        if (status == GL_FALSE) {
            throw new std::exception("Failed to validate shader");
        }
    }

    GLuint shaderProgram = 0;
};

}}}