#include "AD9954.h"
#include "delay.h"
#include "Cont_DDS.h"
#include "usart6.h"

u8 StopF[5]={'S','T','O','P','F'};
u8 StartF[5]={'S','T','A','R','F'};
u8 Fre[40];

void Step_scan()
{
	u8 Freq=1,i=1;
	u8 j=0;
	
/***********点频模式*************/
//	AD9954_SETFRE(10000000);

/***********扫频模式*************/	
	for(i=1;i<=40;i++)
	{
		Send_data(StartF,5);
		delay_ms(10);
		AD9954_SETFRE(i*1000000);
		Fre[i]=i*2;
		Send_data(&Fre[i],1);
			
		Send_data(StopF,5);
		delay_ms(20);
	}
}

void Atuo_scan()
{
	Linear_Sweep( 1000000, 8, 1000000,  250, 1000000,250, 1 );
}


