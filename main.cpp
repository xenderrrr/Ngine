// Yes I know my code is complete shit, but it works
// No GUI for now, just make a new C++ file or edit the code
// I don't know if this is gonna compile on any other device


#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


float movementSpeed = 0.001f;

unsigned int previousProgram;
unsigned int currentProgram;

unsigned int program1;
unsigned int program2;
unsigned int program3;
GLFWwindow* window;
float cameraXPos = 0.0f;
float cameraZPos = 0.0f;

float mouseXPos = 0.0f;
float mouseYPos = 0.0f;

float cameraDirectionH = 0.0f;
float cameraDirectionV = 0.0f;


float deltaMouseXPos;
float deltaMouseYPos;

glm::vec3 movementDirWS;
glm::vec3 movementDirAD;
glm::vec3 movement;

bool moving;

bool holdingM2;

void ResizeWindow(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
    moving = false;
    movement = glm::vec3(0.0f);
    movementDirAD = glm::vec3(sin(cameraDirectionH * 0.01 + glm::radians(90.0f)), 0.0f, cos(cameraDirectionH * 0.01 + glm::radians(90.0f)));
    movementDirWS = glm::vec3(sin(cameraDirectionH * 0.01), 0.0f, cos(cameraDirectionH * 0.01));

    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moving = true;
        movement = movementDirWS * movementSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moving = true;
        movement = movementDirWS * movementSpeed * -1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moving = true;
        movement = movementDirAD * movementSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moving = true;
        movement = movementDirAD * movementSpeed * -1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        movementSpeed += 0.00001f;
        std::cout << movementSpeed << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && movementSpeed > 0) {
        movementSpeed -= 0.00001f;
        std::cout << movementSpeed << std::endl;
    }
}

double lastX = 300.0, lastY = 300.0;

bool firstMouse = true;

void ProcessMouseMovement(GLFWwindow* window, double x, double y) {
    if (firstMouse) {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    
    mouseXPos = x;
    mouseYPos = y;

    

    deltaMouseXPos = x - lastX;
    deltaMouseYPos = lastY - y;

    if (holdingM2) {
        cameraDirectionH -= deltaMouseXPos;
        cameraDirectionV += deltaMouseYPos;
    }

    lastX = x;
    lastY = y;
}





//holdingM2 = true;
//std::cout << "M2" << std::endl;

void ProcessMouseInput(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
        holdingM2 = true;
    }

    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
        holdingM2 = false;
    }
}

struct Vector3 {
public:
    float x;
    float y;
    float z;

    

    static float Normalize (float X, float Y, float Z){
        float tan = X / Z;
        float tan2 = Y / Z;

        //float newX = asin(1) * glm::degrees();
        float newX = sin(atan(tan));
        //std::cout << newX << std::endl;
        float newZ = sin(1.5707 - atan(tan));
        std::cout << newX << std::endl << newZ << std::endl;
        return newX;
    }

private:
};



class Camera {
    public:
    glm::vec3 eye = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(1.0f);

    glm::mat4 view;

    float fov;

    void Init() {
        view = glm::mat4(1.0f);
    }
    
