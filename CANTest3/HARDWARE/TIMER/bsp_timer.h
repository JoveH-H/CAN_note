/*******************************************************************************
* @File     bsp_timer.h
* @Brief    ��ʱ���ж�����ͷ�ļ�
* @Date     2019-09-18
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H
#include "sys.h"

extern TIM_HandleTypeDef TIM5_Handler;  //��ʱ����� 

void TIM5_Init(u16 arr, u16 psc);       //ͨ�ö�ʱ��5�жϳ�ʼ������
#endif
