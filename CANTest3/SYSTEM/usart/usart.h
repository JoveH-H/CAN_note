/*******************************************************************************
* @File     usart.h
* @Brief    串口通讯驱动头文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	

#define USART_REC_LEN           200         //定义最大接收字节数 200
#define EN_USART1_RX            1           //使能（1）/禁止（0）串口1接收

extern u8  USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;                    //接收状态标记	
extern UART_HandleTypeDef USART1_Handler;   //UART句柄

#define RXBUFFERSIZE   1                    //缓存大小
extern u8 aRxBuffer[RXBUFFERSIZE];          //HAL库USART接收Buffer

void uart_init(u32 bound);                  //串口1初始化函数
#endif