    // Camera code goes in here (no shit)
    void Render() {
        glm::vec3 directionH = glm::vec3(sin(cameraDirectionH * 0.01), 0.0f, cos(cameraDirectionH * 0.01));
        glm::vec3 directionV = glm::vec3(0.0f, cameraDirectionV * 0.02, 0.0f);

        direction = glm::normalize(directionH + directionV);
        

        if (moving) {
            eye += movement;
        }

        view = glm::lookAt(eye, eye + direction, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 perspective = glm::mat4(1.0f);
        perspective = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

        glm::mat4 inverse = glm::inverse(view);
        glm::vec3 cameraPos = glm::vec3(inverse[3]);

        unsigned int cameraPosition = glGetUniformLocation(currentProgram, "cameraPos");
        unsigned int viewPassed = glGetUniformLocation(currentProgram, "view");
        unsigned int perspectivePassed = glGetUniformLocation(currentProgram, "projection");

        glUniform3f(cameraPosition, cameraPos.x, cameraPos.y, cameraPos.z);
        glUniformMatrix4fv(perspectivePassed, 1, GL_FALSE, glm::value_ptr(perspective));
        glUniformMatrix4fv(viewPassed, 1, GL_FALSE, glm::value_ptr(view));
    }

    void Rotate(float degrees, float X, float Y, float Z) {
        view = glm::rotate(view, glm::radians(degrees), glm::vec3(0.0f + X, 0.0f + Y, 0.0f + Z));
    }
};

using namespace std::string_literals;

const char* solidVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in vec3 normals;\n"

"out vec2 TexCoord;\n"
"out vec3 vPos;\n"

"out vec3 normal;\n"
"out vec3 currentPos;\n"

"float waveSize = 7;\n"
"float waveSpeed = 0.5;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform float time;\n"
"out float fTime;\n"
"void main()\n"
"{\n"
" fTime = time;\n"
//" time = time * 0.5f;\n"
/*" currentPos = vec3(model * vec4(aPos, 1.0));\n"
" gl_Position = projection * view * model * vec4(aPos.x, aPos.y, "

" (sin(time * waveSpeed * 5 + aPos.x * waveSize) + cos(time * waveSpeed * 6 + aPos.x * waveSize) * 0.4 + (cos(time * waveSpeed * 5 + aPos.x * (1 + sin((time * waveSpeed * (cos(time) + 1) * 0.1)))* 0.7)) "
" + sin(time * waveSpeed * 5 + aPos.y * waveSize) + cos(time * waveSpeed * 6 + aPos.y * waveSize) * 0.4 + (cos(time * waveSpeed * 5 + aPos.y * (1 + sin((time * waveSpeed * (cos(time) + 1) * 0.1)))) * 0.7)) * 0.03"

", 1.0f);\n"*/
" gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
" TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
" normal = normals;\n"
" vPos = aPos;\n"
"}\n";

const char* waterVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in vec3 normals;\n"

"out vec2 TexCoord;\n"
"out vec3 vPos;\n"

"out vec3 normal;\n"
"out vec3 currentPos;\n"

"float waveSize = 7;\n"
"float waveSpeed = 0.5;\n"

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform float time;\n"
"out float fTime;\n"
"void main()\n"
"{\n"
" fTime = time;\n"
//" time = time * 0.5f;\n"
" currentPos = vec3(model * vec4(aPos, 1.0));\n"
" gl_Position = projection * view * model * vec4(aPos.x, aPos.y, "

" (sin(time * waveSpeed * 5 + aPos.x * waveSize) + cos(time * waveSpeed * 6 + aPos.x * waveSize) * 0.4 + (cos(time * waveSpeed * 5 + aPos.x * (1 + sin((time * waveSpeed * (cos(time) + 1) * 0.1)))* 0.7)) "
" + sin(time * waveSpeed * 5 + aPos.y * waveSize) + cos(time * waveSpeed * 6 + aPos.y * waveSize) * 0.4 + (cos(time * waveSpeed * 5 + aPos.y * (1 + sin((time * waveSpeed * (cos(time) + 1) * 0.1)))) * 0.7)) * 0.03"

", 1.0f);\n"
//" gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
" TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
" normal = normals;\n"
" vPos = aPos;\n"
"}\n";

const char* solidFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"

"in vec3 normal;\n"
"in vec3 currentPos;\n"
"in float fTime;\n"

"in vec3 vPos;\n"

"uniform sampler2D texture;\n"

"uniform vec4 lightColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 cameraPos;\n"

"float normalMapP;\n"


"void main()\n"
"{\n"
" vec3 norm = normalize(normal);\n"
" vec3 lightDirection = normalize(lightPos - currentPos);\n"
" float diffuse = max(dot(norm, lightDirection), 0.0f);\n"

" normalMapP = texture(texture, TexCoord).r;\n"

" vec3 specularDir = normalize(cameraPos - currentPos);\n"
" vec3 reflectionDir = reflect(-specularDir, norm);\n"
" float specularLighting = pow(max(dot(specularDir, reflectionDir), 0.0f), 2);\n"
" FragColor = (vec4(0.0f, 1.0f, 0.0f, 1.0f) + specularLighting * normalMapP) * diffuse * normalMapP;\n"
" FragColor.a = 1.0f;\n"
//" FragColor = texture(texture, TexCoord);\n"
//" FragColor = vec4(vPos, 1.0f);\n"
//" FragColor.a = 1.0f;\n"
//" FragColor.b = normalMapP.b;\n"
"}\n";

const char* neonFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"in vec2 TexCoord;\n"
"vec4 neonColor;\n"

"void main()\n"
"{\n"
" FragColor = neonColor;\n"
"}\n";

const char* frameBufferVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"

"out vec2 TexCoord;\n"

"void main()\n"
"{\n"
" gl_Position = vec4(aPos, 1.0f);"
" TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
"}\n";

const char* frameBufferFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"

"in vec2 TexCoord;\n"
"uniform sampler2D texture;\n"

"void main()\n"
"{\n"
//" FragColor = pixelsSum / 9.0f;\n"
" FragColor = texture(texture, TexCoord);\n"
"}\n";

using namespace Assimp;

class Transform {


public:
    unsigned int objProgram;

