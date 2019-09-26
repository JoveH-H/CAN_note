/*******************************************************************************
* @File     key.h
* @Brief    KEY驱动头文件
* @Date     2019-09-08
* @Version  V1.0
* @Note     本程序只供学习使用，未经作者许可，不得用于其它任何用途
* @Author   Jove
*******************************************************************************/

#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY0    PHin(3)     //KEY0按键PH3
#define KEY1    PHin(2)     //KEY1按键PH2

#define NO_KEY      0
#define KEY0_PRES   1
#define KEY1_PRES   2

void KEY_Init(void);        //按键初始化函数
u8 KEY_Scan(u8 mode);       //按键处理函数
#endif
