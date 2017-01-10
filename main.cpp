// Std. Includes
#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <map>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Vec3.h"
#include "FrustumG.h"


// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL/SOIL.h>


// ------------------------- PARAMETRES DU PROGRAMME -------------------------------------------------------------------------
// Dimensions de la fenêtre, par défaut 1280 et 720
GLuint screenWidth = 1280, screenHeight = 720;

//Angles de la lumière à partir duquel on est en phase de crépuscule ou d'aube
//A priori, à ne pas changer, valeurs par défaut 3*M_PI/5 et 4*M_PI/3
GLfloat ANGLE_CREPUSC(3*M_PI/5);
GLfloat ANGLE_AUBE(4*M_PI/3);

//Durée du cycle jour/nuit. Par défaut 240 secondes : 2 min de jour, 2 min de nuit
GLint DUREE_CYCLE(240);

//Nombre d'îlots urbains à générer. Par défaut : 16
//Valeurs pour avoir un carré sans trous : les carrés de nombres pairs
//ATTENTION, GOURMAND (surtout au delà de 40-50)
GLint nbIlots(36);

//Probabilité d'apparition des différents types d'ilots. (la dernière est égale à 100 - la somme des autres
//Par défaut : 35, 18, 22, 7, 8
//Etant donné que les arbres sont gourmands, on minmisera leur probabilité (surtout arbres1, et maisons2)
GLint probMaison1(35);
GLint probMaison2(18);
GLint probMaison3(22);
GLint probArbre1(7);
GLint probArbre2(8);
// probArbre3 = 100 - (probMaison1 + probMaison2 + probMaison3 + probArbre1 + probArbre2)

//Booléen concernant la présence de la Croutitower. S'il est true, elle apparaîtra forcément
//Sinon, elle n'apparaîtra pas. Par défaut true
bool tower(true);

//Vitesse des voitures, par défaut 6.0f
GLfloat car_speed(6.0f);

//Nombre de voitures inférieur ou égal au nombre d'îlots, par défaut 6
GLint nbVoitures(20);

//---------------- FIN DES PARAMETRES DU PROGRAMME -------------------------------------------------------------------------

