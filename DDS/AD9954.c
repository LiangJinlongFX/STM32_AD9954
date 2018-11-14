#include "AD9954.h"
#include "delay.h"

//系统频率fosc（外部晶振频率），系统频率=fosc*M
#define fosc  25                        //晶振频率
#define PLL_MULTIPLIER  16              //PLL倍频数（4--20）
#define fs  (fosc*PLL_MULTIPLIER)       //系统时钟频率
#define congji 1
//double fH_Num=11.2204;
double fH_Num=10.73741824;
//double fH_Num=11.3671588397205;//
//double fH_Num = 11.3025455157895;
//double fH_Num=17.17986918;
/*
** 函数名称 ：void GPIO_AD9954_Init(void)
** 函数功能 ：AD9954接口IO初始化
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
** 函数名称 ：void AD9954_RESET(void)
** 函数功能 ：复位AD9954
** 函数说明 ：不复位也可以
** 入口参数 ：无
** 出口参数 ：无
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
** 函数名称 ：void AD9954_SENDBYTE(u8 dat)
** 函数功能 ：往AD9954发送一个字节的内容
** 函数说明 ：AD9954的传输速度最大为25M，所以不加延时也可以
** 入口参数 ：待发送字节
** 出口参数 ：无
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
** 函数名称 ：void UPDATE(void)
** 函数功能 ：产生一个更新信号，更新AD9954内部寄存器，
** 函数说明 ：可以不加任何延时
** 入口参数 ：无
** 出口参数 ：无
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

//读寄存器
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
** 函数名称 ：void AD9954_Init(void))
** 函数功能 ：初始化AD9954的管脚和最简单的内部寄存器的配置，
** 函数说明 ：默认关掉比较器的电源，因板上的晶振为100MHz，最大采用了4倍频，为400M
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/

void AD9954_Init(void)
{
	GPIO_AD9954_Init();
	AD9954_RESET();
	delay_ms(300);
	
	AD9954_CS = 0;
	//single tone
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x02);//
	AD9954_SENDBYTE(0x10);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//比较器power down	  
//	AD9954_SENDBYTE(0x00);//比较器使能		

	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x08);
#if fs>400
    #error "系统频率超过芯片最大值"
#elseif fs>=250
    AD9954_SENDBYTE(PLL_MULTIPLIER<<3|0x04|0X03);
#else
    AD9954_SENDBYTE(PLL_MULTIPLIER<<3);
#endif
//	AD9954_SENDBYTE(0x24);//4倍频，打开VCO控制高位，系统时钟倍频后为400M
	AD9954_CS=1;
	
	
	Write_ASF(0x150);//输出幅度设置函数，输入参数为0-3FFF，对应输出幅度0-480mv	
}

u32 Get_FTW(double Real_fH)
{
		return (u32)(fH_Num*Real_fH);
}

/*********************************************************************************************************
** 函数名称 ：void AD9954_SETFRE(float f)
** 函数功能 ：设置AD9954当前的频率输出，采用的是单一频率输出
** 函数说明 ：因为采用的浮点数进行计算，转换过程中会出现误差，通过调整可以精确到0.1Hz以内
** 入口参数 ：欲设置的频率值
** 出口参数 ：无
*********************************************************************************************************/
void AD9954_SETFRE(double f)//single tone
{
	u32 date;
	AD9954_CS = 0;
	
	date = Get_FTW(f);//date=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(FTW0);//FTW0地址
	AD9954_SENDBYTE((u8)(date >> 24));//频率控制字
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
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(0x01);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0xA4);//4倍频，打开VCO控制高位，系统时钟倍频后为400M
	AD9954_SETFRE(f);//载波频率
	PS0 = 0;
	PS1 = 0;
	AD9954_SENDBYTE(0x07);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0工作于模式0,不停留位没有激活
	UPDATE();
	date = 45.51*phase1;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//频率控制字
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
	AD9954_SENDBYTE(0x00);//RAM1工作于模式0,不停留位没有激活
	UPDATE();
	date = 45.51*phase2;
	date = date << 2;
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE((u8)(date >> 8));//频率控制字
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
	AD9954_SENDBYTE((u8)(date >> 8));//频率控制字
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
	AD9954_SENDBYTE((u8)(date >> 8));//频率控制字
	AD9954_SENDBYTE((u8)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0xa0);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 ：void Generate_PSK(float f,u16 *phase)
