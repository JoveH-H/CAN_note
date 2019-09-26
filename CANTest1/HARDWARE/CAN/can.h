/*******************************************************************************
* @File     can.h
* @Brief    CAN驱动头文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

u8 CAN1_Mode_Init(u32 tsjw, u32 tbs2, u32 tbs1, u16 brp, u32 mode); //CAN1初始化函数
u8 CAN1_Send_Msg(u8* msg, u8 len);  //CAN1发送数据组函数
u8 CAN1_Receive_Msg(u8 *buf);       //CAN1接收数据查询函数
#endif
