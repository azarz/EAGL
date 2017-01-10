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
    - 100% (Nuages, soleil, navette impériale, voitures. Caméra autonome.)  

## COMMENT UTILISER ?

### Compilation
Se placer dans le dossier EAGL à l'aide d'un terminal.
```shell
$ cd "/chemin_dépendant_de_l'emplacement_du_dossier/EAGL"
```
Lancer la commande
```shell
$ cmake .
```
Une fois terminée, lancer la commande
````shell
$ make
```
L'exécutable devrait se trouver dans le dossier EAGL

### Utilisation
Lancer l'exécutable situé dans le dossier EAGL.  
Pour se déplacer, utiliser les **flèches directionnelles ou ZQSD** et la souris.
Maintenir **CTRL** (gauche ou droite) pour s'accroupir, maintenir **SHIFT** (gauche ou droite) pour sprinter.
  
Pour changer de type de caméra, appuyer sur la **barre d'espace**. Les trois modes sont : caméra au sol, caméra aérienne et caméra animée automatiquement.

Les principaux paramètres modifiables du programme se situent **en haut de main.cpp**. **(RECOMPILATION NÉCESSAIRE)**

Les diverses classes et l'implémentation du View Frustum Culling (permettant de ne pas afficher les objets se situant hors du cône de vision, et d'augmenter très significativement les performances) proviennent du site Lighthouse3d http://www.lighthouse3d.com/tutorials/view-frustum-culling/, elles ont ensuite été modifiées pour être intégrées à notre programme dans ses différentes classes.