** 函数功能 ：PSK调制信号输出
** 函数说明 ：通过RAM端来控制信号的相位，从而实现PSK信号的调制
**            这里使用一个RAM段的循环模式，PSK信号为8为数据位。
**            RAM的更新速度由ram rate定时寄存器控制，寄存器为16位，实现对系统提供的100M时钟1~65535分频，写0是无效的
**			  当跟输出PSK载波的频率是ram rate跟新频率的整数倍时，才能保证每次的相位改变一致
** 入口参数 ：float f   ：PSK载波的频率
** 			  u16 *phase: 写入相位累加器的相位值，180实现对相位翻转，0不改变相位，PSK信号为8位，需为8位的数组
** 出口参数 ：无
*********************************************************************************************************/
void Generate_PSK(float f, u16 *phase)
{
	u8 i;
	u16 date;
	AD9954_CS = 0;
	IOUPDATE = 0;

	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(0x01);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0xA4);//4倍频，打开VCO控制高位，系统时钟倍频后为400M
	AD9954_SETFRE(f);//载波频率

	AD9954_SENDBYTE(0x04);//FTW0地址
	AD9954_SENDBYTE(0x00);//频率控制字
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
	AD9954_SENDBYTE(0x80);//RAM0工作于模式4,不停留位没有激活
	UPDATE();

	AD9954_SENDBYTE(0x0b);
	for (i = 0;i<8;i++)
	{
		date = 45.51*phase[i];
		date = date << 2;
		AD9954_SENDBYTE((u8)(date >> 8));//频率控制字
		AD9954_SENDBYTE((u8)date);
		AD9954_SENDBYTE(0x00);
		AD9954_SENDBYTE(0x00);
	}
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0xc0);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 ：void Generate_FM(u32 *fre)
** 函数功能 ：FM调制信号输出
** 函数说明 ：通过RAM端来控制信号的频率，从而实现FM信号的调制
**            这里使用一个RAM段的循环模式，PSK信号为8为数据位。
**            RAM的更新速度由ram rate定时寄存器控制，寄存器为16位，实现对系统提供的100M时钟1~65535分频，写0是无效的
**			  ram rate的跟新速度为调制信号的频率，这里如1KHz，写入ram rate的值为0X0C35,
** 入口参数 ：u32 *fre：FM信号频率值，这里采用的是32点频率采样，调用此函数之前需先设置好频率表的值，其按正弦规律改变
**            for(i=0;i<32;i++)
**				{
**					fre[i]=Fc+Fshift*sin(wt);
**				}
** 出口参数 ：无
*********************************************************************************************************/
void Generate_FM(u32 *fre)
{
	u8 i;
	u32 date;
	AD9954_CS = 0;
	IOUPDATE = 0;

	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);

	AD9954_SENDBYTE(0x01);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x24);//4倍频，打开VCO控制高位，系统时钟倍频后为400M

	PS0 = 0;
	PS1 = 0;
	AD9954_SENDBYTE(0x07);
	AD9954_SENDBYTE(0x35);//ramp rate=32kHz
	AD9954_SENDBYTE(0x0C);
	AD9954_SENDBYTE(0x1F);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0工作于模式0,不停留位没有激活
	UPDATE();

	AD9954_SENDBYTE(0x0b);
	for (i = 0;i<32;i++)
	{
		date = 10.73741*fre[i];
		AD9954_SENDBYTE((u8)(date >> 24));//频率控制字
		AD9954_SENDBYTE((u8)(date >> 16));
		AD9954_SENDBYTE((u8)(date >> 8));
		AD9954_SENDBYTE((u8)date);
	}
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x80);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** 函数名称 ：void ASK_Init(void)
** 函数功能 ：ASK调制信号输出初始化
** 函数说明 ：ASK调制，需要先设置好载波频率，然后改变DAC比例因子 scale factor其为14位，通过设置为最大和0两种值实现ASK信号的调制
**			  调用Write_ASF(u16 factor)来改变幅值
** 入口参数 ：无
** 出口参数 ：无
****************************************************** ***************************************************/
void ASK_Init(void)
{
	AD9954_CS = 0;
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//当受到UPDATE信号，自动清楚相位累加器		//
						  //	AD9954_SENDBYTE(0x00);//不改变任何

	AD9954_SENDBYTE(0x40);

	//控制相位偏移，因默认输出是余弦波，所以需控制相位累加器的累加相位为270
	AD9954_SENDBYTE(0X05);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);

	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** 函数名称 ：void AM_Init(void)
