/*******************************************************************************
* @File     can.c
* @Brief    CAN驱动文件
* @Date     2019-09-17
* @Version  V2.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "can.h"

CAN_HandleTypeDef   CAN1_Handler;   //CAN1句柄
CanTxMsgTypeDef     TxMessage;      //发送消息
CanRxMsgTypeDef     RxMessage;      //接收消息

/*******************************************************************************
* @Brief    CAN1初始化函数
* @Param    tsjw:重新同步跳跃时间单元.   范围:CAN_SJW_1TQ~CAN_SJW_4TQ
*           tbs2:时间段2的时间单元.      范围:CAN_BS2_1TQ~CAN_BS2_8TQ
*           tbs1:时间段1的时间单元.      范围:CAN_BS1_1TQ~CAN_BS1_16TQ
*           brp :波特率分频器.           范围:1~1024; tq=(brp)*tpclk1
*                波特率=Fpclk1/((tbs1+tbs2+1)*brp)，Fpclk1的时钟在初始化的时候设置为45M
*                如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_8tq,6,CAN_MODE_LOOPBACK);
*                则波特率为:45M/((6+8+1)*6)=500Kbps
*           mode:模式.                  普通模式:CAN_MODE_NORMAL,回环模式:CAN_MODE_LOOPBACK
* @Return   0,初始化OK
*           其他,初始化失败
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
u8 CAN1_Mode_Init(u32 tsjw, u32 tbs2, u32 tbs1, u16 brp, u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;

    CAN1_Handler.Instance = CAN1;        //寄存器基地址
    CAN1_Handler.pTxMsg = &TxMessage;    //发送消息
    CAN1_Handler.pRxMsg = &RxMessage;    //接收消息
    CAN1_Handler.Init.Prescaler = brp;   //分频系数(Fdiv)为brp+1
    CAN1_Handler.Init.Mode = mode;       //模式设置 
    CAN1_Handler.Init.SJW = tsjw;        //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1 = tbs1;        //tbs1范围 CAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2 = tbs2;        //tbs2范围 CAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM = DISABLE;    //非时间触发通信模式 
    CAN1_Handler.Init.ABOM = DISABLE;    //软件自动离线管理
    CAN1_Handler.Init.AWUM = DISABLE;    //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN1_Handler.Init.NART = ENABLE;     //禁止报文自动传送 
    CAN1_Handler.Init.RFLM = DISABLE;    //报文不锁定,新的覆盖旧的 
    CAN1_Handler.Init.TXFP = DISABLE;    //优先级由报文标识符决定 

    if (HAL_CAN_Init(&CAN1_Handler) != HAL_OK) return 1;    //初始化

    CAN1_FilerConf.FilterIdHigh = 0X0000;                   //32位ID
    CAN1_FilerConf.FilterIdLow = 0X0000;
    CAN1_FilerConf.FilterMaskIdHigh = 0X0000;               //32位MASK
    CAN1_FilerConf.FilterMaskIdLow = 0X0000;
    CAN1_FilerConf.FilterFIFOAssignment = CAN_FILTER_FIFO0; //过滤器0 关联到FIFO0
    CAN1_FilerConf.FilterNumber = 0;                        //过滤器0
    CAN1_FilerConf.FilterMode = CAN_FILTERMODE_IDMASK;      //标识符屏蔽位模式
    CAN1_FilerConf.FilterScale = CAN_FILTERSCALE_32BIT;     //位宽1个32位
    CAN1_FilerConf.FilterActivation = ENABLE;               //激活滤波器0
    CAN1_FilerConf.BankNumber = 14;                         //CAN2 起始存储区

    if (HAL_CAN_ConfigFilter(&CAN1_Handler, &CAN1_FilerConf) != HAL_OK) return 2;    //滤波器初始化

    return 0;
}

/*******************************************************************************
* @Brief    CAN底层驱动函数
* @Param    hcan:CAN句柄
* @Return   None
* @Date     2019-09-17
* @Version  V2.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           此函数会被HAL_CAN_Init()调用
* @Author   Jove
*******************************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_CAN1_CLK_ENABLE();                    //使能CAN1时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();                   //开启GPIOA时钟

    GPIO_Initure.Pin = GPIO_PIN_11 | GPIO_PIN_12;   //PA11,12
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;            //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;                //上拉
    GPIO_Initure.Speed = GPIO_SPEED_FAST;           //快速
    GPIO_Initure.Alternate = GPIO_AF9_CAN1;         //复用为CAN1
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);            //初始化

#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&CAN1_Handler, CAN_IT_FMP0);//FIFO0消息挂起中断允许
    //CAN1->IER|=1<<1;                              //FIFO0消息挂起中断允许
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 2);      //抢占优先级1，子优先级2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);              //使能中断
#endif    
}

#if CAN1_RX0_INT_ENABLE                             //使能RX0中断
/*******************************************************************************
* @Brief    CAN中断服务函数
* @Param    None
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CAN1_Handler);             //此函数会调用CAN_Receive_IT()接收数据
}

/*******************************************************************************
* @Brief    CAN中断处理过程函数
* @Param    *hcan:CAN句柄
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
            此函数会被CAN_Receive_IT()调用
* @Author   Jove
*******************************************************************************/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i = 0;

    //CAN_Receive_IT()函数会关闭FIFO0消息挂号中断，因此需要重新打开
    __HAL_CAN_ENABLE_IT(&CAN1_Handler, CAN_IT_FMP0);//重新开启FIF00消息挂号中断

    //显示CAN接受的数据
    printf("\r\n接受的数据：");
    for (i = 0; i < 8; i++)
        printf("%3d ", CAN1_Handler.pRxMsg->Data[i]);
}
#endif    

/*******************************************************************************
* @Brief    CAN1发送数据组函数
* @Param    msg:数据指针,最大为8个字节
*           len:数据长度,最大为8
* @Return   0,成功
*           其他,失败
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           固定格式:ID为0X46,标准帧,数据帧
* @Author   Jove
*******************************************************************************/
u8 CAN1_Send_Msg(u8* msg, u8 len)
{
    u16 i = 0;
    CAN1_Handler.pTxMsg->StdId = 0X46;            //标准标识符
    CAN1_Handler.pTxMsg->ExtId = 0X46;            //扩展标识符(29位)
    CAN1_Handler.pTxMsg->IDE = CAN_ID_STD;        //使用标准帧
    CAN1_Handler.pTxMsg->RTR = CAN_RTR_DATA;      //数据帧
    CAN1_Handler.pTxMsg->DLC = len;               //数据长度代码
    for (i = 0; i < len; i++)                     //载入发送数据
        CAN1_Handler.pTxMsg->Data[i] = msg[i];
    if (HAL_CAN_Transmit(&CAN1_Handler, 10) != HAL_OK) return 1;    //发送
    return 0;
}

/*******************************************************************************
* @Brief    CAN1接收数据查询函数
* @Param    buf:数据缓存区
* @Return   0,无数据被收到
*           其他,接收的数据长度
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
u8 CAN1_Receive_Msg(u8* buf)
{
    u32 i;
    if (HAL_CAN_Receive(&CAN1_Handler, CAN_FIFO0, 0) != HAL_OK) return 0;    //接收数据
    for (i = 0; i < CAN1_Handler.pRxMsg->DLC; i++)                           //载入接收数据    
        buf[i] = CAN1_Handler.pRxMsg->Data[i];
    return CAN1_Handler.pRxMsg->DLC;
}
