# 2425_ESE3745_FRICOT_CHAPART

## 1. Séance 1 - Commande MCC basique
### 1.1. Génération de 4 PWM

On génère quatre PWM sur les bras de pont U et V pour contrôler le hacheur à partir du timer déjà attribué sur ces pins.

Cahier des charges :
  - **Fréquence de la PWM** : 20kHz
  - **Temps mort minimum** : à voir selon la datasheet des transistors (faire valider la valeur)
  - **Résolution minimum** : 10bits (ARR > 1024 )

Pour que la PWM est une fréquence de 20kHz, sachant que Fsys = 170 MHz, on doit fixer ARR + 1 = 8500. La résolution est d'environ 13 bits.

Pour les tests, on fixe le rapport cyclique à 60%.
On a 60% = (CCR + 1) / (ARR + 1)
donc on fixe un CCR à 5099 (cela revient à changer le pulse dans l'ioc pour chaque channel).

Pour obtenir un complémentaire décalé, l'autre PWM doit avoir un rapport cyclique de 1-0.6 = 0.4 donc CCR = 3399.
On doit configurer le mode de comptage du timer en center aligned mode 1 pour compter décompter et avoir un complémentaire décalé.

La valeur à mettre dans le registre DTG pour le dead time est 000 10100. On configure un pas d'environ 5.8 ns basé sur notre sysclock à 170 MHz en mettant les 3 premiers bits à 0.
Ensuite, on écrit 20 en binaire sur les 5 derniers bits pour avoir un deadtime total de 5.8 * 20 = 114ns. Cela coresspond à une valeur en décimal de 20 à mettre dans l'ioc dans la partie DeadTime.

Une fois les PWM générées, on les affiche sur un oscilloscope.

### 1.2. Commande de vitesse

Pour controler la vitesse du moteur, nous allons envoyer une séquence via la liaison UART (par l'USB) de la forme :

speed XX

Le traitement de cette chaîne de caractère se fait de la manière suivant :
  - Détection des premiers caractères "speed",
  - Conversion de tous les caractères représentant des chiffres XXXX en nombre entier,
  - Vérification de la valeur (si la valeur est supérieur au maximum autorisé, on l'abaisse à cette valeur),
  - Application de cette vitesse au moteur à travers le registre gérant le rapport cyclique de la PWM.

### 1.3. Premiers tests

Nous effectuons les premiers tests dans les conditions suivantes (dans l'ordre) :
  - Rapport cyclique de 50%
  - Rapport cyclique de 70%

Avec un rapport cyclique de 50%, le moteur ne tourne pas, cela s'explique par le fait que la tension moyenne vu par le moteur est nulle Lorsque l'on modifie le rapport cyclique à 70%, le moteur tourne dans un sens eet à 30%, il tourne à la même vitesse dans l'autre sens.

Pour palier à ce problème, nous générons une montée progressive du rapport cyclique jusqu'à arriver à la vitesse cible commandé par la commande définie précédemment.

## 2. Séance 2 - Commande en boucle ouverte, mesure de Vitesse et de courant

Les objectifs de cette partie sont :
  - Commander en boucle ouverte le moteur avec une accélération limitée,
  - Mesurer le courant aux endroits adéquat dans le montage,
  - Mesurer la vitesse à partir du codeur présent sur chaque moteur.

### 2.1. Commande de la vitesse

Nous ajoutons les fonctionnalités suivantes à notre projet :
  - **Commande start** : permet de fixer le rapport cyclique à 50% (vitesse nulle) et d'activer la génération des pwm (HAL_TIM_PWM_Start et HAL_TIMEx_PWMN_Start),
  - **Commande stop** : permet de désactiver la génération des PWM.
  - **Commande speed XXXX** : permet de définir le rapport cyclique à XXXX/PWM_MAX, mais afin de réduire l'appel à courant, nous avons établie une montée progressive à cette vitesse en quelques secondes. Nous avons effectué une rampe entre la valeur actuelle et la valeur cible avec un incrément de la PWM à un intervalle de temps régulier.

### 2.2. Mesure du courant

Nous devons mesurer les courants suivants : 
  - Bus_Imes → PC2
  - U_Imes → PA1
  - V_Imes → PB1
    
  - Etablir une première mesure de courant avec les ADC en Pooling. Faites des tests à vitesse nulle, non nulle, et en charge (rajouter un couple resistif en consommant du courant sur la machine synchrone couplée à la MCC).
  - Une fois cette mesure validée, modifier la méthode d'acquisition de ces données en établissant une mesure à interval de temps régulier avec la mise en place d'une la chaine d'acquisition Timer/ADC/DMA.
  - Vous pouvez utiliser le même timer que celui de la génération des PWM pour que les mesures de courant soit synchrone aux PWM. Pour vérifier cela, utiliser un GPIO disponible sur la carte pour établir une impulsion lors de la mesure de la valeur.
