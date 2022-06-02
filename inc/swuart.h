#ifndef __SWUSART_H
#define __SWUSART_H

#include "stdio.h"
#include "hx_drv_tflm.h"


void swuart_init(hx_drv_gpio_config_t *GPIO_Initure);
void GPIO_UART_TX(uint8_t *data, uint32_t len , hx_drv_gpio_config_t *Tx);
void GPIO_UART_RX(uint8_t *data, uint16_t *len, hx_drv_gpio_config_t * Rx);


#endif