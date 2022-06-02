#include "swuart.h"	
#include "delay.h"
#include "hx_drv_tflm.h"



void swuart_init(hx_drv_gpio_config_t *GPIO_Initure) // gpio 模擬 uart，所以要對gpio初始化。
{
    if(hx_drv_gpio_initial(GPIO_Initure) != HX_DRV_LIB_PASS)
    	hx_drv_uart_print("gpio init fail\n");
  	else
    	hx_drv_uart_print("gpio init success\n");

}

void GPIO_UART_TX(uint8_t *data, uint32_t len , hx_drv_gpio_config_t *Tx) //gpio0 為板子的Tx
{
	Tx -> gpio_data = 0;
    hx_drv_gpio_set(Tx);
	for ( int i = 0; i < len; i++ )
    {
        uint8_t c = data[i];
        
        // pull down as start bit
		Tx -> gpio_data = 0;
        hx_drv_gpio_set(Tx);
        delay_us(26);  // baudrate默認 38400 (26)
        for ( int j = 0; j < 8; j++ )
        {
            if ( c & 0x01 )
            {
				Tx->gpio_data = 1;
                hx_drv_gpio_set(Tx);  //GPIO_PIN_SET = 1
            }
            else
            {
				Tx->gpio_data = 0;
                hx_drv_gpio_set(Tx);
            }
            delay_us(26);
            c >>= 1;
        }
        // pull high as stop bit
		Tx->gpio_data = 1;
        hx_drv_gpio_set(Tx);
        delay_us(26);
    }
}
void GPIO_UART_RX(uint8_t *data, uint16_t *len, hx_drv_gpio_config_t * Rx) //gpio1 為板子的Rx
{
	char string_buf[100];
	sprintf(string_buf,"receive\n");
	hx_drv_uart_print(string_buf);
	int count = 0; 
	while(1)
    {
        uint8_t b[8] = { 0 };
        uint32_t c = 0;
        // wait for start bit
		do{
			hx_drv_gpio_get(Rx);
		}while ( Rx->gpio_data == 1 );  // wait while gpio read high
        delay_us(26/2);
		//delay_us(26);
        for ( int j = 0; j < 8; j++ )
        {
            delay_us(26);
			hx_drv_gpio_get(Rx);
            b[j] = ( Rx->gpio_data) ? 1 : 0;
        }
        
        for ( int j = 0; j < 8; j++ )
        {
            c |= (b[j] << j);
        }
        data[count & 0x3fff] = c;

		count++;
		if(c == 0x0d){   // 0x0d'\r'
				
			count |= 0x8000;
		}else if((c == 0x0a) && (count & 0x8000) ){ //0x0a '\n'
			count |= 0x4000;
		}
		if( count & 0x4000){
			*len = (count & 0x3fff) - 2;
			data[*len] = '\0';
			break;
		}
        // wait for stop bit
		do{
			hx_drv_gpio_get(Rx);
		}
        while ( Rx->gpio_data == 0 );  //wait until gpio read high as stop bit
    }
}