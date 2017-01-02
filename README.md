# EAGL

Emulsion Artistique en openGL   
or   
Especially Artistic usage of openGL

Projet openGL du binôme composé par Aranud Grégoire et Amaury Zarzelli

## Modules implémentés
+ Lumière dynamique : 
    - 100%   
+ Caméra réaliste :
    - 100% (changement de mode en appuyant sur la barre d'espace)  
+ Génération aléatoire du monde :
    - 100% (paramétrisation en haut de main.cpp)  
+ Mouvements planifiés :
    - 33% (Nuages, soleil, navette impériale, reste voiture et caméra)  

## COMMENT UTILISER ?

Pour se déplacer, utiliser les **flèches directionnelles ou ZQSD** et la souris.
Pour changer de type de caméra, appuyer sur la **barre d'espace**.

Les principaux paramètres modifiables du programme se situent **en haut de main.cpp**.

Les diverses classes et l'implémentation du View Frustum Culling (permettant de ne pas afficher les objets se situant hors du cône de vision, et d'augmenter très significativement les performances) proviennent du site Lighthouse3d http://www.lighthouse3d.com/tutorials/view-frustum-culling/, elles ont ensuite été modifiées pour être adaptées à notre programme.
