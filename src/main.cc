/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"
#include "hx_drv_tflm.h"
#include "stdio.h"
#include "delay.h"
#include "swuart.h"
#include "imu_lsm9ds1.h"

//#include "synopsys_wei_gpio.h"

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
// int signal_pass=0;
// volatile void delay_ms(unsigned int ms);
// volatile void delay_us(unsigned int ms);

char string_buf[100] = "test\n";

#define accel_scale 1000
#define sample_data 300
#define sample_data_nim_of_bytes 17

void acc_decode_to_str(accel_type *type, float data);

int main(int argc, char* argv[]) {
    int int_buf;

    char string_buf[100];

    hx_drv_uart_initial(UART_BR_115200);              // uart 初始化
    hx_drv_uart_print("URAT_GET_STRING_START\n");

    hx_drv_gpio_config_t hal_gpio_ZERO;               //gio 初始化
	hx_drv_gpio_config_t hal_gpio_ONE;

    accel_type accel_x, accel_y, accel_z;
    accel_type acc_gyro_x, acc_gyro_y, acc_gyro_z;
	float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z;

    hal_gpio_ZERO.gpio_pin = HX_DRV_PGPIO_0;  // 決定pin角要用數字
    hal_gpio_ZERO.gpio_direction = HX_DRV_GPIO_OUTPUT;  //HX_DRV_GPIO_OUTPUT(3)
    hal_gpio_ZERO.gpio_data = 1;

    hal_gpio_ONE.gpio_pin = HX_DRV_PGPIO_1;  // 決定pin角要用數字
    hal_gpio_ONE.gpio_direction = HX_DRV_GPIO_INPUT;  //HX_DRV_GPIO_OUTPUT(3)
    hal_gpio_ONE.gpio_data = 0;

    swuart_init(&hal_gpio_ZERO);  //將gpio 使用 swuart 初始化(swuart 裡面其實就只是gpio的初始化罷了)
    swuart_init(&hal_gpio_ONE);

    imu_initial();  // 使用客製化 imu 初始化。
    setup();
    uint8_t temp_a[sample_data_nim_of_bytes];
    uint16_t counter = 0;
    int signal_pass;
    uint8_t label;
    float x_data2[3][sample_data];
    uint8_t in[3];
    uint8_t num = 3;
    
    GPIO_UART_RX(in,&counter,&hal_gpio_ONE);

    while (true) {  
		imu_receive(&acc_x, &acc_y, &acc_z);
        acc_x = (acc_x +2 ) / 4;
        acc_y = (acc_y +2 ) / 4;
        acc_z = (acc_z +2 ) / 4;
        // imu_gyro_receive(&gyro_x, &gyro_y, &gyro_z);
        acc_decode_to_str(&accel_x,acc_x);
        acc_decode_to_str(&accel_y,acc_y);
        acc_decode_to_str(&accel_z,acc_z);
        // acc_decode_to_str(&acc_gyro_x,gyro_x);
        // acc_decode_to_str(&acc_gyro_y,gyro_y);
        // acc_decode_to_str(&acc_gyro_z,gyro_z);

        print_imu(accel_x,accel_y,accel_z,counter);

        x_data2[0][counter] = acc_x; // acc_x is (-2~2)*10 => (-20~20)
		x_data2[1][counter] = acc_y;
		x_data2[2][counter] = acc_z;

        temp_a[0] = 0 ; 
		temp_a[1] = counter >> 8;     //高8位
		temp_a[2] = counter & 0xff ;  //低8位
		temp_a[3] = accel_x.symbol == '+';
		temp_a[4] = accel_x.int_part;
		temp_a[5] = accel_x.frac_part & 0XFF;
		temp_a[6] = accel_x.frac_part >> 8;
		temp_a[7] = accel_y.symbol == '+';
		temp_a[8] = accel_y.int_part ;
		temp_a[9] = accel_y.frac_part & 0XFF;
		temp_a[10] = accel_y.frac_part >> 8;
		temp_a[11] = accel_z.symbol == '+';
		temp_a[12] = accel_z.int_part; 
		temp_a[13] = accel_z.frac_part & 0XFF; 
		temp_a[14] = accel_z.frac_part >> 8; 
		temp_a[15] = 0xff; 
		temp_a[16] = 0xff;
        if (counter == 0){
			temp_a[1] = 0x00;
			temp_a[2] = 0x00;
		}
        uint8_t sum = 0;
		for(int k=1;k<15;k++){
			sum += temp_a[k];
		}
        temp_a[0] = sum;

        GPIO_UART_TX(temp_a,sample_data_nim_of_bytes,&hal_gpio_ZERO);
        for(int k=0;k<sample_data_nim_of_bytes;k++){
			temp_a[k] = 0;   
		}
        
        /*start to recognize*/
        counter ++;
        if (counter >= 301){
            label = 0;
            counter = 0;
            // GPIO_UART_RX(in,&counter,&hal_gpio_ONE);
			loop_har(x_data2,&signal_pass);
			label = signal_pass;
			GPIO_UART_TX(&label,1,&hal_gpio_ZERO);
			delay_ms(100);
			counter=0;
            for (int i=0;i<16;i++){
                temp_a[i] = 0;
            }
            for(int i=0;i<301;i++){
                x_data2[0][counter] = 0; 
                x_data2[1][counter] = 0;
                x_data2[2][counter] = 0;
            }
		}
        // delay_us(500);
    }
}

void acc_decode_to_str(accel_type *type, float data)
{
    int int_buf = data * accel_scale;
    if (data < 0 ){
        int_buf = int_buf * -1;
        type->symbol = '-';
    }else{
        type->symbol = '+';
    }
    type->int_part = int_buf/ accel_scale;
    type->frac_part = int_buf % accel_scale;
}
