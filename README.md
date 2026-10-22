# EAGL

Emulsion Artistique en openGL
or
Especially Artistic usage of openGL

Projet openGL (ING2) du binôme composé par Arnaud Grégoire et Amaury Zarzelli

Vidéo de présentation : https://www.youtube.com/watch?v=GPBEjxnHNRY

**TODO**
+ Shadow mapping
+ Menus

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

### Prérequis
xorg-dev, libglu1-mesa-dev, libsoil-dev

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
```shell
$ make
```
L'exécutable devrait se trouver dans le dossier EAGL

### Version navigateur

La version navigateur utilise Emscripten, WebAssembly et WebGL 2. Installer un SDK Emscripten, puis compiler depuis la racine du dépôt :

```shell
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web --parallel
```

Servir `build-web` via HTTP pour tester localement, par exemple avec `python3 -m http.server --directory build-web`. Ouvrir ensuite `http://localhost:8000/`.

Le workflow GitHub Actions publie automatiquement la version navigateur sur `https://azarz.github.io/EAGL/` après chaque push sur `master` ou `main`. Dans les paramètres du dépôt, choisir **GitHub Actions** comme source de publication Pages.

Le menu de lancement permet de régler les paramètres suivants (les valeurs entre parenthèses sont celles par défaut) :

- Durée du cycle jour/nuit (240 s) : 2 minutes de jour et 2 minutes de nuit. Valeurs possibles : 1 à 86 400 secondes.
- Nombre d'îlots (4) : pour obtenir un carré sans trous, choisir le carré d'un nombre pair (4, 16 ou 36). La génération devient gourmande au-delà de 40 à 50 îlots. Valeurs possibles : 2 à 50.
- Vitesse des voitures (6) : valeurs possibles de 0 à 30.
- Nombre de voitures (6) : limité automatiquement au nombre d'îlots; avec les 4 îlots par défaut, 4 voitures au maximum sont utilisées.

Les mêmes paramètres peuvent être fournis au lancement de l'exécutable natif, dans le même ordre :

```shell
./EAGL 240 16 6 6
```

Sans arguments, les valeurs par défaut sont utilisées. Les valeurs hors limites sont ramenées dans les plages ci-dessus. Les contrôles restent identiques dans les deux versions : flèches directionnelles ou ZQSD et souris pour se déplacer, **CTRL** pour s'accroupir, **SHIFT** pour sprinter, et **ESPACE** pour changer de caméra.

### Utilisation
Lancer l'exécutable situé dans le dossier EAGL.
Pour se déplacer, utiliser les **flèches directionnelles ou ZQSD** et la souris.
Maintenir **CTRL** (gauche ou droite) pour s'accroupir, maintenir **SHIFT** (gauche ou droite) pour sprinter.

Pour changer de type de caméra, appuyer sur la **barre d'espace**. Les trois modes sont : caméra au sol, caméra aérienne et caméra animée automatiquement.

Les diverses classes et l'implémentation du View Frustum Culling (permettant de ne pas afficher les objets se situant hors du cône de vision, et d'augmenter très significativement les performances) proviennent du site Lighthouse3d http://www.lighthouse3d.com/tutorials/view-frustum-culling/, elles ont ensuite été modifiées pour être intégrées à notre programme dans ses différentes classes.
