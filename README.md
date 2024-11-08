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
On doit configurer le mode de  comptage du timer en center aligned mode 1 pour compter décompter et avoir un complémentaire décalé.

la valeur à mettre dans le registre DTG pour le dead time est 000 10100. on configure un pas d'environ 5.8 ns basé sur notre sysclock à 170 MHz en mettant les 3 premiers bits à 0.
ENsuit, on écrit 20 en binaire sur les 5 derniers bits pour avoir un deadtime total de 5.8 * 20 = 114ns. c'est ce qu'il nous faut. Cela coresspond à une valeur en décimal de 20 à mettre dans l'ioc dans la partie DeadTime.

Une fois les PWM générées, on les affiche sur un oscilloscope.
