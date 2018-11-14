#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "usart6.h"
#include "adc.h"
#include "AD9954.h"
#include "Cont_DDS.h"

u8 Start[5]={'S','T','A','R','T'};
u8 Stop[5]={'S','T','O','P','T'};

int main(void)
{
	u8 i;
	
	uart_init(115200);
	delay_init(168);
	usart6_Init();
	AD9954_Init();
	Adc_Init();
	delay_ms(100);
	
	printf("Begin!\r\n");
//	length=sizeof(Start);
	
	USART_ClearFlag(USART6,USART_FLAG_TC); 
//	AD9954_SETFRE(10000000);
	
	while(1)
	{
	
		Send_data(Start,5);
		delay_ms(50);
		Send_data(Start,5);
		delay_ms(50);
		Step_scan();	
		Send_data(Stop,5);		
		chang_vlaue();
		delay_ms(50);	
	}
	
//	Step_scan();	
//	
//	while(1);

}



