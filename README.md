# 2425_ESE3745_FRICOT_CHAPART

## 1. Séance 1 - Commande MCC basique
### 1.1. Génération de 4 PWM

On génère quatre PWM sur les bras de pont U et V pour contrôler le hacheur à partir du timer déjà attribué sur ces pins.

Cahier des charges :
  - **Fréquence de la PWM** : 20kHz
  - **Temps mort minimum** : à voir selon la datasheet des transistors (faire valider la valeur)
  - **Résolution minimum** : 10bits (ARR > 1024 )

Pour que la PWM est une fréquence de 20kHz, sachant que Fsys = 170 MHz, on doit fixer ARR + 1 = 8500. La résolution est d'environ 13 bits.

Pour les tests, on fixe le rapport cyclique à 60%. donc on fixe un CCR à 5100
Une fois les PWM générées, on les affiche sur un oscilloscope.
