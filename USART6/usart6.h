#ifndef __USART6_H
#define __USART6_H

#include "sys.h"

#define USART6_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART6_RX					0			//1-Ϊʹ�ܴ��ڽ��ն�																		//0-��֮
																		
extern u8  USART6_RX_BUF[USART6_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA_6;         		//����״̬���	

void usart6_Init(void);
void Send_data(u8 *data,u8 length);

#endif

