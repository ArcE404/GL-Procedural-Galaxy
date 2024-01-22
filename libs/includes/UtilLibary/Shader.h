#ifndef SHADER_H
#define SHADER_H

#include <glad43/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader
{
public:
    // the program ID
    unsigned int ID;
    unsigned int reloadedProgramID;
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath) {
        string vertexCode = readFile(vertexPath);
        string fragmentCode = readFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        ID = createShaderProgram(vShaderCode, fShaderCode);
    }

    Shader(const char* vertexPath, const char* fragmentPath,
        const char* tcsPath, const char* tesPath) {
        string vertexCode = readFile(vertexPath);
        string fragmentCode = readFile(fragmentPath);
        string tcsCode = readFile(tcsPath);
        string tesCode = readFile(tesPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* tcsShaderCode = tcsCode.c_str();
        const char* tesShaderCode = tesCode.c_str();

        ID = createTellShaderProgram(vShaderCode, fShaderCode, tcsShaderCode, tesShaderCode);
    }

    Shader(const char* computePath) {
        string computeCode = readFile(computePath);

        const char* computeShaderCode = computeCode.c_str();

        ID = createComputeShaderProgram(computeShaderCode);
    }

    // use/activate the shader
    void use() {
        glUseProgram(ID);
    }
    // utility uniform functions
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setMat4(const string& name, glm::mat4 value) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setMat3(const string& name, glm::mat3 value) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setVec3(const string& name, GLfloat v1, GLfloat v2, GLfloat v3) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform3f(location, v1, v2, v3);
    }

    void setVec3(const string& name, glm::vec3 value) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }


    void setVec2(const string& name, GLfloat v1, GLfloat v2) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform2f(location, v1, v2);
    }

    void setVec2(const string& name, glm::vec2 value) const {
        int location = glGetUniformLocation(ID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }
    //hot reload

    unsigned int createShaderProgram(const char* vShaderCode, const char* fShaderCode) {
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        // vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        };

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        };


        // shader Program
        reloadedProgramID = glCreateProgram();
        glAttachShader(reloadedProgramID, vertex);
        glAttachShader(reloadedProgramID, fragment);
        glLinkProgram(reloadedProgramID);
        // print linking errors if any
        glGetProgramiv(reloadedProgramID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(reloadedProgramID, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return 0;
        }
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return reloadedProgramID;
    }

    unsigned int createComputeShaderProgram(const char* cShaderCode) {
        unsigned int compute;
        int success;
        char infoLog[512];

        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        // print compile errors if any
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(compute, 512, NULL, infoLog);
            cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << endl;
        };


        // shader Program
        reloadedProgramID = glCreateProgram();
        glAttachShader(reloadedProgramID, compute);
        glLinkProgram(reloadedProgramID);
        // print linking errors if any
        glGetProgramiv(reloadedProgramID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(reloadedProgramID, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
            glDeleteShader(compute);
            return 0;
        }
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(compute);
        return reloadedProgramID;
    }

    unsigned int createTellShaderProgram(const char* vShaderCode, const char* fShaderCode,
        const char* tcsShaderCode, const char* tesShaderCode) {
        unsigned int vertex, fragment, tcs, tes;
        int success;
        char infoLog[512];

        // vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        // print compile errors if any
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        };

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        // print compile errors if any
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
        };

        tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcs, 1, &tcsShaderCode, NULL);
        glCompileShader(tcs);
        // print compile errors if any
        glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(tcs, 512, NULL, infoLog);
            cout << "ERROR::SHADER::TESS_CONTROL::COMPILATION_FAILED\n" << infoLog << endl;
        };

        tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tes, 1, &tesShaderCode, NULL);
        glCompileShader(tes);
        // print compile errors if any
        glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(tes, 512, NULL, infoLog);
            cout << "ERROR::SHADER::TESS_EVALUATION::COMPILATION_FAILED\n" << infoLog << endl;
        };

        // shader Program
        reloadedProgramID = glCreateProgram();
        glAttachShader(reloadedProgramID, vertex);
        glAttachShader(reloadedProgramID, tcs);
        glAttachShader(reloadedProgramID, tes);
        glAttachShader(reloadedProgramID, fragment);
        glLinkProgram(reloadedProgramID);
        // print linking errors if any
        glGetProgramiv(reloadedProgramID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(reloadedProgramID, 512, NULL, infoLog);
            cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteShader(tcs);
            glDeleteShader(tes);
            return 0;
        }
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteShader(tcs);
        glDeleteShader(tes);
        return reloadedProgramID;
    }

    string readFile(const char* _path) {
        ifstream shaderFile;
        shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        try {
            // open files
            shaderFile.open(_path);
            stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            return shaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
            return "ERROR";
        }
    }

    void reloadShaderProgram(const char* vertexPath, const char* fragmentPath) {
        assert(vertexPath && fragmentPath);

        string vertexCode = readFile(vertexPath);
        string fragmentCode = readFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        if (createShaderProgram(vShaderCode, fShaderCode) != 0) {
            glDeleteProgram(ID);
            ID = reloadedProgramID;
            cout << "Reaload succeed.  " << "The program ID is " << ID;
        }
    }

    void reloadTellShaderProgram(const char* vertexPath, const char* fragmentPath,
        const char* tcsPath, const char* tesPath) {
        assert(vertexPath && fragmentPath && tcsPath && tesPath);

        string vertexCode = readFile(vertexPath);
        string fragmentCode = readFile(fragmentPath);
        string tcsCode = readFile(tcsPath);
        string tesCode = readFile(tesPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* tcsShaderCode = tcsCode.c_str();
        const char* tesShaderCode = tesCode.c_str();

        if (createTellShaderProgram(vShaderCode, fShaderCode, tcsShaderCode, tesShaderCode) != 0) {
            glDeleteProgram(ID);
            ID = reloadedProgramID;
            cout << "Reaload succeed.  " << "The program ID is " << ID;
        }
    }

    void reloadComputeShaderProgram(const char* computePath) {
        assert(computePath);

        string computeCode = readFile(computePath);

        const char* computeShaderCode = computeCode.c_str();

        if (createComputeShaderProgram(computeShaderCode) != 0) {
            glDeleteProgram(ID);
            ID = reloadedProgramID;
            cout << "Reaload succeed.  " << "The program ID is " << ID;
        }
    }

};

#endif