** 函数功能 ：AM调制信号输出初始化
** 函数说明 ：AM调制，需要先设置好载波频率，然后改变DAC比例因子 scale factor其为14位,AM的幅度值按正弦规律变换，可以实现AM
**            最高可以采用32点采样
**			  调用Write_ASF(u16 factor)来改变幅值
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/
void AM_Init(void)
{
	AD9954_CS = 0;
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//不改变任何
	AD9954_SENDBYTE(0x40);

	AD9954_CS = 1;
	UPDATE();
}
/*********************************************************************************************************
** 函数名称 ：void Write_ASF(u16 factor)
** 函数功能 ：改变scale factor数值，改变DAC输出幅度
** 函数说明 ：写入最大为0X3FFF,最小为0
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************/
void Write_ASF(u16 factor)  //2ASK
{
	AD9954_CS = 0;
	/*	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//当受到UPDATE信号，自动清楚相位累加器		//
	//	AD9954_SENDBYTE(0x00);//不改变任何

	AD9954_SENDBYTE(0x40);

	//控制相位偏移
	AD9954_SENDBYTE(0X05);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);	  */

	AD9954_SENDBYTE(0x02);//幅度	
	AD9954_SENDBYTE(factor >> 8);
	AD9954_SENDBYTE(factor);
	AD9954_CS = 1;
	UPDATE();

}
/******************************************************************
*  名    称：Linear_Sweep
*  功    能：AD9954扫频模式输出
*  参    数：MinFreq：扫频下限频率，MaxFreq：扫频上限频率
*            UpStepFreq：向上扫频步进，UpStepTime：向上扫频的跳频时间
*            DownStepFreq：向下扫频步进，DownStepTime：向下扫频的跳频时间
*            Mode：扫频模式 
*  返 回 值：无 Linear_Sweep(100, 100000000, 100,  250,   100,250, 1);
*  说    明：与频率相关的参数的单位都是 MHz
*            Mode=DoubleScan(双边扫频)时，PS0=1，即为向上扫频,但配置了
*            向下扫频的寄存器,可以直接通过PS0_WriteBit()来改变扫频方向
******************************************************************/
void Linear_Sweep( double  MinFreq, double MaxFreq, double  UpStepFreq, u8 UpStepTime,double  DownStepFreq, u8 DownStepTime,ScanMode Mode )       //0向下扫频模式，1向上扫频模式，2双边扫频

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
    
    //写入FTW0----最小频率
    FTW_Vau =  Get_FTW(MinFreq);;
    AD9954_SENDBYTE(FTW0);
    AD9954_SENDBYTE(FTW_Vau>>24);
    AD9954_SENDBYTE(FTW_Vau>>16);
    AD9954_SENDBYTE(FTW_Vau>>8);
    AD9954_SENDBYTE(FTW_Vau);
    
    //写入FTW1----最大频率
    FTW_Vau = Get_FTW(MaxFreq);
    AD9954_SENDBYTE(FTW1);
    AD9954_SENDBYTE(FTW_Vau>>24);
    AD9954_SENDBYTE(FTW_Vau>>16);
    AD9954_SENDBYTE(FTW_Vau>>8);
    AD9954_SENDBYTE(FTW_Vau);
    
    Mode &= 0x7F;
    
    if(Mode != UpScan)
    {
        //写入NLSCW----下降频率步进和跳频时间
        FTW_Vau = Get_FTW(DownStepFreq);
        AD9954_SENDBYTE(NLSCW);
        AD9954_SENDBYTE(DownStepTime); //跳频时间（DownStepTime个？？周期）     
        AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
        AD9954_SENDBYTE(FTW_Vau>>16);
        AD9954_SENDBYTE(FTW_Vau>>8);
        AD9954_SENDBYTE(FTW_Vau);
        PS0=0;                         //下降扫频
    }
    if (Mode != DownScan)
    {
        //写入PLSCW----上升频率步进和跳频时间
        FTW_Vau = Get_FTW(UpStepFreq);
        AD9954_SENDBYTE(PLSCW);
        AD9954_SENDBYTE(UpStepTime); //跳频时间（0XFF个？？周期）
        AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
        AD9954_SENDBYTE(FTW_Vau>>16);
        AD9954_SENDBYTE(FTW_Vau>>8);
        AD9954_SENDBYTE(FTW_Vau);
        PS0=1;                         //上升扫频
    }
    AD9954_CS=1;
		UPDATE();
}

