#include "usart6.h"

u16 USART_RX_STA_6=0;       //接收状态标记
u8 USART6_RX_BUF[USART6_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

void usart6_Init()
{
		 //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART6时钟
 
	//串口6对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC6复用为USART6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7复用为USART6
	
	GPIO_StructInit(&GPIO_InitStructure);//缺省值填入
	
	//USART6端口配置   PC6为输出  PC7为输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC6与GPIOC7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC6  PC7

   //USART6 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART6, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART6, ENABLE);  //使能串口1

#if EN_USART6_RX
//开启串口相关中断
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=4;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =4;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
}

void USART6_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART6);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA_6&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA_6&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA_6=0;//接收错误,重新开始
				else USART_RX_STA_6|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA_6|=0x4000;
				else
				{
					USART6_RX_BUF[USART_RX_STA_6&0X3FFF]=Res ;
					USART_RX_STA_6++;
					if(USART_RX_STA_6>(USART6_REC_LEN-1))USART_RX_STA_6=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
  } 
}

void Send_data(u8 *data,u8 length)
{
	u8 i,j;
	u8 End[2]={'\r','\n'};
	
	for(i=0;i<length;i++)
	{
		USART_SendData(USART6, data[i]);         //向串口6发送数据
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);//等待发送结束
		j=255;
		while(j--);
	}
	
	USART_SendData(USART6, End[0]);         //向串口6发送数据
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);//等待发送结束	

	USART_SendData(USART6, End[1]);         //向串口6发送数据
	while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);//等待发送结束		
	
}
	
