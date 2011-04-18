Les paquets suivant doivent être installés pour la compilation sous ubuntu

libsdl-image1.2-dev
libsdl-mixer1.2-dev

libavutil-dev
libavformat-dev
libavcodec-dev
libswscale-dev

Pourquoi QT ?

Le python c'est bien mais ça ne se compile pas.
Du coup on ne découvre les erreurs de type dans les appels de fonction et les fautes de frappes qu'au moment de l'exécution.
En python, j'estime que les 3/4 du temps de debuggage sont consacrés à ça.
Avec QT, tout ce travail est fait par le compilateur et du coup :
- Le code est beaucoup plus fiable (le compilateur voit tout et passe partout !)
- Le temps de debuggage est considérablement réduit : on se concentre sur ce qui compte vraiment

De plus, QT offre :
- Des tonnes de lib disponible sur internet
- Le support des "widget" et des "custom objets" directement dans QT Designer
- Le support des QList
- Plus aucun problème d'accent ! (Les QString : ça marche !)

Sans compter que QT Creator est un super environnement de dev ! et le debuggeur est très efficace.

Bref : que du bonheur !

Les grands principes du programme (par rapport à Videoporama) :

1. Séparation totale des données de l'interface

La timeline ne contient plus aucune données et tout est placé dans des QList.
L'intérêt est d'avoir toujours accès à tout sans devoir passer par l'interface ni par des XML internes.
Cela simplifie terriblement le code !

2. Refonte de l'interface autour d'un player

La fenêtre de preview est maintenant un player temps réel !
L'intérêt est de se passer de mplayer pour les preview et de voir immédiatement ce que ça donne.

3. Ajout des fmt_filters

Cette lib est issue du projet ksquirrel (un morceau de KDE qui reprend ImageMagick)
Cela donne une vingtaine de filtre et effet graphique (réglage luminosité, contraste, gamma, yeux rouge, etc...)
=> Je n'ai pas encore tout exploité !
=> J'ai intégré certains filtres dans les effets Ken burns (zoomer sur une partie sombre d'une photo en l'éclaircissant est un effet terrible !)
4. Remplacement de ffmpeg par libavformat/libavcodec/libswscale

Le principal problème de 0.8.1 est la désynchronisation son/image dans les vidéos : Les lèvres bougent avant ou après la parole !!!!

Je pense que le seul moyen de corriger ça est d'inverser le processus de génération des films :
a-Générer le film en embarquant les sons des vidéos (trame par trame)
b-Reprendre la piste son du fichier généré et lui ajouter la musique
c-Re-muxer le film généré en (a) avec la fichier généré en (b)

A terme et en étant très bon, il doit être possible de tout faire en 1 passe
=> Pour le moment, j'en suis encore à me battre avec la partie vidéo (qui commence à fonctionner).
=> Les bases de temps dans libavformat sont vraiment difficiles à appréhender !

5. Passage en boite des options sur objet

J'ai ajouté tellement de truc que la zone de droite ni suffit plus : Du coup je l'ai supprimé !
Pour modifier un objet, il faut maintenant double cliquer dessus

Un des gros apport de QT est le support des "Widget". J'ai donc reprogrammé toutes les interfaces des boites en utilisant à fond les widgets ce qui permet d'ajouter un maximum d'onglet pour un minimum de code (la séparation des données m'a beaucoup aidé pour cela)
=> Ceci dit, je trouve mon interface encore trop compliqué pour monsieur tout le monde
=> Va falloir encore réfléchir !

6. Séparation du fond et du montage

Le fond est maintenant traité à part et le montage se fait sur un fond transparent.
A terme cela permettra :
a) d'utiliser des vidéos comme fond (passage en mode non linéaire !)
b) d'intégrer des masques de fusion (utilisation des géométries des photos 10x15-12x18 etc ...)
=> Je n'ai pas encore commencé cette partie là

Pour le moment, cela se traduit surtout par 2 pistes dans la timeline
Deux de plus devraient venir dès que je commencerai le son (1 pour le son + 1 pour la musique)
D'autres pistes sont envisageables

7. Création d'un canvas étendu

Le canvas (zone de travail) de 0.8.1 est basé sur la hauteur de l'image finale
J'en ai créé un deuxième basé sur la diagonale de l'image source
Cela permet d'ajouter les rotations (au degré près) dans les effets ken burns

8. Gros travail sur les brush

J'ai retravaillé les brush (fond+texte) pour ajouter :
a) les patternes (hachurée)
b) les dégradés de couleurs (à 2 ou 3 couleurs)
c) des images (cette dernière partie n'est pas encore totalement achevée mais j'ai déjà commencé à construire une librairie avec quelques images récupérées sur flickr. Il reste à permettre l'ajout de n'importe quel fichier+recadrage+filtre)

9. Gros travail sur les shapes

J'ai retravaillé les shapes (fond+texte) pour ajouter les polygones réguliers
Ça non plus ce n'est pas fini car je pense ajouter les rotations de forme et les déformations dynamiques (passage de toutes les shapes en mode polygones à 24 ou 32 points plus calculs des transformations dans les effets ken burns !)