/******************************************************************
*  名    称：PS0_WriteBit
*  功    能：设置PS0的状态
*  参    数：BitVal：PS0状态
*  返 回 值：无
*  说    明：PS0可以控制AD9954扫频方向
******************************************************************/
void PS0_WriteBit(BitAction BitVal)
{
    PS0=BitVal;
}

/*********************************************************************************************************
** 函数名称 ：void DirectSwitchFSK(float f1,float f2,float f3,float f4)
** 函数功能 ：四相FSK信号输出
** 函数说明 ：在四个RAM区各设置了一个频率值，通过改变PS0和PS1的电平选择对应的RAM端输出相应的频率值来实现FSK，也可以实现二项的FSK；
**            通过设置定时器中断控制PS0,PS1管脚的电平就可以将二进制的编码转化为FSK信号输出
** 入口参数 ：float f1:频率1
** 	      float f2:频率2
** 	      float f3:频率3
** 	      float f4:频率4
** 隐含控制   			PS0: 0   1	0	 1
** 管脚参数： 			PS1: 0   0	1	 1
** 对应控制 RAM段：		 0   1	2	 3
** 出口参数 ：无
*********************************************************************************************************/
void DirectSwitchFSK(double f1,double f2,double f3,double f4)   
{
	u32 FTW_Vau;
	AD9954_CS = 0;

	AD9954_SENDBYTE(0x00);//地址0写操作   
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x40);   

	AD9954_SENDBYTE(0x01);//地址1写操作   
	AD9954_SENDBYTE(0x00);//   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0xA4);//8倍频，打开VCO控制高位，系统时钟倍频后为400M   
	PS0=0;   
	PS1=0;   
	AD9954_SENDBYTE(0x07);   
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);//final address:0x000   
	AD9954_SENDBYTE(0x00);//start address:0x000;   
	AD9954_SENDBYTE(0x00);//RAM0工作于模式0,不停留位没有激活   
	UPDATE();   
	FTW_Vau=Get_FTW(f1);
	AD9954_SENDBYTE(0x0b);
	AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
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
	AD9954_SENDBYTE(0x00);//RAM1工作于模式0,不停留位没有激活   
	UPDATE();
	FTW_Vau=Get_FTW(f2);   
	AD9954_SENDBYTE(0x0b);   
	AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
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
	AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
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
	AD9954_SENDBYTE(FTW_Vau>>24);  //频率步进
	AD9954_SENDBYTE(FTW_Vau>>16);
	AD9954_SENDBYTE(FTW_Vau>>8);
	AD9954_SENDBYTE(FTW_Vau); 

	AD9954_SENDBYTE(0x00);//地址0写操作   
	AD9954_SENDBYTE(0x80);//打开RAM控制位驱动FTW   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x00);   
	AD9954_SENDBYTE(0x40);   
	AD9954_CS = 1;
	UPDATE();
}   
/*********************************************************************************************************
** 函数名称 ：void LINEARSWEEP(float f1,float f2)
** 函数功能 ：线性扫面输出模式
** 函数说明 ：使频率按预置的模式线性扫描上去，详细参见官方PDF
** 入口参数 ：float f1:起始频率
** 			  float f2:终止频率
** 出口参数 ：无
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
	AD9954_SENDBYTE(0x00);//地址0写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x20);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	

	AD9954_SENDBYTE(0x01);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x24);//4倍频，打开VCO控制高位，系统时钟倍频后为400M
AD9954_SENDBYTE(PLL_MULTIPLIER<<3|0x04);
	date=10.7374*f1;
	AD9954_SENDBYTE(0x04);//FTW0地址
	AD9954_SENDBYTE((u8)(date>>24));//频率控制字
	AD9954_SENDBYTE((u8)(date>>16));
	AD9954_SENDBYTE((u8)(date>>8));
	AD9954_SENDBYTE((u8)date);
	date=10.7374*f2;
	AD9954_SENDBYTE(0x06);//FTW1地址
	AD9954_SENDBYTE((u8)(date>>24));//频率控制字
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
