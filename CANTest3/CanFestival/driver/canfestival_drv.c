/*******************************************************************************
* @File     canfestival_drv.c
* @Brief    canfestival附加配置文件
* @Date     2019-09-19
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "sys.h"
#include <stdbool.h>
#include "canfestival.h"
#include "bsp_can.h"

unsigned int TimeCNT = 0;                    //时间计数
unsigned int NextTime = 0;                   //下一次触发时间计数
unsigned int TIMER_MAX_COUNT = 60000;        //最大时间计数60s
static TIMEVAL last_time_set = TIMEVAL_MAX;  //上一次的时间计数

/*******************************************************************************
* @Brief    定时调度函数
* @Param    value:定时时间数
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
            被源码用来设置定时，时间到了就需要调用一下函数TimeDispatch()
* @Author   Jove
*******************************************************************************/
void setTimer(TIMEVAL value)
{
    NextTime = (TimeCNT + value) % TIMER_MAX_COUNT;
}

/*******************************************************************************
* @Brief    查询调度函数
* @Param    None
* @Return   ret:剩余时间
* @Date     2019-09-19
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
            查询距离下一个定时触发还有多少时间
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
* @Brief    CAN发送函数
* @Param    notused:未使用参数
            *m:     消息参数
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
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
        printf("发送成功\r\n");
        return 0xff;
    }
    else
    {
        printf("发送失败\r\n");
        return 0x00;
    }
}

/*******************************************************************************
* @Brief    时间计数运算函数
* @Param    None
* @Return   None
* @Date     2019-09-19
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
            每1毫秒调用一下这个函数
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
