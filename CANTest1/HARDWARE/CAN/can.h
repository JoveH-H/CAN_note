/*******************************************************************************
* @File     can.h
* @Brief    CAN����ͷ�ļ�
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#ifndef __CAN_H
#define __CAN_H
#include "sys.h"

u8 CAN1_Mode_Init(u32 tsjw, u32 tbs2, u32 tbs1, u16 brp, u32 mode); //CAN1��ʼ������
u8 CAN1_Send_Msg(u8* msg, u8 len);  //CAN1���������麯��
u8 CAN1_Receive_Msg(u8 *buf);       //CAN1�������ݲ�ѯ����
#endif
