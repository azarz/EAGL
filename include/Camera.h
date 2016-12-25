#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum Camera_Type {
    GROUND,
    SKY
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.25f;

bool keys[1024];
GLfloat lastX = 400;
GLfloat lastY = 300;
GLfloat xoffset;
GLfloat yoffset;
bool firstMouse = true;
bool mouse_on = false;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Eular Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;

    GLfloat deltaTime;
    GLfloat lastFrame;

    //Mod
    Camera_Type type;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),  GLFWwindow* window = nullptr, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH, Camera_Type type = GROUND) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), deltaTime(0.0f), lastFrame(0.0f)
    {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();

        glfwSetKeyCallback(window, key_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        if (type != SKY){
            //Si la caméra n'est pas dans les airs, le fonctionnement est normal
            if (direction == FORWARD)
                this->Position += this->Front * velocity;
            if (direction == BACKWARD)
                this->Position -= this->Front * velocity;
            if (direction == LEFT)
                this->Position -= this->Right * velocity;
            if (direction == RIGHT)
                this->Position += this->Right * velocity;

        } else {
            //Sinon, le déplcement est particulier : y constant et seuls x et z changent linéairement
            if (direction == FORWARD)
                this->Position += glm::vec3(0.0f, 0.0f, -1.0f)  * velocity;
            if (direction == BACKWARD)
                this->Position -= glm::vec3(0.0f, 0.0f, -1.0f) * velocity;
            if (direction == LEFT)
                this->Position -= glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
            if (direction == RIGHT)
                this->Position += glm::vec3(1.0f, 0.0f, 0.0f) * velocity;

        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        if (this->type != SKY){
            xoffset *= this->MouseSensitivity;
            yoffset *= this->MouseSensitivity;

            this->Yaw   += xoffset;
            this->Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (this->Pitch > 89.0f)
                    this->Pitch = 89.0f;
                if (this->Pitch < -89.0f)
                    this->Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Eular angles
            this->updateCameraVectors();
        }
    }

    // Moves/alters the camera positions based on user input
    void Do_Movement()
    {
        GLfloat currentFrame = glfwGetTime();
        this->deltaTime = currentFrame - this->lastFrame;
        this->lastFrame = currentFrame;

        // Camera controls
        if(keys[87] || keys [265])
            this->ProcessKeyboard(FORWARD, this->deltaTime);
        if(keys[83] || keys [264])
            this->ProcessKeyboard(BACKWARD, this->deltaTime);
        if(keys[65] || keys [263])
            this->ProcessKeyboard(LEFT, this->deltaTime);
        if(keys[68] || keys [262])
            this->ProcessKeyboard(RIGHT, this->deltaTime);

	if(mouse_on)
	{
	        this->ProcessMouseMovement(xoffset, yoffset);
		mouse_on = false;
	}

        if(this->type == GROUND){
                this->Position.y = 1.0f;

        } else if(this->type == SKY){
                this->Pitch = -70.0f;
                this->Position.y = 25.0f;
        }

    }

    //Switch between camera modes
    void Switch_Mode(){
        if(keys[32]){
            if(this->type == GROUND){
                this->type  =  SKY;
                this->Pitch = -70.0f;
                this->Yaw   =  YAW;
                this->updateCameraVectors();
                this->Up    =  glm::vec3(0.0f, 0.0f, -1.0f);
                this->MovementSpeed = 12*SPEED;
            }else{
                this->type  = GROUND;
                this->Pitch = 0.0f;
                this->updateCameraVectors();
                this->Up    = glm::vec3(0.0f, 1.0f, 0.0f);
                this->MovementSpeed = SPEED;
            }
            keys[32] = false;
        }
    }



private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    xoffset = xpos - lastX;
    yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    mouse_on=true;	
    
    //this->ProcessMouseMovement(xoffset, yoffset);
}
