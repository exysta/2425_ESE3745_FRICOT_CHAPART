/*
 * mesure_courant.h
 *
 *  Created on: Nov 15, 2024
 *      Author: exysta
 */

#ifndef INC_MYLIBS_MESURE_COURANT_H_
#define INC_MYLIBS_MESURE_COURANT_H_

#define PHASE_QUANTITY 2
#define BUS_QUANTITY 1

#include "shell.h"
int ADC_Start();
int ADC_Read(h_shell_t *h_shell,char **argv,int argc);

#endif /* INC_MYLIBS_MESURE_COURANT_H_ */
