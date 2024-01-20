#include <iostream>
#include <glad43/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <UtilLibary/WindowsUtil.h>
#include <UtilLibary/Shader.h>
#include <UtilLibary/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <UtilLibary/Camera.h>



#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM 1.0 / float(RANDOM_IM)
#define RANDOM_IQ 127773
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876
const int NUMBER_PARTICLE = 100000;
const int NUMBER_STAR = 80000;
const float PI = 3.14159265359f;

int _SEED = 0;

void srand_cycle()
{
    _SEED ^= RANDOM_MASK;
    int k = _SEED / RANDOM_IQ;
    _SEED = RANDOM_IA * (_SEED - k * RANDOM_IQ) - RANDOM_IR * k;

    if (_SEED < 0)
        _SEED += RANDOM_IM;

    _SEED ^= RANDOM_MASK;
}

void srand_set(int seed)
{
    _SEED = seed;
    srand_cycle();
}

float myRand()
{
    srand_cycle();
    return RANDOM_AM * _SEED;
}

float ease_in_exp(float x)
{
    return x <= 0.0 ? 0.0 : pow(2, 10.0 * x - 10.0);
}

struct Particle {
    glm::vec3 pos;
    float rotation;
    float angle;
    float height;
    float angleVel;
    float brightness;
    float temp;
};

struct ComputeParameters {
    float inExc;
    float outExc;
    float offset;
    float maxRad;
    float core;
    float inExcDiv;
    float outExcDiv;
    float bugleRad;
    float speed;
    unsigned int numStarts;
    float maxStarBrightness;
    float minStarBrightness;
    float maxDustBrightness;
    float minDustBrightness;
    float maxTemp;
    float minTemp;
};

struct VertexParams {
    float starScale;
    float dustScale;
    unsigned int numStarts;
    float h2Size;
    float h2Distance;
};

using namespace std;

void renderSphere();
void SphereInit();

const int VIEW_PORT_WIDTH = 1920;
const int VIEW_PORT_HEIGHT = 1080;
Camera* camera = new Camera(glm::vec3(0.0f, 50000, -50000.f), 3000);

WindowsUtil* windowUtil = new WindowsUtil();

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool close = false;

using namespace glm;

unsigned int sphereVAO;
unsigned int indexCount;

Shader* galaxyShader;
Shader* computeShader;
GLFWwindow* window;


Particle particles[NUMBER_PARTICLE];

enum RenderMode {
    wireframeMode,
    pointMode,
    fillMode
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    windowUtil->mouse_callback(window, xpos, ypos, camera);
}


void init() {
    window = windowUtil->InitWindowV43(VIEW_PORT_WIDTH, VIEW_PORT_HEIGHT, "dProxy_window", NULL, NULL);
    glEnable(GL_DEPTH_TEST);

    galaxyShader = new Shader("GalaxyShader.vs", "GalaxyShader.frag");
    computeShader = new Shader("./particleProcessor.comp");

    //user input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

int main()
{
    init();
    //render mode
    RenderMode renderMode = fillMode;

    unsigned int particleSsbo;
    glGenBuffers(1, &particleSsbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSsbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(Particle) + 8) * NUMBER_PARTICLE, particles, GL_STATIC_DRAW);

    unsigned int particlesIndex = glGetUniformBlockIndex(computeShader->ID, "Particles");
    glUniformBlockBinding(computeShader->ID, particlesIndex, 2);

    particlesIndex = glGetUniformBlockIndex(galaxyShader->ID, "Particles");
    glUniformBlockBinding(galaxyShader->ID, particlesIndex, 2);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, particleSsbo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ComputeParameters cParam;
    cParam.inExc = 0.25;
    cParam.outExc = 0.25;
    cParam.offset = 5.0;
    cParam.core = 600;
    cParam.maxRad = 1000;
    cParam.inExcDiv = 1 - cParam.inExc;
    cParam.outExcDiv = 1 - cParam.outExc;
    cParam.bugleRad = 500;
    cParam.speed = 7.0f;
    cParam.numStarts = NUMBER_STAR;
    cParam.minDustBrightness = 0.001f;
    cParam.maxDustBrightness = 0.005f;
    cParam.maxStarBrightness = 0.1f;
    cParam.minStarBrightness = 0.5f;
    cParam.maxTemp = 6000;
    cParam.minTemp = 4500;
    //cParam.dustTemp = 8000;

    unsigned int computeParams;
    glGenBuffers(1, &computeParams);
    glBindBuffer(GL_UNIFORM_BUFFER, computeParams);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ComputeParameters) , &cParam, GL_STATIC_DRAW); 
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, computeParams);

    VertexParams vParam;
    vParam.starScale = 14.5f;
    vParam.dustScale = 22.4f;
    vParam.numStarts = NUMBER_STAR;
    vParam.h2Distance = 100;
    vParam.h2Size = 23;

    unsigned int vertexParams;
    glGenBuffers(1, &vertexParams);
    glBindBuffer(GL_UNIFORM_BUFFER, vertexParams);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(VertexParams), &vParam, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 5, vertexParams);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSsbo);

    computeShader->use();
    glDispatchCompute(NUMBER_PARTICLE/250, 1, 1);

    // make sure writing to image has finished before read
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);



    SphereInit();

    //render loop

    while (!close)
    {
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)VIEW_PORT_WIDTH / (float)VIEW_PORT_HEIGHT, 0.1f, 100000.0f);
        galaxyShader->use();
        galaxyShader->setMat4("projection", projection);
        //hot reaload
        if (glfwGetKey(window, GLFW_KEY_R)) {
            computeShader->reloadComputeShaderProgram("./particleProcessor.comp");
            computeShader->use();
            glDispatchCompute(NUMBER_PARTICLE / 256, 1, 1);

            // make sure writing to image has finished before read
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            glUseProgram(0);

            galaxyShader->reloadShaderProgram("GalaxyShader.vs", "GalaxyShader.frag");
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.001f, 0.001f, 0.001f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            renderMode = wireframeMode;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            renderMode = pointMode;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            renderMode = fillMode;
        }

        switch (renderMode) {
        case fillMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // el modo wireframe
            break;
        case pointMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // el modo wireframe
            break;
        case wireframeMode:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // el modo wireframe
            break;
        default:
            break;
        }
        close = windowUtil->processInput(window, camera, deltaTime);

        galaxyShader->use();
        glm::mat4 view = camera->GetViewMatrix();
        galaxyShader->setMat4("view", view);
        galaxyShader->setVec3("camPos", camera->Position);

        float time = glfwGetTime();
        glm::mat4 model = glm::mat4(1.0f);
        galaxyShader->setMat4("model", model);
        galaxyShader->setFloat("time", time);
        glBindVertexArray(sphereVAO);
        glDrawElementsInstanced(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0, NUMBER_PARTICLE);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();
    return 0;
}

void SphereInit() {
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        vector<glm::vec3> positions;
        vector<glm::vec2> uv;
        vector<glm::vec3> normals;
        vector<unsigned int> indices;

        const unsigned int X_SEGMENTS =8;
        const unsigned int Y_SEGMENTS = 8;

        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;  // 0 -> 1
                float ySegment = (float)y / (float)Y_SEGMENTS; // 0 -> 1
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI); 
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
}

void renderSphere() {

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}