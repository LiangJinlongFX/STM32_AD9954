#ifndef __USART6_H
#define __USART6_H

#include "sys.h"

#define USART6_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART6_RX					0			//1-为使能串口接收端																		//0-反之
																		
extern u8  USART6_RX_BUF[USART6_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA_6;         		//接收状态标记	

void usart6_Init(void);
void Send_data(u8 *data,u8 length);

#endif

