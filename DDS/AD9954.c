#include "AD9954.h"
#include "delay.h"

//ϵͳƵ��fosc���ⲿ����Ƶ�ʣ���ϵͳƵ��=fosc*M
#define fosc  25                        //����Ƶ��
#define PLL_MULTIPLIER  16              //PLL��Ƶ����4--20��
#define fs  (fosc*PLL_MULTIPLIER)       //ϵͳʱ��Ƶ��
#define congji 1
//double fH_Num=11.2204;
double fH_Num=10.73741824;
//double fH_Num=11.3671588397205;//
//double fH_Num = 11.3025455157895;
//double fH_Num=17.17986918;
/*
** �������� ��void GPIO_AD9954_Init(void)
** �������� ��AD9954�ӿ�IO��ʼ��
**            AD9954_CS----------PD10	 		OUT
**	      		AD9954_SCLK--------PD9	 		OUT
**            AD9954_SDIO--------PD8	 	  OUT
**            AD9954_OSK---------PD15  	 	OUT
**          	PS0----------------PB14			OUT
**	      		PS1----------------PD13	 		OUT
**            IOUPDATE-----------PB12	  	OUT

**	      		AD9954_SDO---------PD11	  	IN
**            AD9954_IOSY--------PD12	  	OUT
**            AD9954_RES---------PD13    	OUT
**            AD9954_PWR---------PD14    	OUT
*/
void GPIO_AD9954_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;

	/** Configure pins as GPIO
		 PC5	 ------> GPIO_Output
		 PB0	 ------> GPIO_Output
		 PB1	 ------> GPIO_Output
		 PB2	 ------> GPIO_Output
		 PF11	 ------> GPIO_Output
		 PF12	 ------> GPIO_Output
		 PF13	 ------> GPIO_Output
		 PF14	 ------> GPIO_Input
		 PF15	 ------> GPIO_Output
		 PG0	 ------> GPIO_Output
		 PG1	 ------> GPIO_Output
	*/
	/*Enable or disable the AHB1 peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);

	AD9954_IOSY=0;
	AD9954_OSK=0;
	AD9954_PWR=0;
}

/*********************************************************************************************************
** �������� ��void AD9954_RESET(void)
** �������� ����λAD9954
** ����˵�� ������λҲ����
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/
void AD9954_RESET(void)
{
	AD9954_CS = 0;
	AD9954_RES = 0;
	AD9954_RES = 1;
	delay_ms(100);
	AD9954_RES = 0;
	AD9954_CS = 0;
	AD9954_SCLK = 0;
	PS0 = 0;
	PS1 = 0;
	IOUPDATE = 0;
	AD9954_CS = 1;
}

/*********************************************************************************************************
** �������� ��void AD9954_SENDBYTE(u8 dat)
** �������� ����AD9954����һ���ֽڵ�����
** ����˵�� ��AD9954�Ĵ����ٶ����Ϊ25M�����Բ�����ʱҲ����
** ��ڲ��� ���������ֽ�
** ���ڲ��� ����
*********************************************************************************************************/
void AD9954_SENDBYTE(u8 dat)
{
	u8 i;
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		if (dat & 0x80)
		{
			AD9954_SDIO = 1;
			
		}
		else
		{
			AD9954_SDIO = 0;
		}
		__nop();
		
		AD9954_SCLK = 1;
		__nop();
		dat <<= 1;
	}
}

u8 AD9954_ReadByte(void)
{
	u8 i,dat=0;
	
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		dat|=AD9954_SDO;
		AD9954_SCLK = 1;
		dat <<= 1;
	}
	
	return dat;
}

