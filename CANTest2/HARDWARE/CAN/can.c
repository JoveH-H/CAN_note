/*******************************************************************************
* @File     can.c
* @Brief    CAN�����ļ�
* @Date     2019-09-17
* @Version  V2.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#include "can.h"

CAN_HandleTypeDef   CAN1_Handler;   //CAN1���
CanTxMsgTypeDef     TxMessage;      //������Ϣ
CanRxMsgTypeDef     RxMessage;      //������Ϣ

/*******************************************************************************
* @Brief    CAN1��ʼ������
* @Param    tsjw:����ͬ����Ծʱ�䵥Ԫ.   ��Χ:CAN_SJW_1TQ~CAN_SJW_4TQ
*           tbs2:ʱ���2��ʱ�䵥Ԫ.      ��Χ:CAN_BS2_1TQ~CAN_BS2_8TQ
*           tbs1:ʱ���1��ʱ�䵥Ԫ.      ��Χ:CAN_BS1_1TQ~CAN_BS1_16TQ
*           brp :�����ʷ�Ƶ��.           ��Χ:1~1024; tq=(brp)*tpclk1
*                ������=Fpclk1/((tbs1+tbs2+1)*brp)��Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ45M
*                �������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_8tq,6,CAN_MODE_LOOPBACK);
*                ������Ϊ:45M/((6+8+1)*6)=500Kbps
*           mode:ģʽ.                  ��ͨģʽ:CAN_MODE_NORMAL,�ػ�ģʽ:CAN_MODE_LOOPBACK
* @Return   0,��ʼ��OK
*           ����,��ʼ��ʧ��
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
u8 CAN1_Mode_Init(u32 tsjw, u32 tbs2, u32 tbs1, u16 brp, u32 mode)
{
    CAN_FilterConfTypeDef  CAN1_FilerConf;

    CAN1_Handler.Instance = CAN1;        //�Ĵ�������ַ
    CAN1_Handler.pTxMsg = &TxMessage;    //������Ϣ
    CAN1_Handler.pRxMsg = &RxMessage;    //������Ϣ
    CAN1_Handler.Init.Prescaler = brp;   //��Ƶϵ��(Fdiv)Ϊbrp+1
    CAN1_Handler.Init.Mode = mode;       //ģʽ���� 
    CAN1_Handler.Init.SJW = tsjw;        //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.BS1 = tbs1;        //tbs1��Χ CAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.BS2 = tbs2;        //tbs2��Χ CAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TTCM = DISABLE;    //��ʱ�䴥��ͨ��ģʽ 
    CAN1_Handler.Init.ABOM = DISABLE;    //����Զ����߹���
    CAN1_Handler.Init.AWUM = DISABLE;    //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN1_Handler.Init.NART = ENABLE;     //��ֹ�����Զ����� 
    CAN1_Handler.Init.RFLM = DISABLE;    //���Ĳ�����,�µĸ��Ǿɵ� 
    CAN1_Handler.Init.TXFP = DISABLE;    //���ȼ��ɱ��ı�ʶ������ 

    if (HAL_CAN_Init(&CAN1_Handler) != HAL_OK) return 1;    //��ʼ��

    CAN1_FilerConf.FilterIdHigh = 0X0000;                   //32λID
    CAN1_FilerConf.FilterIdLow = 0X0000;
    CAN1_FilerConf.FilterMaskIdHigh = 0X0000;               //32λMASK
    CAN1_FilerConf.FilterMaskIdLow = 0X0000;
    CAN1_FilerConf.FilterFIFOAssignment = CAN_FILTER_FIFO0; //������0 ������FIFO0
    CAN1_FilerConf.FilterNumber = 0;                        //������0
    CAN1_FilerConf.FilterMode = CAN_FILTERMODE_IDMASK;      //��ʶ������λģʽ
    CAN1_FilerConf.FilterScale = CAN_FILTERSCALE_32BIT;     //λ��1��32λ
    CAN1_FilerConf.FilterActivation = ENABLE;               //�����˲���0
    CAN1_FilerConf.BankNumber = 14;                         //CAN2 ��ʼ�洢��

    if (HAL_CAN_ConfigFilter(&CAN1_Handler, &CAN1_FilerConf) != HAL_OK) return 2;    //�˲�����ʼ��

    return 0;
}

/*******************************************************************************
* @Brief    CAN�ײ���������
* @Param    hcan:CAN���
* @Return   None
* @Date     2019-09-17
* @Version  V2.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           �˺����ᱻHAL_CAN_Init()����
* @Author   Jove
*******************************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_CAN1_CLK_ENABLE();                    //ʹ��CAN1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();                   //����GPIOAʱ��

    GPIO_Initure.Pin = GPIO_PIN_11 | GPIO_PIN_12;   //PA11,12
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;            //���츴��
    GPIO_Initure.Pull = GPIO_PULLUP;                //����
    GPIO_Initure.Speed = GPIO_SPEED_FAST;           //����
    GPIO_Initure.Alternate = GPIO_AF9_CAN1;         //����ΪCAN1
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);            //��ʼ��

#if CAN1_RX0_INT_ENABLE
    __HAL_CAN_ENABLE_IT(&CAN1_Handler, CAN_IT_FMP0);//FIFO0��Ϣ�����ж�����
    //CAN1->IER|=1<<1;                              //FIFO0��Ϣ�����ж�����
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 2);      //��ռ���ȼ�1�������ȼ�2
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);              //ʹ���ж�
#endif    
}

#if CAN1_RX0_INT_ENABLE                             //ʹ��RX0�ж�
/*******************************************************************************
* @Brief    CAN�жϷ�����
* @Param    None
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CAN1_Handler);             //�˺��������CAN_Receive_IT()��������
}

/*******************************************************************************
* @Brief    CAN�жϴ�����̺���
* @Param    *hcan:CAN���
* @Return   None
* @Date     2019-09-17
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
            �˺����ᱻCAN_Receive_IT()����
* @Author   Jove
*******************************************************************************/
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
    int i = 0;

    //CAN_Receive_IT()������ر�FIFO0��Ϣ�Һ��жϣ������Ҫ���´�
    __HAL_CAN_ENABLE_IT(&CAN1_Handler, CAN_IT_FMP0);//���¿���FIF00��Ϣ�Һ��ж�

    //��ʾCAN���ܵ�����
    printf("\r\n���ܵ����ݣ�");
    for (i = 0; i < 8; i++)
        printf("%3d ", CAN1_Handler.pRxMsg->Data[i]);
}
#endif    

