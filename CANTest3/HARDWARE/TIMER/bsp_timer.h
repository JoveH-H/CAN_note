/*******************************************************************************
* @File     bsp_timer.h
* @Brief    定时器中断驱动头文件
* @Date     2019-09-18
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM5_Handler;  //定时器句柄 

void TIM5_Init(u16 arr, u16 psc);       //通用定时器5中断初始化函数
#endif
