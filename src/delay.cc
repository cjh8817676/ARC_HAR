#include "hx_drv_tflm.h"
#include "stdio.h"
#include "delay.h"

volatile void delay_ms(unsigned int ms)
{
  volatile unsigned int delay_i = 0;
  volatile unsigned int delay_j = 0;
  volatile const unsigned int delay_cnt = 20000;

  for(delay_i = 0; delay_i < ms; delay_i ++)
    for(delay_j = 0; delay_j < delay_cnt; delay_j ++);
} 
volatile void delay_us(unsigned int us)
{
  uint32_t start = 0;
  uint32_t end = 0;
  uint32_t diff = 0;
  hx_drv_tick_get(&start);	

  do{
	hx_drv_tick_get(&end);	
	if(end < start){
		diff = 0XFFFFFFFF - start + end;
	}else{
		diff = (end - start);
	}
  }while( diff < (us * 400) );
} 