/*********************************************************************************************************
** �������� ��void UPDATE(void)
** �������� ������һ�������źţ�����AD9954�ڲ��Ĵ�����
** ����˵�� �����Բ����κ���ʱ
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/
void UPDATE(void)
{
	IOUPDATE=0;
//	delay_us(100); 
	IOUPDATE = 1;
	delay_us(200);
	IOUPDATE = 0;
}

void UPDATE_SYNC(void)
{
		IOUPDATE=0;
//	delay_us(100); 
		IOUPDATE = 1;
		delay_us(200);
		IOUPDATE = 0;
}

//���Ĵ���
u32 Read_Vau1(void)
{
    uint32_t ret;
//    char i;
    AD9954_CS=1;
    AD9954_SENDBYTE(CFR1);
	
    ret = AD9954_ReadByte();
    ret<<=8;
    ret += AD9954_ReadByte();
    ret<<=8;
    ret += AD9954_ReadByte();
    ret<<=8;
    ret += AD9954_ReadByte();
    AD9954_CS=0;
	
    return ret;
}

u32 Read_Vau2(void)
{
    uint32_t ret;
//    char i;
    AD9954_CS=1;
    AD9954_SENDBYTE(CFR2);
    ret = AD9954_ReadByte();
    ret<<=8;
    ret += AD9954_ReadByte();
    ret<<=8;
    ret += AD9954_ReadByte();
    AD9954_CS=0;
    return ret;
}

/*********************************************************************************************************
** �������� ��void AD9954_Init(void))
** �������� ����ʼ��AD9954�Ĺܽź���򵥵��ڲ��Ĵ��������ã�
** ����˵�� ��Ĭ�Ϲص��Ƚ����ĵ�Դ������ϵľ���Ϊ100MHz����������4��Ƶ��Ϊ400M
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/

void AD9954_Init(void)
{
	GPIO_AD9954_Init();
	AD9954_RESET();
	delay_ms(300);
	
	AD9954_CS = 0;
	//single tone
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x02);//
	AD9954_SENDBYTE(0x10);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//�Ƚ���power down	  
//	AD9954_SENDBYTE(0x00);//�Ƚ���ʹ��		

	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x08);
#if fs>400
    #error "ϵͳƵ�ʳ���оƬ���ֵ"
#elseif fs>=250
    AD9954_SENDBYTE(PLL_MULTIPLIER<<3|0x04|0X03);
#else
    AD9954_SENDBYTE(PLL_MULTIPLIER<<3);
#endif
//	AD9954_SENDBYTE(0x24);//4��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M
	AD9954_CS=1;
	
	
	Write_ASF(0x150);//����������ú������������Ϊ0-3FFF����Ӧ�������0-480mv	
}

u32 Get_FTW(double Real_fH)
{
		return (u32)(fH_Num*Real_fH);
}

/*********************************************************************************************************
** �������� ��void AD9954_SETFRE(float f)
** �������� ������AD9954��ǰ��Ƶ����������õ��ǵ�һƵ�����
** ����˵�� ����Ϊ���õĸ��������м��㣬ת�������л������ͨ���������Ծ�ȷ��0.1Hz����
** ��ڲ��� �������õ�Ƶ��ֵ
** ���ڲ��� ����
*********************************************************************************************************/
void AD9954_SETFRE(double f)//single tone
{
	u32 date;
	AD9954_CS = 0;
	
	date = Get_FTW(f);//date=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(FTW0);//FTW0��ַ
	AD9954_SENDBYTE((u8)(date >> 24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)(date >> 16));
	AD9954_SENDBYTE((u8)(date >> 8));
	AD9954_SENDBYTE((u8)date);
	AD9954_CS=1;
	UPDATE();
//	fH_Num+=0.001;
}

void DirectSwitchPSK(float f, float phase1, float phase2, float phase3, float phase4)
{
	u16 date;
	AD9954_CS = 0;
	IOUPDATE = 0;
	delay_us(1);
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(0x01);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0xA4);//4��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M
	AD9954_SETFRE(f);//�ز�Ƶ��
	PS0 = 0;
	PS1 = 0;
	AD9954_SENDBYTE(0x07);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0������ģʽ0,��ͣ��λû�м���
	UPDATE();
	date = 45.51*phase1;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	PS0 = 1;//ram1
	PS1 = 0;

	AD9954_SENDBYTE(0x08);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);//final address:0x0001
	AD9954_SENDBYTE(0x04);//start address:0x0001
	AD9954_SENDBYTE(0x00);//RAM1������ģʽ0,��ͣ��λû�м���
	UPDATE();
	date = 45.51*phase2;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	PS0 = 0;//ram2
	PS1 = 1;

	AD9954_SENDBYTE(0x09);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x02);//final address:0x0002
	AD9954_SENDBYTE(0x08);//start address:0x0002
	AD9954_SENDBYTE(0x00);
	UPDATE();
	date = 45.51*phase3;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	PS0 = 1;//ram3
	PS1 = 1;
	AD9954_SENDBYTE(0x0a);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x03);//final address:0x0003
	AD9954_SENDBYTE(0x0c);//start address:0x0003
	AD9954_SENDBYTE(0x00);
	UPDATE();
	date = 45.51*phase4;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0xa0);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** �������� ��void Generate_PSK(float f,u16 *phase)
