# 2425_ESE3745_FRICOT_CHAPART

## 0. Introduction

Ce TP réalisé par Charlotte Fricot et Lucas Chapart a pour but l'asservissement d'un moteur DC répartit sur 3 séances de 4h.
Une maquette réalisé en interne à l'ENSEA est utilisée pour interfacer le moteur DC avec le STM32 qui assurera l'asservissement.

Les objectifs du TP sont les suivants :
  -  Controler la vitesse du moteur à partir de la génération de pwm complémentaires décalées sur les deux phases d'un pont en H.
  -  Mesurer le courant consommé par le moteur et les 2 phases utilisés avec un convertisseur courant/tension et 3 ADC internes au STM32.
  -  Mesurer la vitesse de nos moteurs à partir des encodeurs en utilisant un timer du STM32 configuré en mode enocder.
  -  Réaliser l'assevissement de la vitesse du moteur avec une fonction de transfert de type PID.

Afin de pouvoir controler le moteur, une interface est nécessaire entre un pc est le STM32.
Pour cela, nous utilisons un shell fonctionnant en UART.

A la fin du TP, les commandes suivantes étaients implementées dans le shell:
![image](https://github.com/user-attachments/assets/5874c691-3a30-4214-9140-8d25aa82761f)

## 1. Commande MCC basique
### 1.1. Génération de 4 PWM

On génère quatre PWM sur les bras de pont U et V pour contrôler le hacheur à partir du timer déjà attribué sur ces pins.

Cahier des charges :
  - **Fréquence de la PWM** : 20kHz
  - **Temps mort minimum** : à voir selon la datasheet des transistors 
  - **Résolution minimum** : 10bits (ARR > 1024 )

Pour que la PWM est une fréquence de 20kHz, sachant que Fsys = 170 MHz, on doit fixer ARR + 1 = 8500. La résolution est d'environ 13 bits.

Pour les tests, on fixe le rapport cyclique à 60%.
On a 60% = (CCR + 1) / (ARR + 1)
donc on fixe un CCR à 5099 (cela revient à changer le pulse dans l'ioc pour chaque channel).

Pour obtenir un complémentaire décalé, l'autre PWM doit avoir un rapport cyclique de 1-0.6 = 0.4 donc CCR = 3399.
On doit configurer le mode de comptage du timer en center aligned mode 1 pour compter décompter et avoir un complémentaire décalé.

![image](https://github.com/user-attachments/assets/5427b828-435e-4cea-88f0-e7b1fc20ddbe)

![image](https://github.com/user-attachments/assets/6c29613e-953c-4885-abab-99e330509307)

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

## 2. Commande en boucle ouverte, mesure de Vitesse et de courant

Les objectifs de cette partie sont :
  - Commander en boucle ouverte le moteur avec une accélération limitée,
  - Mesurer le courant aux endroits adéquat dans le montage,
  - Mesurer la vitesse à partir du codeur présent sur chaque moteur.

### 2.1. Commande de la vitesse

Nous ajoutons les fonctionnalités suivantes à notre projet :
  - **Commande start** : permet de fixer le rapport cyclique à 50% (vitesse nulle) et d'activer la génération des pwm (HAL_TIM_PWM_Start et HAL_TIMEx_PWMN_Start),
  - **Commande stop** : permet de désactiver la génération des PWM.
  - **Commande speed XXXX** : permet de définir le rapport cyclique à XXXX/PWM_MAX, mais afin de réduire l'appel à courant, nous avons établie une montée progressive à cette vitesse en quelques secondes. Nous avons effectué une rampe entre la valeur actuelle et la valeur cible avec un incrément de la PWM à un intervalle de temps régulier.
    ![image](https://github.com/user-attachments/assets/d5b5604c-39ee-47f3-9d15-b98c16add0ef)

### 2.2. Mesure du courant

Nous devons mesurer les courants suivants : 
  - Bus_Imes → PC2
  - U_Imes → PA1
  - V_Imes → PB1

D'après la datasheet, la sensibilité du capteur est de 50 mV/A.
Pour V_ref=3.3 V et une résolution de 12 bits : U_mesuré​ = 2063 × (3.3/4095) = 1.66 V

Etablir une première mesure de courant avec les ADC en DMA. Faites des tests à vitesse nulle, non nulle, et en charge (rajouter un couple resistif en consommant du courant sur la machine synchrone couplée à la MCC).

Pour cela, nous utilisons le même timer que celui de la génération des PWM pour que les mesures de courant soit synchrone aux PWM.

Il faut penser à mettre la clock source du timer utilisé en internal clock pour la génération de trigger afin de lancer automatiquement les conversions par le DMA. 

![image](https://github.com/user-attachments/assets/5420be37-af1a-408e-8205-693ee8fae6e7)

Lorsque le moteur est à vitesse nulle, l'adc est à la motié de sa valeur maximale.

On peut théoriser le comportement suivant :
  - Lorsque le courant traversant la phase est nul, le convertisseur génère une tension égale à 
  𝑉𝑅𝐸𝐹/2 pour indiquer qu'il n'y a ni courant positif ni courant négatif.

  - Si Vphase > VREF/2 alors le courant est positif, si Vphase < VREF/2 alors le courant est négatif.

On remarque sur les photos que la modification du rapport cyclique entraîne l'augmentation de la consomation de courant d'une phase et la diminution sur l'autre phase. Cela est cohérent car une de phase va passer plus de temps en condction et l'autre moins.

### 2.3. Mesure de vitesse

D'après la datasheet du capteur, Maximum Data Rate = 32 Mbps.

Les pin du stm32 utilisés pour faire cette mesure de vitesse sont : 
  - ENC_A → PA6
  - ENC_B → PA4
  - ENC_Z → PC8

- Déterminer la fonction de transfert du capteur de vitesse,
    La datasheet du moteur ne précisait pas le nomre de pulses par rotation du moteur ( Pulses Per Rotation = PPR). Nous n'avons pas eu le temps de déterminer expérimentalement ce nombre.
    On aurait pu, par exemple, faire tourner le moteur à sa vitesse nominale, puis, calculer le PPR que l'on détecte.
    La vitesse nominale étant connue, il est possible d'estimer le PPR.
  
- Déterminer la constant de temps mécanique du moteur :
    Pour cela, on peut envoyer un échelon de tension au moteur et analyser la vitesse à partir de la sonde tachymétrique (que l'on n'utilisera pas pour le reste du TP).

- Déterminer la fréquence à laquelle vous allez faire l'asservissement en vitesse du moteur.
    L'asservissement en vitesse doit etre plus lent que l'asserivssement en courant tout en étant 5 à 10 fois plus rapide que la constante de temps mécanique du moteur
  
- Etablir le code de mesure de vitesse et le tester.
  On start l'encoder avec 	  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  On utilise __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3) pour savoir dans quel sens tourne le moteur
  On utilise le timer 16 pour lancer l'acquisiton du compteur de l'encodeur présent sur le timer 3.
  On lance l'acquisiton du compteur tout les 100 ms, la difference de comptage permet de savoir combien de pulses on été détecté sur la période de 100ms.
  Cela nous permet de déterminer le nombre de pulses par minute ou secondes que l'on détecte et l'associer à la vitesse du moteur en tours par minute.
    

## 3. Asservissement
Le principe de l'asservissement est le suivant : 
![image](https://github.com/user-attachments/assets/ad3e4879-2fa8-448c-8a3f-132bf6c81005)

Le calcul des différents coefficients nécessaire pour l'asserviment n'a pas pu être finalisé par manque de temps.
Le TP nécessitant la maquette de l'ENSEA et les moteurs pour travailler, il n'était pas possible de compléter ce travail en dehors des séances de TP.






