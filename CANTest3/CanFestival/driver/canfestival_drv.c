/*******************************************************************************
* @File     canfestival_drv.c
* @Brief    canfestival���������ļ�
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#include "sys.h"
#include <stdbool.h>
#include "canfestival.h"
#include "bsp_can.h"

unsigned int TimeCNT = 0;                    //ʱ�����
unsigned int NextTime = 0;                   //��һ�δ���ʱ�����
unsigned int TIMER_MAX_COUNT = 60000;        //���ʱ�����60s
static TIMEVAL last_time_set = TIMEVAL_MAX;  //��һ�ε�ʱ�����

/*******************************************************************************
* @Brief    ��ʱ���Ⱥ���
* @Param    value:��ʱʱ����
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
            ��Դ���������ö�ʱ��ʱ�䵽�˾���Ҫ����һ�º���TimeDispatch()
* @Author   Jove
*******************************************************************************/
void setTimer(TIMEVAL value)
{
    NextTime = (TimeCNT + value) % TIMER_MAX_COUNT;
}

/*******************************************************************************
* @Brief    ��ѯ���Ⱥ���
* @Param    None
* @Return   ret:ʣ��ʱ��
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
            ��ѯ������һ����ʱ�������ж���ʱ��
* @Author   Jove
*******************************************************************************/
TIMEVAL getElapsedTime(void)
{
    int ret = 0;
    ret = TimeCNT > last_time_set ? TimeCNT - last_time_set : TimeCNT + TIMER_MAX_COUNT - last_time_set;
    last_time_set = TimeCNT;
    return ret;
}

/*******************************************************************************
* @Brief    CAN���ͺ���
* @Param    notused:δʹ�ò���
            *m:     ��Ϣ����
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
unsigned char canSend(CAN_PORT notused, Message* m)
{
    u32    i;

    CAN1_Handler.pTxMsg->StdId = m->cob_id;
    if (m->rtr)
        CAN1_Handler.pTxMsg->RTR = CAN_RTR_REMOTE;
    else
        CAN1_Handler.pTxMsg->RTR = CAN_RTR_DATA;

    CAN1_Handler.pTxMsg->IDE = CAN_ID_STD;
    CAN1_Handler.pTxMsg->DLC = m->len;
    printf("\r\nm->cob_id=%x\r\n", m->cob_id);
    for (i = 0; i < m->len; i++)
        CAN1_Handler.pTxMsg->Data[i] = m->data[i];

    if (HAL_CAN_Transmit(&CAN1_Handler, 10) == HAL_OK)
    {
        printf("���ͳɹ�\r\n");
        return 0xff;
    }
    else
    {
        printf("����ʧ��\r\n");
        return 0x00;
    }
}

/*******************************************************************************
* @Brief    ʱ��������㺯��
* @Param    None
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
            ÿ1�������һ���������
* @Author   Jove
*******************************************************************************/
void timerForCan(void)
{
    TimeCNT++;
    if (TimeCNT >= TIMER_MAX_COUNT)
    {
        TimeCNT = 0;
    }
    if (TimeCNT == NextTime)
    {
        TimeDispatch();
    }
}
