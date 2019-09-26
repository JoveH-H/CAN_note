/*******************************************************************************
* @File     bsp_timer.c
* @Brief    ��ʱ���ж������ļ�
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#include "bsp_timer.h"

TIM_HandleTypeDef TIM5_Handler;      //��ʱ����� 

extern void timerForCan(void);

/*******************************************************************************
* @Brief    ͨ�ö�ʱ��5�жϳ�ʼ������
* @Param    arr:�Զ���װֵ
*           psc:ʱ��Ԥ��Ƶ��
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           ��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us
* @Author   Jove
*******************************************************************************/
void TIM5_Init(u16 arr, u16 psc)
{
    TIM5_Handler.Instance = TIM5;                                //ͨ�ö�ʱ��5
    TIM5_Handler.Init.Prescaler = psc;                           //��Ƶϵ��
    TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;          //���ϼ�����
    TIM5_Handler.Init.Period = arr;                              //�Զ�װ��ֵ
    TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;    //ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM5_Handler);

    HAL_TIM_Base_Start_IT(&TIM5_Handler);            //ʹ�ܶ�ʱ��5�Ͷ�ʱ��5�����ж�
}

/*******************************************************************************
* @Brief    ��ʱ���ײ���������
* @Param    *htim:��ʱ�����
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
*           �˺����ᱻHAL_TIM_Base_Init()��������
* @Author   Jove
*******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();                //ʹ��TIM5ʱ��
        HAL_NVIC_SetPriority(TIM5_IRQn, 1, 5);      //�����ж����ȼ�����ռ���ȼ�1�������ȼ�5
        HAL_NVIC_EnableIRQ(TIM5_IRQn);              //����ITM5�ж�   
    }
}

/*******************************************************************************
* @Brief    ��ʱ��5�жϷ�����
* @Param    None
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

/*******************************************************************************
* @Brief    ��ʱ���жϻص���������
* @Param    *htim:��ʱ�����
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim == (&TIM5_Handler))
    {
        timerForCan();
    }
}