** �������� ��PSK�����ź����
** ����˵�� ��ͨ��RAM���������źŵ���λ���Ӷ�ʵ��PSK�źŵĵ���
**            ����ʹ��һ��RAM�ε�ѭ��ģʽ��PSK�ź�Ϊ8Ϊ����λ��
**            RAM�ĸ����ٶ���ram rate��ʱ�Ĵ������ƣ��Ĵ���Ϊ16λ��ʵ�ֶ�ϵͳ�ṩ��100Mʱ��1~65535��Ƶ��д0����Ч��
**			  �������PSK�ز���Ƶ����ram rate����Ƶ�ʵ�������ʱ�����ܱ�֤ÿ�ε���λ�ı�һ��
** ��ڲ��� ��float f   ��PSK�ز���Ƶ��
** 			  u16 *phase: д����λ�ۼ�������λֵ��180ʵ�ֶ���λ��ת��0���ı���λ��PSK�ź�Ϊ8λ����Ϊ8λ������
** ���ڲ��� ����
*********************************************************************************************************/
void Generate_PSK(float f, u16 *phase)
{
	u8 i;
	u16 date;
	AD9954_CS = 0;
	IOUPDATE = 0;

	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(0x01);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0xA4);//4��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M
	AD9954_SETFRE(f);//�ز�Ƶ��

	AD9954_SENDBYTE(0x04);//FTW0��ַ
	AD9954_SENDBYTE(0x00);//Ƶ�ʿ�����
	AD9954_SENDBYTE(0x10);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	UPDATE();

	PS0 = 0;
	PS1 = 0;
	AD9954_SENDBYTE(0x07);
	AD9954_SENDBYTE(0x00);//ramp rate=0x0400
	AD9954_SENDBYTE(0x04);
	AD9954_SENDBYTE(0x07);//final address:0x007
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0������ģʽ4,��ͣ��λû�м���
	UPDATE();

	AD9954_SENDBYTE(0x0b);
	for (i = 0;i<8;i++)
	{
		date = 45.51*phase[i];
		date = date << 2;
		AD9954_SENDBYTE((u8)(date >> 8));//Ƶ�ʿ�����
		AD9954_SENDBYTE((u8)date);
		AD9954_SENDBYTE(0x00);
		AD9954_SENDBYTE(0x00);
	}
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0xc0);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** �������� ��void Generate_FM(u32 *fre)
** �������� ��FM�����ź����
** ����˵�� ��ͨ��RAM���������źŵ�Ƶ�ʣ��Ӷ�ʵ��FM�źŵĵ���
**            ����ʹ��һ��RAM�ε�ѭ��ģʽ��PSK�ź�Ϊ8Ϊ����λ��
**            RAM�ĸ����ٶ���ram rate��ʱ�Ĵ������ƣ��Ĵ���Ϊ16λ��ʵ�ֶ�ϵͳ�ṩ��100Mʱ��1~65535��Ƶ��д0����Ч��
**			  ram rate�ĸ����ٶ�Ϊ�����źŵ�Ƶ�ʣ�������1KHz��д��ram rate��ֵΪ0X0C35,
** ��ڲ��� ��u32 *fre��FM�ź�Ƶ��ֵ��������õ���32��Ƶ�ʲ��������ô˺���֮ǰ�������ú�Ƶ�ʱ��ֵ���䰴���ҹ��ɸı�
**            for(i=0;i<32;i++)
**				{
**					fre[i]=Fc+Fshift*sin(wt);
**				}
** ���ڲ��� ����
*********************************************************************************************************/
void Generate_FM(u32 *fre)
{
	u8 i;
	u32 date;
	AD9954_CS = 0;
	IOUPDATE = 0;

	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);

	AD9954_SENDBYTE(0x01);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x24);//4��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M

	PS0 = 0;
	PS1 = 0;
	AD9954_SENDBYTE(0x07);
	AD9954_SENDBYTE(0x35);//ramp rate=32kHz
	AD9954_SENDBYTE(0x0C);
	AD9954_SENDBYTE(0x1F);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0������ģʽ0,��ͣ��λû�м���
	UPDATE();

	AD9954_SENDBYTE(0x0b);
	for (i = 0;i<32;i++)
	{
		date = 10.73741*fre[i];
		AD9954_SENDBYTE((u8)(date >> 24));//Ƶ�ʿ�����
		AD9954_SENDBYTE((u8)(date >> 16));
		AD9954_SENDBYTE((u8)(date >> 8));
		AD9954_SENDBYTE((u8)date);
	}
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x80);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** �������� ��void ASK_Init(void)
** �������� ��ASK�����ź������ʼ��
** ����˵�� ��ASK���ƣ���Ҫ�����ú��ز�Ƶ�ʣ�Ȼ��ı�DAC�������� scale factor��Ϊ14λ��ͨ������Ϊ����0����ֵʵ��ASK�źŵĵ���
**			  ����Write_ASF(u16 factor)���ı��ֵ
** ��ڲ��� ����
** ���ڲ��� ����
****************************************************** ***************************************************/
void ASK_Init(void)
{
	AD9954_CS = 0;
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//���ܵ�UPDATE�źţ��Զ������λ�ۼ���		//
						  //	AD9954_SENDBYTE(0x00);//���ı��κ�

	AD9954_SENDBYTE(0x40);

	//������λƫ�ƣ���Ĭ����������Ҳ��������������λ�ۼ������ۼ���λΪ270
	AD9954_SENDBYTE(0X05);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);

	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** �������� ��void AM_Init(void)
