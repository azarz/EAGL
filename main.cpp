// Std. Includes
#include <string>
#include <vector>
#include <ctime>

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
GLuint screenWidth = 1280, screenHeight = 720;

GLfloat ANGLE_CREPUSC(3*M_PI/5);
GLfloat ANGLE_AUBE(4*M_PI/3);
//Un cycle dure par défaut 240 secondes : 2 min de jour, 2 min de nuit
GLint DUREE_CYCLE(60);

int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "EAGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);        
   
    glewExperimental = GL_TRUE;
    glewInit();
    
    // Define the viewport dimensions
    int width, height;
    // On recupere les dimensions de la fenetre creee plus haut
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);

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

	
    GLint nbTriangles;

    //Définition du sol, carré allant de -50,-50 à 50,50
    GLfloat vertices[] = {
        /*     Positions    |      Normales     |     UV     */
        -500.0f,  0.0f, -500.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1250.0f, // Top Left
        -500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Bottom Left
         500.0f, -0.0f, -500.0f,   0.0f, 1.0f, 0.0f,   1250.0f, 1250.0f, // Top Right
         500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f,   1250.0f, 0.0f  // Bottom Right
    };

    GLshort indices[]{
        0, 1, 2,
        1, 2, 3
    };

    nbTriangles = 2;

    

    //Définition d'une grille pour pouvoir définir des altitudes
    //Plus gourmand, donc non implémenté pour l'instant
    //Pour l'implémenter, il suffira de décommenter le bloc suivant et de commenter
    //la précédente définition de vertices et indices

//    int deltaX = 50;
//    int deltaZ = 50;

//    GLfloat altitude;
//    float uStep = 1.25f;
//    float vStep = 1.25f;

//    std::vector<GLfloat> verticesV;
//    std::vector<GLshort> indicesV;

//    for (int Z = -deltaZ; Z < deltaZ; Z++) {
//        for (int X = -deltaX; X < deltaX; X++) {

//            if(true){
//                altitude = 0.0f;
//            } else {
//                altitude = 0.0f;
//            }

//            glm::vec3 vertex1(X, altitude, Z);
//            glm::vec3 vertex2(X, altitude, Z+1);
//            glm::vec3 vertex3(X+1, altitude, Z+1);

//            glm::vec3 edge1 = vertex2 - vertex1;
//            glm::vec3 edge2 = vertex3 - vertex1;
//            glm::vec3 normal = glm::cross(edge1, edge2);
//            normal.normalize();

//            verticesV.push_back(vertex1.x);
//            verticesV.push_back(vertex1.y);
//            verticesV.push_back(vertex1.z);

//            verticesV.push_back(normal.x);
//            verticesV.push_back(normal.y);
//            verticesV.push_back(normal.z);

//            verticesV.push_back(X * uStep);
//            verticesV.push_back(Z * vStep);
//        }
//    }

//    for (int i = 0; i < 4*deltaZ-1; i++) {
//        for (int j = 0; j < 4*deltaX-1; j++) {
//            indicesV.push_back(i * 2*deltaX + j);
//            indicesV.push_back((i+1) * 2*deltaX + j);
//            indicesV.push_back((i+1) * 2*deltaX + (j+1));

//            indicesV.push_back((i+1) * 2*deltaX + (j+1));
//            indicesV.push_back(i * 2*deltaX + (j+1));
//            indicesV.push_back(i * 2*deltaX + j);
//        }
//    }

//    nbTriangles = indicesV.size()/3;

//    GLfloat vertices[verticesV.size()];
//    GLshort indices[indicesV.size()];

//    for(int i(0); i<verticesV.size(); i++){
//        vertices[i] = verticesV[i];
//    }

