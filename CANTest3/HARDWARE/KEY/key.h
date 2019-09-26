/*******************************************************************************
* @File     key.h
* @Brief    KEY����ͷ�ļ�
* @Date     2019-09-08
* @Version  V1.0
* @Note     ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
* @Author   Jove
*******************************************************************************/

#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY0    PHin(3)     //KEY0����PH3
#define KEY1    PHin(2)     //KEY1����PH2

#define NO_KEY      0
#define KEY0_PRES   1
#define KEY1_PRES   2

void KEY_Init(void);        //������ʼ������
u8 KEY_Scan(u8 mode);       //����������
#endif
