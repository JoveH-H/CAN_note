/*******************************************************************************
* @File     bsp_can.h
* @Brief    CAN����ͷ�ļ�
* @Date     2019-09-19
* @Version  V3.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#ifndef __BSP_CAN_H
#define __BSP_CAN_H
#include "sys.h"

#define CAN1_RX0_INT_ENABLE    1        //CAN1����RX0�ж�ʹ��
#define CanOpen_Data TestMaster_Data    //�������ӻ� TestMaster_Data TestSlave_Data
#define CanOpenNodeID 0x00              //�ڵ�ID

/**
  * @brief  CAN Tx message structure definition
  */
typedef struct
{
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
} CanTxMsg;

/**
  * @brief  CAN Rx message structure definition
  */
typedef struct
{
    uint32_t StdId;
    uint32_t ExtId;
    uint8_t IDE;
    uint8_t RTR;
    uint8_t DLC;
    uint8_t Data[8];
    uint8_t FMI;
} CanRxMsg;

extern CAN_HandleTypeDef CAN1_Handler;  //CAN1���

u8 CAN1_Mode_Init(u32 tsjw, u32 tbs2, u32 tbs1, u16 brp, u32 mode); //CAN1��ʼ������
u8 CAN1_Send_Msg(u8* msg, u8 len);  //CAN1���������麯��
u8 CAN1_Receive_Msg(u8* buf);       //CAN1�������ݲ�ѯ����
#endif
