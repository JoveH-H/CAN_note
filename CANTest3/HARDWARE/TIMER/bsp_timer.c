/*******************************************************************************
* @File     bsp_timer.c
* @Brief    定时器中断驱动文件
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#include "bsp_timer.h"

TIM_HandleTypeDef TIM5_Handler;      //定时器句柄 

extern void timerForCan(void);

/*******************************************************************************
* @Brief    通用定时器5中断初始化函数
* @Param    arr:自动重装值
*           psc:时钟预分频数
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us
* @Author   Jove
*******************************************************************************/
void TIM5_Init(u16 arr, u16 psc)
{
    TIM5_Handler.Instance = TIM5;                                //通用定时器5
    TIM5_Handler.Init.Prescaler = psc;                           //分频系数
    TIM5_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;          //向上计数器
    TIM5_Handler.Init.Period = arr;                              //自动装载值
    TIM5_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;    //时钟分频因子
    HAL_TIM_Base_Init(&TIM5_Handler);

    HAL_TIM_Base_Start_IT(&TIM5_Handler);            //使能定时器5和定时器5更新中断
}

/*******************************************************************************
* @Brief    定时器底册驱动函数
* @Param    *htim:定时器句柄
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
*           此函数会被HAL_TIM_Base_Init()函数调用
* @Author   Jove
*******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();                //使能TIM5时钟
        HAL_NVIC_SetPriority(TIM5_IRQn, 1, 5);      //设置中断优先级，抢占优先级1，子优先级5
        HAL_NVIC_EnableIRQ(TIM5_IRQn);              //开启ITM5中断   
    }
}

/*******************************************************************************
* @Brief    定时器5中断服务函数
* @Param    None
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

/*******************************************************************************
* @Brief    定时器中断回调函数函数
* @Param    *htim:定时器句柄
* @Return   None
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim == (&TIM5_Handler))
    {
        timerForCan();
    }
}
