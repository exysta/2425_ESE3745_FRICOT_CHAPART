/*
 * shell.h
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */

#ifndef INC_MYLIBS_SHELL_H_
#define INC_MYLIBS_SHELL_H_

#define SHELL_FUNC_LIST_MAX_SIZE 32
#define UART_RX_BUFFER_SIZE 1
#define UART_TX_BUFFER_SIZE 128
#define CMD_BUFFER_SIZE 64
#define MAX_ARGS 9
#define ASCII_LF 0x0A			// LF = line feed, saut de ligne
#define ASCII_CR 0x0D			// CR = carriage return, retour chariot
#define ASCII_BACK 0x08			// BACK = Backspace

#include <stdint.h>

struct h_shell_struct;
typedef int (*shell_func_pointer_t)(struct h_shell_struct *h_shell, char **argv, int argc);

/**
 * @struct shell_func_struct
 * @brief Structure for individual shell functions.
 *
 * Stores the name, function pointer, and description of each shell command.
 */
typedef struct shell_func_struct {
    char *name;                /**< Name of the shell function */
    shell_func_pointer_t func; /**< Function pointer for the shell function */
    char *description;         /**< Description of the shell function */
} shell_func_t;

typedef struct h_shell_struct {
    int shell_func_list_size;                          /**< Number of registered shell functions */
    shell_func_t shell_func_list[SHELL_FUNC_LIST_MAX_SIZE]; /**< List of shell functions */
    uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
    uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
    char	 	cmdBuffer[CMD_BUFFER_SIZE];
    uint8_t uartRxReceived;
    int idx_cmd;
} h_shell_t;

void Shell_Init( h_shell_t *h_shell);
void Shell_Loop(h_shell_t *h_shell);


#endif /* INC_MYLIBS_SHELL_H_ */