** �������� ��AM�����ź������ʼ��
** ����˵�� ��AM���ƣ���Ҫ�����ú��ز�Ƶ�ʣ�Ȼ��ı�DAC�������� scale factor��Ϊ14λ,AM�ķ���ֵ�����ҹ��ɱ任������ʵ��AM
**            ��߿��Բ���32�����
**			  ����Write_ASF(u16 factor)���ı��ֵ
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/
void AM_Init(void)
{
	AD9954_CS = 0;
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//���ı��κ�
	AD9954_SENDBYTE(0x40);

	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** �������� ��void Write_ASF(u16 factor)
** �������� ���ı�scale factor��ֵ���ı�DAC�������
** ����˵�� ��д�����Ϊ0X3FFF,��СΪ0
** ��ڲ��� ����
** ���ڲ��� ����
*********************************************************************************************************/
void Write_ASF(u16 factor)  //2ASK
{
	AD9954_CS = 0;
	/*	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//���ܵ�UPDATE�źţ��Զ������λ�ۼ���		//
	//	AD9954_SENDBYTE(0x00);//���ı��κ�

	AD9954_SENDBYTE(0x40);

	//������λƫ��
	AD9954_SENDBYTE(0X05);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);	  */

	AD9954_SENDBYTE(0x02);//����	
	AD9954_SENDBYTE(factor >> 8);
	AD9954_SENDBYTE(factor);
	AD9954_CS = 1;
	UPDATE();

}
/******************************************************************
*  ��    �ƣ�Linear_Sweep
*  ��    �ܣ�AD9954ɨƵģʽ���
*  ��    ����MinFreq��ɨƵ����Ƶ�ʣ�MaxFreq��ɨƵ����Ƶ��
*            UpStepFreq������ɨƵ������UpStepTime������ɨƵ����Ƶʱ��
*            DownStepFreq������ɨƵ������DownStepTime������ɨƵ����Ƶʱ��
*            Mode��ɨƵģʽ 
*  �� �� ֵ���� Linear_Sweep(100, 100000000, 100,  250,   100,250, 1);
*  ˵    ������Ƶ����صĲ����ĵ�λ���� MHz
*            Mode=DoubleScan(˫��ɨƵ)ʱ��PS0=1����Ϊ����ɨƵ,��������
*            ����ɨƵ�ļĴ���,����ֱ��ͨ��PS0_WriteBit()���ı�ɨƵ����
******************************************************************/
void Linear_Sweep( double  MinFreq, double MaxFreq, double  UpStepFreq, u8 UpStepTime,double  DownStepFreq, u8 DownStepTime,ScanMode Mode )       //0����ɨƵģʽ��1����ɨƵģʽ��2˫��ɨƵ

{
    uint32_t FTW_Vau;
    PS0=0;
    PS1=0;
    AD9954_CS=0;
    AD9954_SENDBYTE(CFR1);
    AD9954_SENDBYTE(0x02);
    AD9954_SENDBYTE(0x20);     //Linear_Sweep Enable
    AD9954_SENDBYTE(0);
    if (Mode & No_Dwell)
        AD9954_SENDBYTE(0x44);     //Comp Power-Down & Linear Sweep No Dwell
    else
        AD9954_SENDBYTE(0x40);     //Comp Power-Down
    
    //д��FTW0----��СƵ��
    FTW_Vau =  Get_FTW(MinFreq);;
    AD9954_SENDBYTE(FTW0);
    AD9954_SENDBYTE(FTW_Vau>>24);
    AD9954_SENDBYTE(FTW_Vau>>16);
    AD9954_SENDBYTE(FTW_Vau>>8);
    AD9954_SENDBYTE(FTW_Vau);
    
    //д��FTW1----���Ƶ��
    FTW_Vau = Get_FTW(MaxFreq);
    AD9954_SENDBYTE(FTW1);
    AD9954_SENDBYTE(FTW_Vau>>24);
    AD9954_SENDBYTE(FTW_Vau>>16);
    AD9954_SENDBYTE(FTW_Vau>>8);
    AD9954_SENDBYTE(FTW_Vau);
    
    Mode &= 0x7F;
    
    if(Mode != UpScan)
    {
        //д��NLSCW----�½�Ƶ�ʲ�������Ƶʱ��
        FTW_Vau = Get_FTW(DownStepFreq);
        AD9954_SENDBYTE(NLSCW);
        AD9954_SENDBYTE(DownStepTime); //��Ƶʱ�䣨DownStepTime���������ڣ�     
        AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
        AD9954_SENDBYTE(FTW_Vau>>16);
        AD9954_SENDBYTE(FTW_Vau>>8);
        AD9954_SENDBYTE(FTW_Vau);
        PS0=0;                         //�½�ɨƵ
    }
    if (Mode != DownScan)
    {
        //д��PLSCW----����Ƶ�ʲ�������Ƶʱ��
        FTW_Vau = Get_FTW(UpStepFreq);
        AD9954_SENDBYTE(PLSCW);
        AD9954_SENDBYTE(UpStepTime); //��Ƶʱ�䣨0XFF���������ڣ�
        AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
        AD9954_SENDBYTE(FTW_Vau>>16);
        AD9954_SENDBYTE(FTW_Vau>>8);
        AD9954_SENDBYTE(FTW_Vau);
        PS0=1;                         //����ɨƵ
    }
    AD9954_CS=1;
		UPDATE();
}

/******************************************************************
*  ��    �ƣ�PS0_WriteBit
*  ��    �ܣ�����PS0��״̬
*  ��    ����BitVal��PS0״̬
*  �� �� ֵ����
*  ˵    ����PS0���Կ���AD9954ɨƵ����
******************************************************************/
void PS0_WriteBit(BitAction BitVal)
{
    PS0=BitVal;
}

/*********************************************************************************************************
** �������� ��void DirectSwitchFSK(float f1,float f2,float f3,float f4)
** �������� ������FSK�ź����
** ����˵�� �����ĸ�RAM����������һ��Ƶ��ֵ��ͨ���ı�PS0��PS1�ĵ�ƽѡ���Ӧ��RAM�������Ӧ��Ƶ��ֵ��ʵ��FSK��Ҳ����ʵ�ֶ����FSK��
**            ͨ�����ö�ʱ���жϿ���PS0,PS1�ܽŵĵ�ƽ�Ϳ��Խ������Ƶı���ת��ΪFSK�ź����
** ��ڲ��� ��float f1:Ƶ��1
** 	      float f2:Ƶ��2
** 	      float f3:Ƶ��3
** 	      float f4:Ƶ��4
** ��������   			PS0: 0   1	0	 1
** �ܽŲ����� 			PS1: 0   0	1	 1
** ��Ӧ���� RAM�Σ�		 0   1	2	 3
** ���ڲ��� ����
*********************************************************************************************************/
void DirectSwitchFSK(double f1,double f2,double f3,double f4)   
{
	u32 FTW_Vau;
	AD9954_CS = 0;

	AD9954_SENDBYTE(0x00);//��ַ0д����   
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x40);   

	AD9954_SENDBYTE(0x01);//��ַ1д����   
	AD9954_SENDBYTE(0x00);//   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0xA4);//8��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M   
	PS0=0;   
	PS1=0;   
	AD9954_SENDBYTE(0x07);   
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);//final address:0x000   
	AD9954_SENDBYTE(0x00);//start address:0x000;   
	AD9954_SENDBYTE(0x00);//RAM0������ģʽ0,��ͣ��λû�м���   
	UPDATE();   
	FTW_Vau=Get_FTW(f1);
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
	AD9954_SENDBYTE(FTW_Vau>>16);
	AD9954_SENDBYTE(FTW_Vau>>8);
	AD9954_SENDBYTE(FTW_Vau);

	PS0=1;//ram1   
	PS1=0;   

	AD9954_SENDBYTE(0x08);   
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x01);//final address:0x0001   
	AD9954_SENDBYTE(0x04);//start address:0x0001   
	AD9954_SENDBYTE(0x00);//RAM1������ģʽ0,��ͣ��λû�м���   
	UPDATE();
	FTW_Vau=Get_FTW(f2);   
	AD9954_SENDBYTE(0x0b);   
	AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
	AD9954_SENDBYTE(FTW_Vau>>16);
	AD9954_SENDBYTE(FTW_Vau>>8);
	AD9954_SENDBYTE(FTW_Vau);  

	PS0=0;//ram2   
	PS1=1;   

	AD9954_SENDBYTE(0x09);   
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x02);//final address:0x0002   
	AD9954_SENDBYTE(0x08);//start address:0x0002   
	AD9954_SENDBYTE(0x00);   
	UPDATE();   
	FTW_Vau=Get_FTW(f3);   
	AD9954_SENDBYTE(0x0b);   
	AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
	AD9954_SENDBYTE(FTW_Vau>>16);
	AD9954_SENDBYTE(FTW_Vau>>8);
	AD9954_SENDBYTE(FTW_Vau); 
	PS0=1;//ram3   
	PS1=1;   
	AD9954_SENDBYTE(0x0a);   
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x03);//final address:0x0003   
	AD9954_SENDBYTE(0x0c);//start address:0x0003   
	AD9954_SENDBYTE(0x00);     
	UPDATE();   
	FTW_Vau=Get_FTW(f4);   
	AD9954_SENDBYTE(0x0b);   
	AD9954_SENDBYTE(FTW_Vau>>24);  //Ƶ�ʲ���
	AD9954_SENDBYTE(FTW_Vau>>16);
	AD9954_SENDBYTE(FTW_Vau>>8);
	AD9954_SENDBYTE(FTW_Vau); 

	AD9954_SENDBYTE(0x00);//��ַ0д����   
	AD9954_SENDBYTE(0x80);//��RAM����λ����FTW   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x40);   
	AD9954_CS = 1;
	UPDATE();
}   
/*********************************************************************************************************
** �������� ��void LINEARSWEEP(float f1,float f2)
** �������� ������ɨ�����ģʽ
** ����˵�� ��ʹƵ�ʰ�Ԥ�õ�ģʽ����ɨ����ȥ����ϸ�μ��ٷ�PDF
** ��ڲ��� ��float f1:��ʼƵ��
** 			  float f2:��ֹƵ��
** ���ڲ��� ����
*********************************************************************************************************/
void LINEARSWEEP(float f1,float f2)//linear sweep mode
{
	u32 date;
	AD9954_SCLK=0;
	AD9954_RES=0;
	IOUPDATE=0;
	PS0=0;
	PS1=0;
	AD9954_CS=0;

	//linear sweep mode
	AD9954_SENDBYTE(0x00);//��ַ0д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x20);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	

	AD9954_SENDBYTE(0x01);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x24);//4��Ƶ����VCO���Ƹ�λ��ϵͳʱ�ӱ�Ƶ��Ϊ400M
AD9954_SENDBYTE(PLL_MULTIPLIER<<3|0x04);
	date=10.7374*f1;
	AD9954_SENDBYTE(0x04);//FTW0��ַ
	AD9954_SENDBYTE((u8)(date>>24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)(date>>16));
	AD9954_SENDBYTE((u8)(date>>8));
	AD9954_SENDBYTE((u8)date);
	date=10.7374*f2;
	AD9954_SENDBYTE(0x06);//FTW1��ַ
	AD9954_SENDBYTE((u8)(date>>24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((u8)(date>>16));
	AD9954_SENDBYTE((u8)(date>>8));
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x07);//NLSCW
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x1b);
	AD9954_SENDBYTE(0xf4);
	AD9954_SENDBYTE(0x08);//RLSCW
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0xa3);
	AD9954_SENDBYTE(0x6f);
	
	AD9954_CS=1;
	UPDATE();
}