    Vector3 position;
    Vector3 scale;
    Vector3 rotation;
    glm::mat4 model;

    void Translate(float X, float Y, float Z) {
        previousProgram = currentProgram;
        currentProgram = objProgram;
        model = glm::translate(model, glm::vec3(0.0f + X, 0.0f + Y, 0.0f + Z));
        unsigned int transLoc = glGetUniformLocation(currentProgram, "model");
        glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(model));
        currentProgram = previousProgram;
    }

    void Rotate(float degrees, float X, float Y, float Z) {
        previousProgram = currentProgram;
        currentProgram = objProgram;
        model = glm::rotate(model, glm::radians(degrees), glm::vec3(0.0f + X, 0.0f + Y, 0.0f + Z));
        unsigned int rotation = glGetUniformLocation(currentProgram, "model");
        glUniformMatrix4fv(rotation, 1, GL_FALSE, glm::value_ptr(model));
        currentProgram = previousProgram;
    }

    void Scale(float X, float Y, float Z) {
        model = glm::scale(model, glm::vec3(0.0f + X, 0.0f + Y, 0.0f + Z));
        unsigned int scale = glGetUniformLocation(currentProgram, "model");
        glUniformMatrix4fv(scale, 1, GL_FALSE, glm::value_ptr(model));
    }
};

class GameObject {
    public:

    Transform transform = Transform();
    

    float vertices[32768];

    unsigned int indices[32768];

    unsigned int program;

    unsigned int texture;
    

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    
    
    const char* objectTexture;

    void Init() {
        transform.model = glm::mat4(1.0f);
        

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        


        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        
        

        

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(objectTexture, &width, &height, &nrChannels, 0);

        if (data) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture";
        }

        
        
    }
};



Importer importer;


void LoadModel(const char* fileDirectory, float objVertices[], unsigned int objIndices[]) {
    const aiScene* scene = importer.ReadFile(fileDirectory, aiProcess_Triangulate);

    aiMesh* mesh = scene->mMeshes[0];

    float vertices[32768];
    float indices[65536];

    for (int i = 0; i < mesh->mNumVertices; i++) {
        vertices[i * 8] = mesh->mVertices[i].x;
        vertices[1 + i * 8] = mesh->mVertices[i].y;
        vertices[2 + i * 8] = mesh->mVertices[i].z;

        vertices[3 + i * 8] = mesh->mTextureCoords[0][i].x;
        vertices[4 + i * 8] = mesh->mTextureCoords[0][i].y;

        vertices[5 + i * 8] = mesh->mNormals[i].x;
        vertices[6 + i * 8] = mesh->mNormals[i].y;
        vertices[7 + i * 8] = mesh->mNormals[i].z;
    }

    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        indices[i * 3] = face.mIndices[0];
        indices[1 + i * 3] = face.mIndices[1];
        indices[2 + i * 3] = face.mIndices[2];
    }

    for (int i = 0; i < mesh->mNumVertices * 8; i++) {
        objVertices[i] = vertices[i];
    }

    for (int i = 0; i < mesh->mNumFaces * 3; i++) {
        objIndices[i] = indices[i];
    }
}