//    for(int i(0); i<indicesV.size(); i++){
//        indices[i] = indicesV[i];
//    }


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
    // Attribut des normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Attribut des coordonnées UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	
    Camera camera(glm::vec3(0.0f, 1.0f, 0.0f), window);
    
    // On charge les modèles
    Model maison("model/House/house.obj");
    Model soleil("model/Sun/soleil.obj");
    Model lamp("model/Lamp/Lamp.obj");
    Model lit_lamp("model/Lamp/litLamp/Lamp.obj");
    Model arbre1("model/Trees/Tree1/Tree1.3ds");
    Model arbre2("model/Trees/Tree2/Tree2.3ds");
    Model cloud("model/Cloud/nuage2.obj");
    Model shuttle("model/Shuttle/tyderium.obj");
    Model car("model/Car/car.obj");


    //Initialisation des nuages
    GLfloat vitesseNuages;
    GLint nbNuages;
    srand(std::time(NULL)); //Initialise la seed de manière différente à chaque lancement du programme
    vitesseNuages = rand()%51/5; //Vitesse des nuages, entre 0 et 5 écrans/s
    nbNuages = rand()%501; //Nombre de nuages, entre 0 et 500

    GLfloat xNua[nbNuages];
    GLfloat zNua[nbNuages];

    //Positions initiales des nuages
    for(int i(0); i < nbNuages; i++){
        xNua[i] = (rand()%1001) - 500;
        zNua[i] = (rand()%1001) - 500;
    }




    // Boucle principale
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.Do_Movement();
        //Fonction permettant de changer de mode de caméra, appelée si la barre espace est enfoncée
        camera.Switch_Mode();

        //Récupération de la position de la vue, pour la lumière spéculaire
        glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // On récupère les identifiants des variables globales du shader
        GLint modelLoc = glGetUniformLocation(shader.Program, "model");

        glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 500.0f);
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
        GLint ambientStrength = glGetUniformLocation(shader.Program, "ambientStrength");
        GLint specularStrength = glGetUniformLocation(shader.Program, "specularStrength");
        GLfloat ambStr(0.05f);
        GLfloat specStr(0.3f);
        //On la positionne en relatif par rapport à la caméra
        glm::vec4 lPos(camera.Position.x, camera.Position.y + 450.0f, camera.Position.z, 1.0f);
        glm::vec3 lColor(1.0f, 1.0f, 1.0f);

        glm::mat4 rot;
        GLfloat angle = glm::mod(2*M_PI*(glfwGetTime()/DUREE_CYCLE), 2*M_PI);
        rot = glm::rotate(rot, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        lPos = rot * lPos;

        glm::vec3 clearColor;
        //Si le 'soleil' est en-dessous de l'horizon, il ne fait plus de lumière, il fait nuit
        //La lumière ambiante est faible et de couleur bleue foncée
        if (angle > ANGLE_CREPUSC && angle < ANGLE_AUBE){
            lColor = glm::vec3(0.0f, 0.0f, 0.05f);

            //Couleur du ciel de nuit
            clearColor = glm::vec3(0.0f, 0.0f, 0.1f);

            //Lumière ambiante faible
            ambStr = 0.05f;

        //Lumière crépusculaire, orangée et diminuant au fil du temps
        } else if (angle > M_PI/4 && angle < ANGLE_CREPUSC){
            //Variable x utilisée : vaut 1 au début du crépuscule, et 0 à la fin
            double x = (ANGLE_CREPUSC - angle)/(ANGLE_CREPUSC - M_PI/4);

            //Lumière orangée, le 0.05f en B permet la continuité avec la nuit
            lColor = glm::vec3(x * 1.0f, pow(x, 2) * 1.0f, pow(x, 4) * 1.0f + 0.05f);

            //Couleur du 'ciel', le polynôme en x utilisé pour le calcul de R est construit de telle sorte que R(0) = R(1) = 0 et R(0.5) = 0.5
            //Le 0.2f en B permet la continuité avec la nuit
            clearColor = glm::vec3((-2*pow(x,2) + 2*x) * 1.0f, pow(x, 2) * 0.5f, pow(x, 4) * 1.0f + 0.1f);

            //Lumière ambiante décroissante et continue, cohérente avec les valeurs de jour et de nuit
            ambStr = x * 0.65f + 0.05f;


        //Lueur de l'aube
        } else if (angle > ANGLE_AUBE && angle < 5*M_PI/3){
            //Variable x utilisée : vaut 0 au début de l'aube, et 1 à la fin
            double x = (ANGLE_AUBE - angle)/(ANGLE_AUBE - 5*M_PI/3);

            //Lueur rosée-bleutée, le 0.05f en B permet la continuité avec la nuit
            lColor = glm::vec3(pow(x,4) * 1.0f, pow(x, 2) * 1.0f, x * 1.0f + 0.05f);

            //Couleur du 'ciel', le polynôme en x utilisé pour le calcul de R est construit de telle sorte que R(0) = R(1) = 0 et R(0.5) = 0.5
            //Le 0.2f en B permet la continuité avec la nuit
            clearColor = glm::vec3((-2*pow(x,2) + 2*x) * 1.0f, pow(x, 2) * 0.5f, pow(x, 4) * 1.0f + 0.1f);

            //Lumière ambiante croissante et continue, cohérente avec les valeurs de jour et de nuit
            ambStr = x * 0.65f + 0.05f;


        //Lumière de jour
        } else{
            //Lumière blanche
            lColor = glm::vec3(1.0f, 1.0f, 1.0f);

            //Ciel bleu
            clearColor = glm::vec3(0.0f, 0.5f, 1.0f);

            //Lumière ambiante forte
            ambStr = 0.7f;
        }

        //On colore le ciel et on met à jour les variables globales
        glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
        glUniform3f(lightPos, lPos.x, lPos.y, lPos.z);
        glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);
        glUniform1f(ambientStrength, ambStr);
        glUniform1f(specularStrength, specStr);
		
        // On met a jour les variables globales du shader
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


        // On s'occupe de la lumière des lampadaires
        GLint lampPos = glGetUniformLocation(shader.Program, "lampPos");
        GLint lampColor = glGetUniformLocation(shader.Program, "lampColor");

        //Couleur lampe à sodium, si il fait sombre
        glm::vec3 lmpColor;
        if (angle > 2*M_PI/5 && angle < 8*M_PI/5){
            lmpColor = glm::vec3(1.0f, 0.56f, 0.17f);
        } else {
            lmpColor = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);

        //Position utilisée plus tard pour le placement du modèle
        glm::vec3 lmpPos(4.0f, 1.6f, -7.0f);
        glUniform3f(lampPos, lmpPos.x, lmpPos.y , lmpPos.z);


        // On dessine le sol
        glDrawElements(GL_TRIANGLES, 3*nbTriangles, GL_UNSIGNED_SHORT, 0);



        // Dessin des objets :

        // Soleil
        model = glm::mat4(1.0f);
        //On le positionne en relatif par rapport à la caméra
        model = glm::translate(model, glm::vec3(camera.Position.x, camera.Position.y + 490.0f, camera.Position.z));
        model = rot*model;
        model = glm::scale(model, glm::vec3(18.0f));
        // On remet a jour la variable globale du shader, avec une lumière ambiante et une couleur propres au soleil
        // De plus, la lumière des lampadaires ne l'affecte pas
        glUniform1f(ambientStrength, 5.0f);
        glUniform3f(lampColor, 0.0f, 0.0f , 0.0f);
        glUniform3f(lightColor, lColor.x + 0.4f, clearColor.y, 0.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // On redessine l’objet
        soleil.Draw(shader);
        //on repasse au variables de shader normales
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);
        glUniform1f(ambientStrength, ambStr);
        glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);


        //Lampadaire
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lmpPos.x, 0.0f, lmpPos.z));
        //model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        // De nuit, le lampadaire est allumé: il change de couleur et brille
        if(lmpColor.x != 0.0f){
            glUniform1f(ambientStrength, 0.7f);
            glUniform3f(lightColor, lmpColor.x, lmpColor.y, lmpColor.z);

            lit_lamp.Draw(shader);
            //on repasse au variables de shader normales
            glUniform1f(ambientStrength, ambStr);
            glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);
        } else{
            lamp.Draw(shader);
        }

        // Maisons
        // Maison1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        maison.Draw(shader);

        // Maison2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f, 0.0f, -4.0f));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3( 0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.1f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        maison.Draw(shader);


        // Arbres
        // Arbre1
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 3.0f));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.3f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        arbre1.Draw(shader);

        // Arbre2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(6.0f, 0.0f, -3.0f));
        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.7f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        arbre2.Draw(shader);

        // Nuages, non affectés pas la lumière des lampadaires
        glUniform3f(lampColor, 0.0f, 0.0f , 0.0f);
        //On parcourt la liste initialisée anvant la rentrée dans la boucle while
        for(int i(0); i < nbNuages; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(glm::mod((GLfloat)(xNua[i] + glfwGetTime()*vitesseNuages + 500),1000.0f) - 500,
                                                    30.0f, //Altitudde commune aux nuages
                                                    zNua[i])); //Les nuages se déplacent d'ouest en est (vent de mer, par exemple)
            model = glm::scale(model, glm::vec3(3.0f));
            // On remet a jour la variable globale du shader
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // On redessine l’objet
            cloud.Draw(shader);
        }
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);


        // Navette Impériale
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 15.0f, 2.0f));
        model = glm::rotate(model, (GLfloat) 0.0, glm::vec3(-1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.004f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        shuttle.Draw(shader);


        // Voiture
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f));
        // On remet a jour la variable globale du shader, en ajoutant de la lmuière spéculaire, la voiture reflète la lumière
        glUniform1f(specularStrength, 6.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        car.Draw(shader);
        glUniform1f(specularStrength, specStr);

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
