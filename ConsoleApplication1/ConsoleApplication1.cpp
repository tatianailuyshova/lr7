#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <iostream>
#include "Shader.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "locale"


glm::vec3 cameraPosition = glm::vec3(0.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 256.0f;
float lastY = 256.0f;
bool firstMouse = true;


// mesh 2 — основание цепочки, вращение вокруг оси Z 
float angle2 = 0.0f;
// mesh 0 — вращение вокруг оси Y, зависит от mesh 2
float angle0 = 0.0f;
// mesh 1 — вращение вокруг оси Y, зависит от mesh 0
float angle1 = 0.0f;



const float ANGLE_MIN = -90.0f;
const float ANGLE_MAX = 90.0f;
const float ANGLE_STEP = 0.03f; 


float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    const float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    //  mesh 2: вращение вокруг Z — клавиши Q / E 
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        angle2 = glm::clamp(angle2 + ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        angle2 = glm::clamp(angle2 - ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);

    //  mesh 0: вращение вокруг Y — клавиши R / F 
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        angle0 = glm::clamp(angle0 + ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        angle0 = glm::clamp(angle0 - ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);

    //  mesh 1: вращение вокруг Y — клавиши T / G 
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        angle1 = glm::clamp(angle1 + ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        angle1 = glm::clamp(angle1 - ANGLE_STEP, ANGLE_MIN, ANGLE_MAX);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.03f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main()
{
    setlocale(LC_ALL, "Russian");
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Okno = glfwCreateWindow(800, 600, "Lab 7 - Affine Transformations", NULL, NULL);
    if (!Okno) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(Okno);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader labShader("vertex.glsl", "fragment.glsl");


    Model ourModel("ilyushova lab3.obj");

    glfwSetInputMode(Okno, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(Okno, mouse_callback);

    glm::vec3 lightPos(5.0f, 5.0f, 10.0f);

    std::cout << "=== Управление ===" << std::endl;
    std::cout << "W/S/A/D        - камера" << std::endl;
    std::cout << "Q/E            - mesh 2 (вращение Y, как башня)" << std::endl;
    std::cout << "R/F            - mesh 0 (вращение Z, вверх-вниз)" << std::endl;
    std::cout << "T/G            - mesh 1 (вращение Z, вверх-вниз)" << std::endl;
    std::cout << "ESC            - выход" << std::endl;

    while (!glfwWindowShouldClose(Okno))
    {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(Okno);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        labShader.activate();


        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(2.0f));

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

        labShader.setMat4("projection", projection);
        labShader.setMat4("view", view);
        labShader.setMat4("model", model);
        labShader.setMat3("normalMatrix", normalMatrix);
        labShader.setVec3("viewPos", cameraPosition);

        // Параметры освещения
        labShader.setVec3("light.position", lightPos);
        labShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        labShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        labShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // Параметры материала
        labShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        labShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        labShader.setVec3("material.specular", 0.5f, 0.5f, 0.50f);
        labShader.setFloat("material.shininess", 32.0f);



      


        glm::vec3 pivot2(0.07f, 0.0f, -0.058f);
        glm::mat4 t2 = glm::mat4(1.0f);
        t2 = glm::translate(t2, pivot2);
        t2 = glm::rotate(t2, glm::radians(angle2), glm::vec3(0, 1, 0));
        t2 = glm::translate(t2, -pivot2);


        glm::vec3 pivot0(-0.2f, 0.85f, -0.05f);
        glm::mat4 t0 = t2;                                              
        t0 = glm::translate(t0, pivot0);
        t0 = glm::rotate(t0, glm::radians(angle0), glm::vec3(0, 0, 1)); 
        t0 = glm::translate(t0, -pivot0);


        glm::vec3 pivot1(-0.19f, 1.92f, -0.05f);
        glm::mat4 t1 = t0;                                              
        t1 = glm::translate(t1, pivot1);
        t1 = glm::rotate(t1, glm::radians(angle1), glm::vec3(0, 0, 1)); 
        t1 = glm::translate(t1, -pivot1);


        glm::mat4 t3 = glm::mat4(1.0f);


        ourModel.Draw(labShader, t0, t1, t2, t3);

        glfwSwapBuffers(Okno);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}