int main()
{
    


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);

    window = glfwCreateWindow(600, 600, "Ngine", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glViewport(0, 0, 600, 600);

    program1 = glCreateProgram();
    program2 = glCreateProgram();
    program3 = glCreateProgram();

    unsigned int solidVertexShader;
    solidVertexShader = glCreateShader(GL_VERTEX_SHADER);

    unsigned int waterVertexShader;
    waterVertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(solidVertexShader, 1, &solidVertexShaderSource, NULL);
    glCompileShader(solidVertexShader);

    glShaderSource(waterVertexShader, 1, &waterVertexShaderSource, NULL);
    glCompileShader(waterVertexShader);

    unsigned int solidFragmentShader;
    solidFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(solidFragmentShader, 1, &solidFragmentShaderSource, NULL);
    glCompileShader(solidFragmentShader);



    glAttachShader(program1, solidVertexShader);
    glAttachShader(program1, solidFragmentShader);
    glLinkProgram(program1);

    glAttachShader(program2, waterVertexShader);
    glAttachShader(program2, solidFragmentShader);
    glLinkProgram(program2);

    glDetachShader(program1, solidFragmentShader);
    glDetachShader(program1, solidVertexShader);
    glDetachShader(program2, solidFragmentShader);
    glDetachShader(program2, waterVertexShader);
    glDeleteShader(solidFragmentShader);
    glDeleteShader(solidVertexShader);
    glDeleteShader(waterVertexShader);


    unsigned int frameBufferVertexShader;
    frameBufferVertexShader = glCreateShader(GL_VERTEX_SHADER);

    unsigned int frameBufferFragmentShader;
    frameBufferFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frameBufferVertexShader, 1, &frameBufferVertexShaderSource, NULL);
    glCompileShader(frameBufferVertexShader);

    glShaderSource(frameBufferFragmentShader, 1, &frameBufferFragmentShaderSource, NULL);
    glCompileShader(frameBufferFragmentShader);

    glAttachShader(program3, frameBufferVertexShader);
    glAttachShader(program3, frameBufferFragmentShader);
    glLinkProgram(program3);

    glDetachShader(program3, frameBufferFragmentShader);
    glDetachShader(program3, frameBufferVertexShader);
    glDeleteShader(frameBufferFragmentShader);
    glDeleteShader(frameBufferVertexShader);


    

    



    glfwSetFramebufferSizeCallback(window, ResizeWindow);

    unsigned int fbo;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);


    unsigned int fboTexture1;
    unsigned int fboTexture2;

    glGenTextures(1, &fboTexture1);
    glBindTexture(GL_TEXTURE_2D, fboTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 600, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture1, 0);

    glGenTextures(1, &fboTexture2);
    glBindTexture(GL_TEXTURE_2D, fboTexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 600, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fboTexture2, 0);



    float fboV[24] = {
        1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int fboVao, fboVbo;
    glGenVertexArrays(1, &fboVao);
    glGenBuffers(1, &fboVbo);

    glBindVertexArray(fboVao);
    glBindBuffer(GL_ARRAY_BUFFER, fboVbo);





    glBufferData(GL_ARRAY_BUFFER, sizeof(fboV), &fboV, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


    GameObject icoSphere;

    LoadModel("G:/SmoothIco.fbx", icoSphere.vertices, icoSphere.indices);


    icoSphere.objectTexture = "G:/Compressed/ground_0031_2k_8WrVeG/Ice.png";
    icoSphere.Init();


    glm::vec4 lightColor = glm::vec4(0.5f, 0.2f, 0.5f, 1.0f);

    
    std::cout << glGetError() << std::endl;

    
    Camera camera;

    glfwSetCursorPosCallback(window, ProcessMouseMovement);
    glfwSetMouseButtonCallback(window, ProcessMouseInput);

    float mxPosLastFrame;
    float myPosLastFrame;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    
    

    while (!glfwWindowShouldClose(window)) {
        


        ProcessInput(window);
        

        

        


        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, 600, 600);
        

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(program1);
        currentProgram = program1;

        glBindTexture(GL_TEXTURE_2D, icoSphere.texture);
        glBindVertexArray(icoSphere.vao);
        GLfloat rad = 20 * 2.0f;

        camera.Render();
        camera.Init();

        icoSphere.transform.model = glm::mat4(1.0f);
        icoSphere.transform.Scale(0.2f, 0.2f, 0.2f);
        //icoSphere.transform.Translate(0.0f, -1.0f, 0.0f);
        //icoSphere.transform.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);

        lightColor = glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);        


        
        

        

        

        unsigned int time = glGetUniformLocation(currentProgram, "time");
        glUniform1f(time, glfwGetTime());
        

        
        
        
        
        unsigned int light = glGetUniformLocation(currentProgram, "lightColor");
        unsigned int lightPos = glGetUniformLocation(currentProgram, "lightPos");
        
        
        glUniform4f(light, 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform3f(lightPos, 6.0f, 0.3f, 1.0f);
        

        

        glDrawElements(GL_TRIANGLES, sizeof(icoSphere.indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        

        glUseProgram(program3);
        currentProgram = program3;
        glBindVertexArray(fboVao);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, fboTexture1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, fboTexture1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
