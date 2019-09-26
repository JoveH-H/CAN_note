/*******************************************************************************
* @File     led.h
* @Brief    LED驱动头文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LED0 PBout(1)   //LED0
#define LED1 PBout(0)   //LED1

void LED_Init(void);    //LED初始化函数
#endif
