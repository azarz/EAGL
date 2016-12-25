// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>

// Properties
GLuint screenWidth = 960, screenHeight = 720;

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);        
   
    glewExperimental = GL_TRUE;
    glewInit();
    
    // Define the viewport dimensions
    int width, height;
    // On recupere les dimensions de la fenetre creee plus haut
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    glEnable(GL_DEPTH_TEST);
    
    Shader shader("shaders/default.vertexshader", "shaders/default.fragmentshader");

    GLuint texture; // Declaration de l'identifiant

	glGenTextures(1, &texture); // Generation de la texture
	// On bind la texture cree dans le contexte global d'OpenGL
	glBindTexture(GL_TEXTURE_2D, texture); 
	// Modification des parametres de la texture
	// Methode de wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	// Methode de filtrage
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Chargement du fichier image en utilisant la lib SOIL
	int twidth, theight;
    unsigned char* data = SOIL_load_image("texture/wall.jpg", &twidth, &theight,
					                            0, SOIL_LOAD_RGB);
	// Association des donnees image a la texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 
					           0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// Generation de la mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// On libere la memoire
	SOIL_free_image_data(data);
	// On unbind la texture
	glBindTexture(GL_TEXTURE_2D, 0);

	
	GLfloat vertices[] = {
        /*     Positions    |      Normales     |     UV     */
        -50.0f,  0.0f, -50.0f,   0.0f, 1.0f, 0.0f,   0.0f, 125.0f, // Top Left
        -50.0f,  0.0f,  50.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Bottom Left
         50.0f, -0.0f, -50.0f,   0.0f, 1.0f, 0.0f,   125.0f, 125.0f, // Top Right
         50.0f,  0.0f,  50.0f,   0.0f, 1.0f, 0.0f,   125.0f, 0.0f  // Bottom Right
    };
    
    GLshort indices[]{
        0, 1, 2,
    	1, 2, 3
    };    
    
    GLuint VBO, VAO, EBO;
    
    glGenVertexArrays(1, &VAO);
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // On met notre EBO dans le contexte global
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // On assigne au EBO le tableau d'indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribut des positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Attribut des couleurs
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Attribut des normales
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	
    Camera camera(glm::vec3(0.0f, 1.0f, 0.0f), window);
    
    // On charge le modele
    Model maison("model/House/house.obj");


    // Game loop
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.Do_Movement();
        //Fonction permettant de changer de mode de caméra, appelée si la barre espace est enfoncée
        camera.Switch_Mode();
        
        glClearColor(0.0f, 0.08f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // On récupère les identifiants des variables globales du shader
        GLint modelLoc = glGetUniformLocation(shader.Program, "model");

        glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));	
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//  Activiation  de la  texture 0
		glActiveTexture(GL_TEXTURE0 );//  Binding  de  notre  texture
		glBindTexture(GL_TEXTURE_2D , texture );//  Association  du  numero  de la  texture  pour le  shader
		glUniform1i(glGetUniformLocation(shader.Program , "maTexture"), 0);

		glBindVertexArray(VAO);

        // Position et couleur de la lumiere
        GLint lightPos = glGetUniformLocation(shader.Program, "lightPos");
        GLint lightColor = glGetUniformLocation(shader.Program, "lightColor");
        glm::vec3 lPos(0.0f, 150.0f, 0.0f);
        glm::vec3 lColor(1.0f, 1.0f, 1.0f);
        glUniform3f(lightPos, lPos.x, lPos.y, lPos.z);
        glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);
		
        // On met a jour les variables globales du shader
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // On dessine l'objet courant (ici, le sol)
        glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_SHORT, 0);

        // Dessin des objets :
        // On associe la matrice unite a notre matrice model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, 0, -10));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f));
        // On remet a jour la variable global du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        maison.Draw(shader);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}


#pragma endregion