//énumération des différents types d'îlots urbains
enum Type_Ilot{
    MAISONS1,
    MAISONS2,
    MAISONS3,
    ARBRES1,
    ARBRES2,
    ARBRES3,
    TOWER
};



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
    glfwSwapInterval(1);     //Force VSync

    //On active le face culling pour une meilleure performance (cela permet de ne pas afficher les faces non
    //visibles des polygones, et donc de ne pas faire les calculs coûteux de shader sur ces faces)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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

    //Définition du sol, carré allant de -500, -500 à 500, 500
    GLfloat vertices[] = {
        /*     Positions    |      Normales     |     UV     */
        -500.0f,  0.0f, -500.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1250.0f, // Top Left
        -500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Bottom Left
         500.0f,  0.0f, -500.0f,   0.0f, 1.0f, 0.0f,   1250.0f, 1250.0f, // Top Right
         500.0f,  0.0f,  500.0f,   0.0f, 1.0f, 0.0f,   1250.0f, 0.0f,  // Bottom Right
    };

    GLshort indices[]{
        0, 1, 2,
        1, 3, 2,
    };

    nbTriangles = sizeof(indices)/3;

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


    //On initialise notre frustum (cône) (classe FrustumG du tutoriel du site lighthouse3D) pour réaliser du view frustum culling,
    //C'est à dire que l'on évite de faire le rendu des objets qui ne sont pas à l'écran. Cela améliore grandement la performance.
    FrustumG frustum;
    frustum.setCamInternals(60.0f, (float)screenWidth/(float)screenHeight, 0.1f, 350.0f);


    // On charge les modèles, avec le rayon de la sphère à tester pour le frustum culling
    // Si cette sphère est dans le cône de vison ou l'intersecte, on dessine l'objet
    Model maison("model/House/house.obj", 7.5f);
    Model soleil("model/Sun/soleil.obj", 350.0f);
    Model lamp("model/Lamp/Lamp.obj", 3.0f);
    Model lit_lamp("model/Lamp/litLamp/Lamp.obj", 3.0f);
    Model arbre1("model/Trees/Tree1/Tree1.3ds", 4.0f);
    Model arbre2("model/Trees/Tree2/Tree2.3ds", 9.0f);
    Model cloud("model/Cloud/nuage2.obj", 6.0f);
    Model shuttle("model/Shuttle/tyderium.obj", 40.0f);
    Model car("model/Car/car.obj", 2.0f);
    Model croutitower("model/Croutitower/croutitower.obj", 45.0f);
    Model grass("model/Grass/jardin4.obj", 25.0f);
    Model road("model/Road/route3.obj",22.0f);
    Model road2("model/Road/route3-2.obj",20.0f);
    Model magnesie("model/Warehouse/entrpot.obj", 350.0f);



    //Initialisation des nuages
    GLfloat vitesseNuages;
    GLint nbNuages;
    srand(std::time(NULL)); //Initialise la seed de manière différente à chaque lancement du programme
    vitesseNuages = rand()%51/5; //Vitesse des nuages, entre 0 et 10 écrans/s
    nbNuages = rand()%701; //Nombre de nuages, entre 0 et 700

    GLfloat xNua[nbNuages];
    GLfloat zNua[nbNuages];

    //Positions initiales des nuages
    for(int i(0); i < nbNuages; i++){
        xNua[i] = (rand()%1001) - 500;
        zNua[i] = (rand()%1001) - 500;
    }




    //Initialisation des ilots urbains (patés de maisons):
    GLfloat xIlot[nbIlots];
    GLfloat zIlot[nbIlots];
    Type_Ilot typeIlot[nbIlots];
    GLint intervalle(34);       //intervalle entre 2 centres d'îlots
    int emplacement_tour(0);

    //Pour sécurité : on peut rentrer un nombre d'ilots nul
    if (nbIlots == 0){
        tower = false;
    }


    //On sélectionne l'emplacement de la Croutitower
    if(tower){
        emplacement_tour = rand()%nbIlots;
    }

    //Positions des centres d'ilot, et type d'ilot déterminé aléatoirement
    for(int i(0); i < nbIlots; i++){

        GLint j(0); //Distance du carré auquel appartient l'ilot par rapport au centre
        GLint n(i); //Numéro absolu de l'ilot, puis, après la boucle while, numéro de l'ilot dans son carré
        GLint k(4); //Nombre d'ilots sur le carré courant

        while(k<=n){ //Boucle permettant d'obtenir la valeur de j et de n, qui restent aux valeurs initiales sur
            j++;     // le premier carré (4 ilots autour du centre)
            n-=k;
            k+=8;
        }


        GLint l = 2 * (j+1); //Largeur du carré courant
        GLint xzMax = intervalle*(j + 0.5f); //Valeur absolue maximale du x ou du z du centre de l'ilot

        if(n < l-1){ //Arête ouest du carré (sauf son ilot le plus au sud)
            xIlot[i] = -xzMax;
            zIlot[i] =  xzMax - intervalle*(n + 1);
        } else if( l-1 <= n &&  n < 2*l - 2 ){ //Arête nord du carré (sauf son ilot le plus à l'ouest)
            zIlot[i] = -xzMax;
            xIlot[i] = -xzMax + intervalle*(n - (l-2));
        } else if( 2*l-2 <= n && n < 3*l - 3){ //Arête est du carré (sauf son ilot le plus au nord)
            xIlot[i] =  xzMax;
            zIlot[i] = -xzMax + intervalle*(n - (2*l-3));
        } else{ //Arête sud du carré (sauf son ilot le plus à l'est
            zIlot[i] = xzMax;
            xIlot[i] = xzMax - intervalle*(n - (3*l-4));
        }

        if(j==0){      //Cas particulier du permier carré (pour pouvoir faire un enroulement en escargot, à la manière
            if(n==0){  //des arrondissements parisiens
                xIlot[i] = -xzMax;
                zIlot[i] = -xzMax;
            } else if(n==1){
                xIlot[i] =  xzMax;
                zIlot[i] = -xzMax;
            } else if(n==2){
                xIlot[i] =  xzMax;
                zIlot[i] =  xzMax;
            } else if(n==3){
                xIlot[i] = -xzMax;
                zIlot[i] =  xzMax;
            }
        }


        //Détermination d'un pourcentage aléatoire...
        GLint prob = rand()%101;


        //... dont dépend le type de l'ilot courant, les pourcentages sont déterminés au début du programme
        //Ainsi en faisant la somme des autres probabilités, on obtient les valeur de l'intervalle de
        //longueur la probabilité voulue
        if (prob < probMaison1){
            typeIlot[i] = MAISONS1;
        } else if (probMaison1 <= prob && prob < probMaison1 + probMaison2){
            typeIlot[i] = MAISONS2;
        } else if (probMaison1 + probMaison2 <= prob
                   && prob < probMaison1 + probMaison2 + probMaison3){
            typeIlot[i] = MAISONS3;
        } else if (probMaison1 + probMaison2 + probMaison3 <= prob
                   && prob < probMaison1 + probMaison2 + probMaison3 + probArbre1){
            typeIlot[i] = ARBRES1;
        } else if (probMaison1 + probMaison2 + probMaison3 + probArbre1 <= prob
                   && prob < probMaison1 + probMaison2 + probMaison3 + probArbre1 + probArbre2){
            typeIlot[i] = ARBRES2;
        } else{
            typeIlot[i] = ARBRES3;
        }


        // On force la création de la Croutitower à l'emplacement prévu
        if(tower && i==emplacement_tour){
            typeIlot[i] = TOWER;
        }

    }




    //INIT VOITURES
    //Pour sécurité, on limite le nombre de voitures au nombre d'ilots, car leur position initiale dépend des ilots
    if(nbVoitures > nbIlots){
        nbVoitures = nbIlots;
    }

    //Initialisation des voitures
    //Position des voitures
    glm::vec3 carPos[nbVoitures];
    for(int i(0); i<nbVoitures; i++){
        carPos[i] = glm::vec3(xIlot[i] + 17.0f, 0.4f, zIlot[i] + 13.0f);
    }
    //Vecteur pointant vers l'avant de la voiture
    glm::vec3 carFront[nbVoitures];
    for(int i(0); i<nbVoitures; i++){
        carFront[i] = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    //Initialisation de dates et d'intervalles de temps pour le déplacement de la voiture
    GLfloat deltaT(0);
    GLfloat lastT;
    lastT = glfwGetTime();



    //Initialisation du Local à Magnésie
    //Le nombre de "couches" d'îlots et donné par la fonction  x -> sqrt(x)/2 où x est le nombre d'îlots
    //On le place en périphérie de la ville
    GLint nb_couches;
    nb_couches = ceil(sqrt(nbIlots)/2);

    //Détermination de la position du local, aléatoirement
    GLint xMagn;
    GLint zMagn;
    GLint posMagn = rand()%(nbIlots*2 + 1);
    posMagn -= nbIlots;

    //Détermination si on le situe sur un côté est-ouest ou nord-sud de la ville
    if(posMagn%2 == 0){
        xMagn = 50*nb_couches;
        zMagn = posMagn*34;
    } else {
        zMagn = 50*nb_couches;
        xMagn = posMagn*34;
    }


    // Boucle principale
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.Do_Movement();
        //Fonction permettant de changer de mode de caméra, appelée si la barre espace est enfoncée
        camera.Switch_Mode();

        //Après le mouvement, on met à jour le cône de vision
        frustum.setCamDef(Vec3(camera.Position.x, camera.Position.y, camera.Position.z),
                          Vec3(camera.Position.x + camera.Front.x ,camera.Position.y + camera.Front.y ,camera.Position.z + camera.Front.z),
                          Vec3(camera.Up.x, camera.Up.y, camera.Up.z));

        //Récupération de la position de la vue, pour la lumière spéculaire
        glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        // On récupère les identifiants des variables globales du shader
        GLint modelLoc = glGetUniformLocation(shader.Program, "model");

        glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth/(float)screenHeight, 0.1f, 350.0f);
        glm::mat4 view = camera.GetViewMatrix();

        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));	
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//  Activiation  de la  texture 0
		glActiveTexture(GL_TEXTURE0 );//  Binding  de  notre  texture
		glBindTexture(GL_TEXTURE_2D , texture );//  Association  du  numero  de la  texture  pour le  shader
		glUniform1i(glGetUniformLocation(shader.Program , "maTexture"), 0);

        glBindVertexArray(VAO);

        // Position et couleur de la lumiere du soleil
        GLint lightPos = glGetUniformLocation(shader.Program, "lightPos");
        GLint lightColor = glGetUniformLocation(shader.Program, "lightColor");
        GLint ambientStrength = glGetUniformLocation(shader.Program, "ambientStrength");
        GLint specularStrength = glGetUniformLocation(shader.Program, "specularStrength");
        GLfloat ambStr(0.05f);
        GLfloat specStr(0.3f);
        //On la positionne en relatif par rapport à la caméra, afin qu'elle "suive" l'utilisateur
        glm::vec4 lPos(camera.Position.x, camera.Position.y + 300.0f, camera.Position.z, 1.0f);
        glm::vec3 lColor(1.0f, 1.0f, 1.0f);

        //Matrice de rotation du soleil autour de l'axe Z (les Z négatifs correspondent au nord)
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

            //Lumière orangée, le + 0.05f en B permet la continuité avec la nuit
            lColor = glm::vec3(x * 1.0f, pow(x, 2) * 1.0f, pow(x, 4) * 1.0f + 0.05f);

            //Couleur du 'ciel', le polynôme en x utilisé pour le calcul de R est construit de telle sorte que R(0) = R(1) = 0 et R(0.5) = 0.5
            //Le + 0.1f en B permet la continuité avec la nuit
            clearColor = glm::vec3((-2*pow(x,2) + 2*x) * 1.0f, pow(x, 2) * 0.5f, pow(x, 4) * 1.0f + 0.1f);

            //Lumière ambiante décroissante et continue, cohérente avec les valeurs de jour et de nuit
            ambStr = x * 0.65f + 0.05f;


        //Lueur de l'aube
        } else if (angle > ANGLE_AUBE && angle < 5*M_PI/3){
            //Variable x utilisée : vaut 0 au début de l'aube, et 1 à la fin
            double x = (ANGLE_AUBE - angle)/(ANGLE_AUBE - 5*M_PI/3);

            //Lueur rosée-bleutée, le + 0.05f en B permet la continuité avec la nuit
            lColor = glm::vec3(pow(x,4) * 1.0f, pow(x, 2) * 1.0f, x * 1.0f + 0.05f);

            //Couleur du 'ciel', le polynôme en x utilisé pour le calcul de R est construit de telle sorte que R(0) = R(1) = 0 et R(0.5) = 0.5
            //Le + 0.1f en B permet la continuité avec la nuit
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

        //Couleur lampe à sodium, si il fait sombre (entre le crépuscule et l'aube)
        glm::vec3 lmpColor;
        if (angle > 2*M_PI/5 && angle < 8*M_PI/5){
            lmpColor = glm::vec3(1.0f, 0.56f, 0.17f);
        //De jour, aucune lumière
        } else {
            lmpColor = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        //On met à jour la variable du shader
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);

        //Position utilisée plus tard pour le placement du modèle
        glm::vec3 lmpPos(0.0f, 1.6f, -4.0f);
        glUniform3f(lampPos, lmpPos.x, lmpPos.y , lmpPos.z);




        //DESSIN

        // On dessine le sol
        glDrawElements(GL_TRIANGLES, 3*nbTriangles, GL_UNSIGNED_SHORT, 0);




        // Dessin des objets :

        // Soleil -------------------------------
        model = glm::mat4(1.0f);
        //On le positionne en relatif par rapport à la caméra, afin qu'il "suive" l'utilisateur
        model = glm::translate(model, glm::vec3(camera.Position.x, camera.Position.y + 340.0f, camera.Position.z));
        model = rot*model;
        model = glm::scale(model, glm::vec3(13.0f));
        // On remet a jour la variable globale du shader, avec une lumière ambiante et une couleur propres au soleil
        // De plus, la lumière des lampadaires ne l'affecte pas
        glUniform1f(ambientStrength, 5.0f);
        glUniform3f(lampColor, 0.0f, 0.0f , 0.0f);
        glUniform3f(lightColor, lColor.x + 0.4f, clearColor.y, 0.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // On redessine l’objet
        soleil.Draw(shader, frustum, 0.0f, 0.0f, 0.0f);
        //On repasse au variables de shader normales
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);
        glUniform1f(ambientStrength, ambStr);
        glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);





        //Lampadaire ----------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lmpPos.x, 0.0f, lmpPos.z));
        model = glm::scale(model, glm::vec3(1.0f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        // De nuit, le lampadaire est allumé: il change de couleur et brille
        if(lmpColor.x != 0.0f){
            glUniform1f(ambientStrength, 0.7f);
            glUniform3f(lightColor, lmpColor.x, lmpColor.y, lmpColor.z);

            lit_lamp.Draw(shader, frustum, lmpPos.x, 0.0f, lmpPos.z);
            //On repasse au variables de shader normales
            glUniform1f(ambientStrength, ambStr);
            glUniform3f(lightColor, lColor.x, lColor.y , lColor.z);

        //S'il n'est pas allumé, on le desine normalement
        } else{
            lamp.Draw(shader, frustum, lmpPos.x, 0.0f, lmpPos.z);
        }






        //Local à magnésie ---------------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xMagn, 0.0f, zMagn-4.0f));
        model = glm::scale(model, glm::vec3(80.0f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        magnesie.Draw(shader, frustum, xMagn, 0.0f, zMagn)






        // Nuages, non affectés pas la lumière des lampadaires -------------------------------
        glUniform3f(lampColor, 0.0f, 0.0f , 0.0f);
        //On parcourt la liste initialisée avant la rentrée dans la boucle while
        for(int i(0); i < nbNuages; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(glm::mod((GLfloat)(xNua[i] + glfwGetTime()*vitesseNuages + 500),1000.0f) - 500,
                                                    75.0f, //Altitudde commune aux nuages
                                                    zNua[i])); //Les nuages se déplacent d'ouest en est (vent de mer, par exemple)
            model = glm::scale(model, glm::vec3(3.0f));
            // On remet a jour la variable globale du shader
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // On redessine l’objet
            cloud.Draw(shader, frustum,
                       glm::mod((GLfloat)(xNua[i] + glfwGetTime()*vitesseNuages + 500),1000.0f) - 500,
                       30.0f,
                       zNua[i]);
        }
        glUniform3f(lampColor, lmpColor.x, lmpColor.y , lmpColor.z);





        // Navette Impériale -------------------------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 40.0f, 18.0f));
        model = glm::rotate(model, -(GLfloat)M_PI/2, glm::vec3(0.0f, 1.0f, 0.0f));

        //Mouvement circulaire
        glm::mat4 rot3;
        rot3 = glm::rotate(rot3, -angle*20, glm::vec3(0.0f, 1.0f, 0.0f));
        model=rot3*model;

        GLfloat xNav(0.0f);
        GLfloat zNav(0.0f);

        //Si la Croutitower est présente, la navette lui tourne autour
        if(tower){
            xNav = xIlot[emplacement_tour];
            zNav = zIlot[emplacement_tour];
            glm::mat4 transl;
            transl = glm::translate(transl, glm::vec3(xNav, 0.0f, zNav));
            model = transl*model;
        }
        model = glm::scale(model, glm::vec3(0.008f));
        // On remet a jour la variable globale du shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // On redessine l’objet
        shuttle.Draw(shader, frustum, xNav, 40.0f, zNav);






        // Voitures ----------------------------------------
        //On met à jour les variables temporelles
        deltaT = glfwGetTime() - lastT;
        lastT = glfwGetTime();

        for(int current_car(0); current_car < nbVoitures; current_car++){
            model = glm::mat4(1.0f);
            //On tourne le modèle en fonction de l'orientation de la voiture
            model = glm::rotate(model,
                                (carFront[current_car].z - glm::length(carFront[current_car].z)
                                 + carFront[current_car].x) * (GLfloat)M_PI/2,
                                glm::vec3(0.0f, 1.0f, 0.0f));
            GLint dir;
            dir = rand()%3;
            //Le nombre de "couches" d'îlots et donné par la fonction  x -> sqrt(x)/2 où x est le nombre d'îlots
            //Si la voiture d'approche trop de la limite de la ville,
            //elle ne peut plus qu'aller à droit à la prochaine intersection pour ne pas sortir des limites
            if(glm::length(carPos[current_car].x) + 34.0f >= floor(sqrt(nbIlots)/2)*34.0f + deltaT*car_speed
                    || carPos[current_car].z + 30.0f >= floor(sqrt(nbIlots)/2)*34.0f + deltaT*car_speed
                    || carPos[current_car].z - 38.0f <= -floor(sqrt(nbIlots)/2)*34.0f + deltaT*car_speed){

                dir = 0 ;
            }


            //Si la voiture se déplace selon les x et qu'elle rencontre une intersection,
            //elle tourne à gauche ou à droite ou continue tout droit
            if(carFront[current_car].x != 0 && glm::mod(carPos[current_car].x, 34.0f) <= deltaT*car_speed){
                //tourner à droite
                if(dir == 0){
                    carFront[current_car] = glm::vec3(0.0f, 0.0f, carFront[current_car].x);
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                //tourner à gauche
                }else if(dir == 1){
                    carFront[current_car] = glm::vec3(0.0f, 0.0f, -carFront[current_car].x);
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                //aller tout droit
                }else {
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                }


            //Si la voiture se déplace selon les z et qu'elle rencontre une intersection,
            //elle tourne à gauche ou à droite ou continue tout droit
            } else if(carFront[current_car].z != 0 && glm::mod(carPos[current_car].z + 4.0f, 34.0f) <= deltaT*car_speed){
                //tourner à gauche
                if(dir == 0){
                    carFront[current_car] = glm::vec3(-carFront[current_car].z, 0.0f, 0.0f);
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                //tourner à droite
                }else if(dir == 1){
                    carFront[current_car] = glm::vec3(carFront[current_car].z, 0.0f, 0.0f);
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                //aller tout droit
                }else {
                    carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                    carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
                }


            //Sinon, elle continue tout droit
            } else {
                carPos[current_car].x += carFront[current_car].x * deltaT * car_speed;
                carPos[current_car].z += carFront[current_car].z * deltaT * car_speed;
            }

            //On place la voiture à gauche ou à droite de la route en fonction de son orientation
            glm::mat4 mouvVoiture;
            mouvVoiture = glm::translate(mouvVoiture,
                                         glm::vec3(carPos[current_car].x - carFront[current_car].z * 0.9f,
                                                   carPos[current_car].y,
                                                   carPos[current_car].z + carFront[current_car].x * 0.9f));
            model = mouvVoiture*model;

            model = glm::scale(model, glm::vec3(0.004f));

            // On remet a jour la variable globale du shader, en ajoutant de la lumière spéculaire, la voiture reflètant la lumière
            glUniform1f(specularStrength, 6.0f);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            // On redessine l’objet
            car.Draw(shader, frustum, carPos[current_car].x, carPos[current_car].y, carPos[current_car].z);
            glUniform1f(specularStrength, specStr);
        }







        //Dessin des îlots -------------------------------------
        GLint rotMais2(0); //Facteur de rotations des maisons de type 2, pour une génération aléatoire.
        for(int i(0); i < nbIlots; i++){

            //Maisons de type 1 : Un carré de maisons avec des arbres au milieu
            if(typeIlot[i]==MAISONS1){
                //Dessin des maisons (en carré)
                GLfloat x, y, z;
                for (int house(0); house <10; house++){
                    if(house<3){ //Arête sud
                        model = glm::mat4(1.0f);
                        x = xIlot[i] - 10.0f + house*10.0f;
                        y = 0.0f;
                        z = zIlot[i] - 15.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(0.1f));

                    } else if(3<=house && house<6){ //Arête nord
                        model = glm::mat4(1.0f);
                        x = xIlot[i] - 10.0f + (house-3)*10.0f;
                        y = 0.0f;
                        z = zIlot[i] + 10.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(0.1f));

                    } else if(6 <= house && house<8){ //Arête est
                        model = glm::mat4(1.0f);
                        x = xIlot[i] + 14.2f;
                        y = 0.0f;
                        z = zIlot[i] + 1.25f - (house-6)*10.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3( 0.0f, 0.0f, 1.0f));
                        model = glm::scale(model, glm::vec3(0.1f));

                    } else { //Arête ouest
                        model = glm::mat4(1.0f);
                        x = xIlot[i] - 11.0f;
                        y = 0.0f;
                        z = zIlot[i] + 1.25f - (house-8)*10.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3( 0.0f, 0.0f, 1.0f));
                        model = glm::scale(model, glm::vec3(0.1f));
                    }
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    maison.Draw(shader, frustum, x, y, z);
                }

                //Dessin des sapins
                //Leur position est déterministe, mais différente pour chaque ilot
                for (int tree(0); tree < 4; tree++){
                        model = glm::mat4(1.0f);
                        model = glm::translate(model,
                                               glm::vec3(xIlot[i] + pow(-1,tree)*(i%4 +1)*1.5f + (tree*i)%3*1.4,
                                                         0.0f,
                                                         zIlot[i] + pow(-1,tree)*(i%3 + 1)*tree - 3.0f + pow(-1,i)*(tree%3)*0.8));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(1.3f));
                        // On remet a jour la variable globale du shader
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        // On redessine l’objet
                        arbre1.Draw(shader, frustum,
                                    xIlot[i] + pow(-1,tree)*(i%4 +1)*1.5f + (tree*i)%3*1.4,
                                    0.0f,
                                    zIlot[i] + pow(-1,tree)*(i%3 + 1)*tree - 3.0f + pow(-1,i)*(tree%3)*0.8);
                }

                //Dessin des feuillus
                //Leur position est déterministe, mais différente pour chaque ilot
                for (int tree(0); tree < 2; tree++){
                        model = glm::mat4(1.0f);
                        model = glm::translate(model, glm::vec3(xIlot[i] +1.0f + pow(-1,tree)*(i%2) + pow(-1,i)*tree*7,
                                                                0.0f,
                                                                zIlot[i] + (i%3)*0.5f - 5.0f + (tree*i)%3));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(0.7f));
                        // On remet a jour la variable globale du shader
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        // On redessine l’objet
                        arbre2.Draw(shader, frustum, xIlot[i] + 1.0f + pow(-1,tree)*(i%2) + pow(-1,i)*tree*7,
                                    0.0f,
                                    zIlot[i] + (i%3)*0.5f - 5.0f + (tree*i)%3);
                }



            // Maisons de type 2 : 2 maisons en angle droit avec un sapin et éventuellement un feuillu à côté
            } else if(typeIlot[i]==MAISONS2){
                    // Maisons
                    // Maison1
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(xIlot[i], 0.0f, zIlot[i] - 6.0f));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                    model = glm::rotate(model, (GLfloat) (rotMais2*M_PI/2), glm::vec3( 0.0f, 0.0f, 1.0f));
                    model = glm::scale(model, glm::vec3(0.1f));
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    maison.Draw(shader, frustum, xIlot[i], 0.0f, zIlot[i] - 6.0f);

                    // Maison2
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(xIlot[i]-6.0f, 0.0f, zIlot[i]+3.0f));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3( 0.0f, 0.0f, 1.0f));
                    model = glm::rotate(model, (GLfloat) (rotMais2*M_PI/2), glm::vec3(0.0f, 0.0f, 1.0f));
                    model = glm::scale(model, glm::vec3(0.1f));
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    maison.Draw(shader, frustum, xIlot[i]-6.0f, 0.0f, zIlot[i]+3.0f);

                    // Arbres
                    // Sapin
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(xIlot[i]+2.0f, 0.0f, zIlot[i]+1.0f));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(1.3f));
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    arbre1.Draw(shader, frustum, xIlot[i]+2.0f, 0.0f, zIlot[i]+1.0f);

                    // Feuillu
                    if((i + nbNuages)%4==0){
                        model = glm::mat4(1.0f);
                        model = glm::translate(model, glm::vec3(xIlot[i] + 6.0f, 0.0f, zIlot[i] + 5.0f));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(0.7f));
                        // On remet a jour la variable globale du shader
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        // On redessine l’objet
                        arbre2.Draw(shader, frustum, xIlot[i] + 6.0f, 0.0f, zIlot[i] + 5.0f);
                    }
                    //La prochaine instaciation des maisons de type 2 aura une oriantation différente
                    rotMais2++;


            //La Croutitower
            } else if(typeIlot[i] == TOWER){
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.0f, zIlot[i]-4.0f));
                model = glm::scale(model, glm::vec3(80.0f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet
                croutitower.Draw(shader, frustum, xIlot[i], 35.0f, zIlot[i]-3.0f);



            // Maisons de type 3 : Trois rangées de maisons
            } else if(typeIlot[i] == MAISONS3){
                GLfloat x, y, z;
                for (int house(0); house <9; house++){
                    model = glm::mat4(1.0f);
                    x = xIlot[i] - 10.0f + (house%3)*10.0f;
                    y = 0.0f;
                    z = zIlot[i] - 15.0f + floor(house/3)*12.5f;
                    model = glm::translate(model, glm::vec3(x, y, z));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(0.1f));
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    maison.Draw(shader, frustum, x, y, z);
                }


            // Arbres de type 1 : Un carré de sapins
            } else if(typeIlot[i] == ARBRES1){
                GLfloat x, y, z;
                for (int tree(0); tree < 16; tree++){
                    if(tree < 10){
                        model = glm::mat4(1.0f);
                        x = xIlot[i] - 13.0f + (tree%5)*6.5;
                        y = 0.0f;
                        z = zIlot[i] + pow(-1,floor(tree/5))*13.0f - 4.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(1.3f));
                    } else{
                        model = glm::mat4(1.0f);
                        x = xIlot[i] + pow(-1,floor(tree/3))*13.0;
                        y = 0.0f;
                        z = zIlot[i] - 6.5f + (tree%3)*6.5 - 4.0f;
                        model = glm::translate(model, glm::vec3(x, y, z));
                        model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                        model = glm::scale(model, glm::vec3(1.3f));
                    }

                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    arbre1.Draw(shader, frustum, x, y, z);
                }

                //De l'herbe en dessous et une clôture
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.01f, zIlot[i]-4.0f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet (qui ne reflète pas la lumière, c'est de l'herbe)
                glUniform1f(specularStrength, 0.0f);
                grass.Draw(shader, frustum, xIlot[i], 0.0f, zIlot[i]-4.0f);
                glUniform1f(specularStrength, specStr);


            //Arbres de type 2 : 4 sapins et une maison au centre
            } else if(typeIlot[i] == ARBRES2){
                GLfloat x, y, z;
                for (int tree(0); tree < 4; tree++){
                    model = glm::mat4(1.0f);
                    x = xIlot[i] - 13.0f + (tree%2)*26;
                    y = 0.0f;
                    z = zIlot[i] + pow(-1,floor(tree/2))*13.0f - 4.0f;
                    model = glm::translate(model, glm::vec3(x, y, z));
                    model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                    model = glm::scale(model, glm::vec3(1.3f));
                    // On remet a jour la variable globale du shader
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    // On redessine l’objet
                    arbre1.Draw(shader, frustum, x, y, z);
                }
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.0f, zIlot[i] -3.0f));
                model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.1f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet
                maison.Draw(shader, frustum, xIlot[i], 0.0f, zIlot[i] - 3.0f);

                //De l'herbe en dessous et une clôture
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.01f, zIlot[i]-4.0f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet (qui ne reflète pas la lumière, c'est de l'herbe)
                glUniform1f(specularStrength, 0.0f);
                grass.Draw(shader, frustum, xIlot[i], 0.0f, zIlot[i]-4.0f);
                glUniform1f(specularStrength, specStr);



            //Arbres de type 3 : Un arbre géant au centre
            } else{
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.0f, zIlot[i]-5.0f));
                model = glm::rotate(model, (GLfloat) M_PI/2, glm::vec3(-1.0f, 0.0f, 0.0f));
                model = glm::scale(model, glm::vec3(1.4f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet
                arbre2.Draw(shader, frustum, xIlot[i], 4.0f, zIlot[i]-5.0f);

                //De l'herbe en dessous et une clôture
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.01f, zIlot[i]-4.0f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet (qui ne reflète pas la lumière, c'est de l'herbe)
                glUniform1f(specularStrength, 0.0f);
                grass.Draw(shader, frustum, xIlot[i], 0.0f, zIlot[i]-4.0f);
                glUniform1f(specularStrength, specStr);
            }


            //Dessin des routes
            for(int signe=-1; signe < 2; signe+=2){
                //Routes Est-Ouest
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i], 0.01f, zIlot[i]-4.0f +signe*16.1f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet
                road.Draw(shader, frustum, xIlot[i], 0.01f, zIlot[i]-4.0f +signe*16.1f);

                //Routes Nord-Sud
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(xIlot[i]+ signe*16.1f, 0.01f, zIlot[i]-4.0f));
                // On remet a jour la variable globale du shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                // On redessine l’objet
                road2.Draw(shader, frustum, xIlot[i]+ signe*16.55f, 0.01f, zIlot[i]-4.0f);
            }

        } //Fin de dessin des ilots




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