/*******************************************************************************
* @Brief    CAN1���������麯��
* @Param    msg:����ָ��,���Ϊ8���ֽ�
*           len:���ݳ���,���Ϊ8
* @Return   0,�ɹ�
*           ����,ʧ��
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           �̶���ʽ:IDΪ0X46,��׼֡,����֡
* @Author   Jove
*******************************************************************************/
u8 CAN1_Send_Msg(u8* msg, u8 len)
{
    u16 i = 0;
    CAN1_Handler.pTxMsg->StdId = 0X46;            //��׼��ʶ��
    CAN1_Handler.pTxMsg->ExtId = 0X46;            //��չ��ʶ��(29λ)
    CAN1_Handler.pTxMsg->IDE = CAN_ID_STD;        //ʹ�ñ�׼֡
    CAN1_Handler.pTxMsg->RTR = CAN_RTR_DATA;      //����֡
    CAN1_Handler.pTxMsg->DLC = len;               //���ݳ��ȴ���
    for (i = 0; i < len; i++)                     //���뷢������
        CAN1_Handler.pTxMsg->Data[i] = msg[i];
    if (HAL_CAN_Transmit(&CAN1_Handler, 10) != HAL_OK) return 1;    //����
    return 0;
}

/*******************************************************************************
* @Brief    CAN1�������ݲ�ѯ����
* @Param    buf:���ݻ�����
* @Return   0,�����ݱ��յ�
*           ����,���յ����ݳ���
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
u8 CAN1_Receive_Msg(u8* buf)
{
    u32 i;
    if (HAL_CAN_Receive(&CAN1_Handler, CAN_FIFO0, 0) != HAL_OK) return 0;    //��������
    for (i = 0; i < CAN1_Handler.pRxMsg->DLC; i++)                           //�����������    
        buf[i] = CAN1_Handler.pRxMsg->Data[i];
    return CAN1_Handler.pRxMsg->DLC;
}
